/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next
 * ========================== NEXT Project ==================================
 */
/**
 * @file     next_impl.cpp
 * @brief    functions implementation of the Next Component Interface
 *
 **/

#include "next_impl.hpp"

#include <algorithm>

#include <json/json.h>

#include <next/sdk/logger/logger.hpp>

using config = next::appsupport::ConfigClient;

namespace next {

NextComponentInterfaceImpl::NextComponentInterfaceImpl(const std::string &component_name)
    : component_name_(component_name), session_client_(component_name), next_inports_(component_name),
      next_outports(component_name), time_service_(component_name) {
  if (!next::appsupport::ConfigClient::getConfig()) {
    error(__FILE__, "NextComponentInterfaceImpl constructed without ConfigClient");
    next::appsupport::ConfigClient::do_init(component_name);
  }
  trigger_handler_ = std::make_shared<next::control::orchestrator::OrchestratorTriggerHandler>(component_name);
}

NextComponentInterfaceImpl::~NextComponentInterfaceImpl() { session_client_.StopSessionClient(); }

int NextComponentInterfaceImpl::SetupInput(const std::string &adapter_dll_path,
                                           const std::vector<ComponentInputPortConfig> &input_ports) {
  next_inports_.SetupInput(adapter_dll_path, input_ports);
  return 0;
}

uint32_t NextComponentInterfaceImpl::ConvertVersionNumber(const std::string &version) {
  next::udex::extractor::DataExtractor extractor;
  return extractor.ConvertVersion(version);
}

int NextComponentInterfaceImpl::SetupOutput(const std::vector<std::string> &sdl_filepath,
                                            const std::vector<ComponentOutputPortConfig> &output_ports) {
  next_outports.SetupOutput(sdl_filepath, output_ports);
  return 0;
}

int NextComponentInterfaceImpl::SetupTriggers(
    const std::vector<std::pair<std::string, triggerCallback>> &triggers,
    std::function<void(next::control::orchestrator::OrchestratorErrors &, std::string &, const std::string &,
                       const triggerCallback &)>
        callback_func) {
  for (auto it : triggers) {
    std::string method_name = it.first;
    triggerCallback trigger_callback = it.second;

    // give eco callback into OnTriggerCallback
    auto onTriggerBind =
        std::bind(callback_func, std::placeholders::_1, std::placeholders::_2, method_name, trigger_callback);
    trigger_handler_->registerHook(method_name, onTriggerBind);
  };
  return 0;
}

void NextComponentInterfaceImpl::OnTrigger(next::control::orchestrator::OrchestratorErrors &err, std::string &err_msg,
                                           const std::string &method_name, const triggerCallback &client_callback) {

  // acquire lock to synchronize and get information from trigger
  std::lock_guard<std::mutex> lock(current_trigger_mtx_);
  current_trigger_ = trigger_handler_->extractTriggerInfo(method_name);

  next_inports_.FillReferenceValueAndData(current_trigger_);

  time_service_.FillCurrentTimestampMicroseconds();

  std::string callback_err_msg;
  try {
    int eco_result = client_callback(callback_err_msg);
    // we prepend the return code of the callback to the message
    err_msg = "{\"callback_message\":\"" + callback_err_msg + std::string("\",\"error_code\":") +
              std::to_string(eco_result) + std::string("}");
    if (eco_result == 0) {
      err = next::control::orchestrator::OrchestratorErrors::SUCCESS;
    } else {
      err = next::control::orchestrator::OrchestratorErrors::EXEC_FAILED;
      next::appsupport::NextStatusCode::PublishStatusCode(component_name_, next::appsupport::StatusCode::WARN_COMP_EXEC,
                                                          "Exec function of component returned unsuccesful result");
    }
  } catch (...) {
    err = next::control::orchestrator::OrchestratorErrors::FW_FAILED;
    error(__FILE__, "Callback failed with exception during exec call for component {0} and method {1} with msg: {2}",
          this->component_name_, method_name, err_msg);
  }
}

int NextComponentInterfaceImpl::SetupSyncref(const std::vector<std::pair<std::string, SyncMapping>> &syncref_mapping) {
  // Defines the sync method for each Input port
  for (auto &it_sync_mapping : syncref_mapping) {
    config::getConfig()->put(
        next::appsupport::configkey::getSyncMemberNameSyncIdKey(component_name_, it_sync_mapping.first),
        it_sync_mapping.second.signal_url_syncid.synchronized_port_member_name);
    config::getConfig()->put(
        next::appsupport::configkey::getSyncMemberUrlSyncIdKey(component_name_, it_sync_mapping.first),
        it_sync_mapping.second.signal_url_syncid.syncref_signal_url);
    config::getConfig()->put(
        next::appsupport::configkey::getSyncMemberNameTimestampKey(component_name_, it_sync_mapping.first),
        it_sync_mapping.second.signal_url_timestamp.synchronized_port_member_name);
    config::getConfig()->put(
        next::appsupport::configkey::getSyncMemberUrlTimestampKey(component_name_, it_sync_mapping.first),
        it_sync_mapping.second.signal_url_timestamp.syncref_signal_url);
  }
  return 0;
}

// TODO Need to align on function name
// TODO !! ADD Description !!
DataPackage NextComponentInterfaceImpl::getTriggerAsyncPush(const std::string &input_port_name) {
  next::udex::TimestampedDataPacket synchronized_extracted_data = next_inports_.getSyncedPacket(input_port_name);
  return std::make_tuple(synchronized_extracted_data.data, synchronized_extracted_data.size);
}

bool NextComponentInterfaceImpl::isDataAvailable(const std::string &input_port_name) {
  return next_inports_.isDataAvailable(input_port_name);
}

void NextComponentInterfaceImpl::triggerAsyncPush(std::string output_port_name, DataPackage data, bool append_device) {
  // check if port wit name output_port_name is already available.
  next_outports.TriggerAsyncPush(output_port_name, data, append_device,
                                 std::stoull(current_trigger_.trigger_timestamp));
}

void NextComponentInterfaceImpl::triggerAsyncPush(const uint64_t virtual_address, DataPackage data,
                                                  [[maybe_unused]] bool append_device) {

  next_outports.TriggerAsyncPush(virtual_address, data, std::stoull(current_trigger_.trigger_timestamp));
}

std::vector<ComponentInputPortConfig> NextComponentInterfaceImpl::getInputPorts() {
  return next_inports_.GetInputPorts();
}
std::vector<ComponentOutputPortConfig> NextComponentInterfaceImpl::getOutputPorts() {
  return next_outports.GetOutports();
}

next::SimulationStatus NextComponentInterfaceImpl::getStatus() { return simulation_status_.getStatus(); }
uint64_t NextComponentInterfaceImpl::getSystemTimeMicroseconds() { return time_service_.getSystemTimeMicroseconds(); }

bool NextComponentInterfaceImpl::userEnterReset() {

  info(__FILE__, "Reset called for component: {0}", component_name_);
  next_inports_.Reset();
  next_outports.Reset();
  return true;
}

bool NextComponentInterfaceImpl::userEnterConfigOutput() {
  // move from SetupOutput create publishers and stuff
  next_outports.SetupSavedOutputConfig();

  info(__FILE__, "component {0} configured output", component_name_);
  return true;
}

bool NextComponentInterfaceImpl::userEnterConfigInput() {
  // move from Setupinput create publishers and stuff
  next_inports_.SetupSavedInputConfig();
  time_service_.SetupSystemService();

  info(__FILE__, "component {0} configured input", component_name_);
  return true;
}

bool NextComponentInterfaceImpl::isUpdated(const std::string &port_name) { return next_inports_.isUpdated(port_name); }

std::list<TriggerReturn> NextComponentInterfaceImpl::invokeTrigger(const std::string &node_name,
                                                                   const std::string &method_name,
                                                                   const std::string &timestamp,
                                                                   const std::string &flow_id) {
  std::list<std::string> responses;
  if (trigger_handler_)
    responses = trigger_handler_->invokeTrigger(node_name, method_name, timestamp, flow_id);
  return parseTriggerResponse(responses);
}

std::list<TriggerReturn> NextComponentInterfaceImpl::invokeTrigger(const std::string &node_name,
                                                                   const std::string &method_name) {
  std::list<std::string> responses;
  if (trigger_handler_)
    responses = trigger_handler_->invokeTrigger(node_name, method_name, current_trigger_.trigger_timestamp,
                                                current_trigger_.flow_id);
  return parseTriggerResponse(responses);
}

std::list<TriggerReturn> NextComponentInterfaceImpl::parseTriggerResponse(const std::list<std::string> &responses) {
  std::list<TriggerReturn> resp_ret;
  for (auto &resp : responses) {
    TriggerReturn trigger_ret;
    Json::Reader reader;
    Json::Value json_response;
    bool parseSuccess = reader.parse(resp, json_response, false);
    if (parseSuccess) {
      if (json_response.isMember("return_type") && json_response.isMember("callback_return")) {
        std::string ret_type = json_response["return_type"].asString();
        trigger_ret.error = getErrorCodeFromString(ret_type);
        if (json_response.isMember("callback_return")) {
          auto ret_data = json_response["callback_return"];
          trigger_ret.error_message = ret_data["callback_message"].asString();
          trigger_ret.SigState = ret_data["error_code"].asInt();
        }
      } else {
        trigger_ret.error = ErrorCode::PARSE_RESPONSE_FAIL;
        trigger_ret.SigState = -1;
        trigger_ret.error_message = "Unable to parse return json: " + resp;
      }
    } else {
      trigger_ret.error = ErrorCode::PARSE_RESPONSE_FAIL;
      trigger_ret.SigState = -1;
      trigger_ret.error_message = "Unable to parse return json: " + resp;
    }
    resp_ret.push_back(trigger_ret);
  }
  return resp_ret;
}

} // namespace next

/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     orchestrator_master.cpp
 * @brief    orchestrator for event handling for Next Player Control
 *           Orchestrator handles event requests and forwards them to the destined parties
 *           It uses Event Manager to do so and is agnostic how Event Manager handles the stuff.
 *
 **/

#include "orchestrator_master_impl.h"

#include <fstream>
#include <string>

#include <json/json.h>

#include <next/control/orchestrator/flow_control_config.h>
#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include <next/appsupport/next_status_code/next_status_code.hpp>
#include <next/sdk/logger/logger.hpp>
#include <next/udex/explorer/signal_explorer.hpp>

#include "base_ctrl_data_generator/control_data_generator.hpp"

using namespace next::appsupport;

namespace next {
namespace control {
namespace orchestrator {

using namespace next::control::events;

OrchestratorMasterImpl::OrchestratorMasterImpl() {}

OrchestratorMasterImpl::~OrchestratorMasterImpl() {

  for (auto &async_trigger : list_of_async_fired_triggers_) {
    async_trigger.join();
  }
}

void OrchestratorMasterImpl::resetOrchestrator() {
  for (auto &async_trigger : list_of_async_fired_triggers_) {
    async_trigger.join();
  }
  list_of_async_fired_triggers_.clear();
  hash_url_map_data_triggering_.clear();
  multi_map_data_triggering_.clear();
  multi_map_cycle_triggering_.clear();
  multi_map_timebased_triggering_.clear();
  map_cached_flows_.clear();
}

void OrchestratorMasterImpl::eraseFlow(std::multimap<std::string, FlowLeaf> &multi_map, const std::string &key,
                                       const FlowLeaf &leaf) {
  bool flow_erased{false};
  auto range = multi_map.equal_range(key);
  for (auto it = range.first; it != range.second;) {
    if (it->second == leaf) {
      it = multi_map.erase(it);
      flow_erased = true;
    } else {
      ++it;
    }
  }
  if (!flow_erased) {
    debug(__FILE__, "No flow was found for {0} to erase it from orchestrator. No flow removed.", key);
  }
}

void OrchestratorMasterImpl::unRegisterFlow(const FlowLeaf &flow_leaf) {
  switch (flow_leaf.trigger_.type) {
  case TriggerType::TIME_TRIGGER:
    eraseFlow(multi_map_timebased_triggering_, std::to_string(flow_leaf.trigger_.val.time), flow_leaf);
    break;
  case TriggerType::SIGNAL_TRIGGER: {
    std::string package_url{""};
    if (getPackageUrlFromSignalUrl(flow_leaf.trigger_.val.signal_name, package_url)) {
      eraseFlow(multi_map_data_triggering_, package_url, flow_leaf);
    } else {
      // try with existing name
      eraseFlow(multi_map_data_triggering_, flow_leaf.trigger_.val.signal_name, flow_leaf);
    }
    break;
  }
  case TriggerType::CYCLE_START_TRIGGER:
    eraseFlow(multi_map_cycle_triggering_, std::to_string(flow_leaf.trigger_.val.cycle_start), flow_leaf);
    break;
  case TriggerType::CYCLE_END_TRIGGER:
    eraseFlow(multi_map_cycle_triggering_, std::to_string(flow_leaf.trigger_.val.cycle_end), flow_leaf);
    break;
  default:
    warn(__FILE__, "Could not unregister flow as type {0} is unknown.", std::to_string(int(flow_leaf.trigger_.type)));
    break;
  }
}

void OrchestratorMasterImpl::registerFlow(const FlowLeaf &flow_leaf) {
  switch (flow_leaf.trigger_.type) {
  case TriggerType::TIME_TRIGGER:
    addFlow(multi_map_timebased_triggering_, std::to_string(flow_leaf.trigger_.val.time), flow_leaf);
    break;
  case TriggerType::SIGNAL_TRIGGER: {
    std::string package_url{""};
    if (getPackageUrlFromSignalUrl(flow_leaf.trigger_.val.signal_name, package_url)) {
      addFlow(multi_map_data_triggering_, package_url, flow_leaf);
    } else {
      addFlow(multi_map_data_triggering_, flow_leaf.trigger_.val.signal_name, flow_leaf);
      warn(__FILE__,
           "Package url not found for url: {0}. Databased trigger flow will be registered with the url {1}. If the url "
           "is no package url and not available during runtime, database trigger won't be called.",
           flow_leaf.trigger_.val.signal_name, flow_leaf.trigger_.val.signal_name);
    }
    break;
  }
  case TriggerType::CYCLE_START_TRIGGER:
    addFlow(multi_map_cycle_triggering_, std::to_string(flow_leaf.trigger_.val.cycle_start), flow_leaf);
    break;
  case TriggerType::CYCLE_END_TRIGGER:
    addFlow(multi_map_cycle_triggering_, std::to_string(flow_leaf.trigger_.val.cycle_end), flow_leaf);
    break;
  default:
    warn(__FILE__, "Could not register flow as type {0} is unknown.", std::to_string(int(flow_leaf.trigger_.type)));
    break;
  }
}

FlowLeaf OrchestratorMasterImpl::createFlow(const std::string &flow_id) const {
  FlowLeaf flow_leaf{};
  flow_leaf.id = flow_id;
  flow_leaf.meta_data_.delay =
      (unsigned int)next::appsupport::ConfigClient::getConfig()->get_int(configkey::orchestrator::getDelay(flow_id), 0);
  flow_leaf.meta_data_.isSync =
      next::appsupport::ConfigClient::getConfig()->get_option(configkey::orchestrator::getSync(flow_id), true);

  flow_leaf.synch_.synch_id_url =
      next::appsupport::ConfigClient::getConfig()->get_string(configkey::orchestrator::getSyncId(flow_id), "");
  flow_leaf.synch_.synch_timestamp_url =
      next::appsupport::ConfigClient::getConfig()->get_string(configkey::orchestrator::getSyncTimestamp(flow_id), "");

  // read the components
  auto clients = next::appsupport::ConfigClient::getConfig()->getChildren(configkey::orchestrator::getClients(flow_id));
  for (auto &client : clients) {
    auto methods = next::appsupport::ConfigClient::getConfig()->getChildren(
        configkey::orchestrator::getClientMethods(flow_id, client));
    for (auto &method : methods) {
      flow_leaf.trigger_ports_.push_back(std::make_pair(client, method));
    }
  }

  auto trigger_type =
      next::appsupport::ConfigClient::getConfig()->get_string(configkey::orchestrator::getType(flow_id), "time_stamp");
  if (trigger_type == "time_stamp") {
    flow_leaf.trigger_.type = TriggerType::TIME_TRIGGER;
    flow_leaf.trigger_.val.time = (long unsigned int)next::appsupport::ConfigClient::getConfig()->get_int(
        configkey::orchestrator::getValue(flow_id), 10);
  } else if (trigger_type == "signal_name") {
    flow_leaf.trigger_.type = TriggerType::SIGNAL_TRIGGER;
    flow_leaf.trigger_.val.signal_name =
        next::appsupport::ConfigClient::getConfig()->get_string(configkey::orchestrator::getValue(flow_id), "");
  } else if (trigger_type == "cycle_start") {
    flow_leaf.trigger_.type = TriggerType::CYCLE_START_TRIGGER;
    flow_leaf.trigger_.val.cycle_start =
        next::appsupport::ConfigClient::getConfig()->get_int(configkey::orchestrator::getValue(flow_id), 0);
  } else if (trigger_type == "cycle_end") {
    flow_leaf.trigger_.type = TriggerType::CYCLE_END_TRIGGER;
    flow_leaf.trigger_.val.cycle_end =
        next::appsupport::ConfigClient::getConfig()->get_int(configkey::orchestrator::getValue(flow_id), 0);
  }
  return flow_leaf;
}

void OrchestratorMasterImpl::AddFlowToOrchestrator(const FlowLeaf &flow_leaf) {

  registerFlow(flow_leaf);

  ctrl_data_generators_[flow_leaf.id] = std::make_shared<ControlDataGenerator>(flow_leaf.id);
  ctrl_data_generators_[flow_leaf.id]->initControlDataGenerator();
}

bool OrchestratorMasterImpl::loadConfiguration() {

  auto orchestrator_file = next::appsupport::ConfigClient::getConfig()->get_string(
      configkey::orchestrator::getOrchestratorConfigFileKey(), "");
  if (!orchestrator_file.empty()) {
    next::appsupport::ConfigClient::getConfig()->putCfg(orchestrator_file);
  }

  auto flows = next::appsupport::ConfigClient::getConfig()->getChildren(configkey::orchestrator::getFlows());
  if (flows.empty()) {
    return false;
  }
  for (auto &flow : flows) {
    // ignore the config_file as a flow name
    if (flow == configkey::orchestrator::k_config_file_flow_exclude) {
      continue;
    }
    FlowLeaf flow_leaf;
    flow_leaf = createFlow(flow);
    AddFlowToOrchestrator(flow_leaf);
  }
  return true;
}

bool OrchestratorMasterImpl::triggerTimestampBased(uint64_t timeStamp,
                                                   std::map<std::string, TriggerStatus> &trigger_output) {
  bool result = false;

  for (auto it_trigger_timestamps = multi_map_timebased_triggering_.begin();
       it_trigger_timestamps != multi_map_timebased_triggering_.end(); ++it_trigger_timestamps) {

    result = true;
    auto &timebased_trigger = it_trigger_timestamps->second.trigger_.val;
    // check if current timestamp is high enough to trigger
    // trigger always for the first cycle
    if (timebased_trigger.last_trigger_time == 0) {
      execFlowLeaf(it_trigger_timestamps->second, trigger_output, timeStamp);
      timebased_trigger.last_trigger_time = timeStamp;
    } else if ((timeStamp > timebased_trigger.last_trigger_time) &&
               ((timeStamp - timebased_trigger.last_trigger_time) >= timebased_trigger.time)) {
      execFlowLeaf(it_trigger_timestamps->second, trigger_output, timeStamp);
      timebased_trigger.last_trigger_time = timebased_trigger.last_trigger_time + timebased_trigger.time;
    }
  }

  return result;
}

bool OrchestratorMasterImpl::triggerDataBased(uint64_t timeStamp, size_t hash,
                                              std::map<std::string, TriggerStatus> &trigger_output) {

  auto it_topic_name = hash_url_map_data_triggering_.find(hash);
  if (it_topic_name != hash_url_map_data_triggering_.end()) {
    return triggerDataBased(timeStamp, it_topic_name->second, trigger_output);
  } else {
    return false;
  }
  return true;
}

bool OrchestratorMasterImpl::triggerDataBased(uint64_t timeStamp, const std::string &URL,
                                              std::map<std::string, TriggerStatus> &trigger_output) {
  bool result = false;

  auto range_signal_name = multi_map_data_triggering_.equal_range(URL);

  for (auto it = range_signal_name.first; it != range_signal_name.second; it++) {
    result = true;
    execFlowLeaf(it->second, trigger_output, timeStamp);
  }

  return result;
}

bool OrchestratorMasterImpl::triggerCycleBased(uint32_t cycleId, std::map<std::string, TriggerStatus> &trigger_output,
                                               bool is_cycle_start, uint64_t timestamp) {

  bool result = false;
  auto range = multi_map_cycle_triggering_.equal_range(std::to_string(cycleId));

  for (auto it = range.first; it != range.second; it++) {
    // cycle which are both start and end cycle are handeled before
    // TODO: better optimize this to avoid unnecessary runs
    if (is_cycle_start) {
      if (it->second.trigger_.type == TriggerType::CYCLE_START_TRIGGER) {
        result = true;
        execFlowLeaf(it->second, trigger_output, timestamp);
      }
    } else {
      if (it->second.trigger_.type == TriggerType::CYCLE_END_TRIGGER) {
        result = true;
        execFlowLeaf(it->second, trigger_output, timestamp);
      }
    }
  }
  return result;
}

void OrchestratorMasterImpl::generateControlData(const uint64_t timestamp, const std::string &flow_id,
                                                 const std::string &client, const std::string &method) {
  if (ctrl_data_generators_.find(flow_id) != ctrl_data_generators_.end()) {
    ctrl_data_generators_[flow_id]->generateAndSendControlData(timestamp, client, method);
  }
}

TriggerStatus OrchestratorMasterImpl::fireTrigger(const std::string &flowName, uint64_t timeStamp,
                                                  const std::string &flow_id) const {
  OrchestratorEventBinary orchestrator_event(flowName);
  events::OrchestratorTrigger data;
  data.trigger_timestamp = std::to_string(timeStamp);
  data.flow_id = flow_id;
  std::list<std::string> resps;
  orchestrator_event.publish(data, resps);
  if (resps.empty()) {
    return TriggerStatus::NO_RESPONSE;
  }
  if (resps.size() > 1) {
    warn(__FILE__, "Multiple clients for unique trigger event: {0}", flowName);
  }

  const auto &response = resps.front(); // always use just first response

  Json::Reader reader;
  Json::Value json_response;
  bool parseSuccess = reader.parse(response, json_response, false);
  if (parseSuccess) {
    if (json_response.isMember("return_type")) {
      std::string ret_type = json_response["return_type"].asString();
      if (ret_type == "SUCCESS") {
        return TriggerStatus::SUCCESS;
      } else {
        return TriggerStatus::EXEC_WARN;
      }
    }
  } else {
    warn(__FILE__, "Could not parse response from callback. Response: {0}", response);
    return TriggerStatus::FAILURE;
  }

  return TriggerStatus::SUCCESS;
}

void OrchestratorMasterImpl::execFlowLeaf(const FlowLeaf &flow, std::map<std::string, TriggerStatus> &trigger_output,
                                          uint64_t timestamp) {
  auto &saved_cached_flows = map_cached_flows_[flow.id];

  if (saved_cached_flows.size() < flow.meta_data_.delay) {
    saved_cached_flows.push(std::make_pair(flow, timestamp));
  } else {
    saved_cached_flows.push(std::make_pair(flow, timestamp));
    auto [cached_flow, cached_timestamp] = saved_cached_flows.front();

    saved_cached_flows.pop();

    for (auto clients : cached_flow.trigger_ports_) {
      auto trigger_name_port = clients.first + clients.second;
      generateControlData(cached_timestamp, flow.id, clients.first, clients.second);
      if (cached_flow.meta_data_.isSync) {
        auto status = this->fireTrigger(trigger_name_port, cached_timestamp, cached_flow.id);
        trigger_output[trigger_name_port] = status;
      } else {
        auto th = std::thread([=]() {
          [[maybe_unused]] auto status = this->fireTrigger(trigger_name_port, cached_timestamp, cached_flow.id);
          // status not handled in case of asynchronously fired triggers
        });
        list_of_async_fired_triggers_.push_back(std::move(th));
      }
    }
  }
}

void OrchestratorMasterImpl::setupDataBasedTriggering(
    const std::unordered_map<std::string, size_t> &src_hash_to_url_map) {
  for (const auto &it_trigger_data : multi_map_data_triggering_) {
    auto it_url_hash = src_hash_to_url_map.find(it_trigger_data.first);
    if (it_url_hash != src_hash_to_url_map.end()) {
      hash_url_map_data_triggering_.insert(std::make_pair(it_url_hash->second, it_url_hash->first));
    } else {
      warn(__FILE__, "Topic {0} for databased triggering not found. This trigger is not called.",
           it_trigger_data.first);
    }
  }
}

bool OrchestratorMasterImpl::addFlow(std::multimap<std::string, FlowLeaf> &multi_map, const std::string &key,
                                     const FlowLeaf &flow_leaf) {
  multi_map.insert(std::make_pair(key, flow_leaf));
  return true;
}

bool OrchestratorMasterImpl::getPackageUrlFromSignalUrl(const std::string &url, std::string &package_url) {
  package_url = "";
  next::udex::explorer::SignalExplorer signal_explorer;
  std::vector<std::string> package_name = signal_explorer.GetPackageUrl(url);
  if (!package_name.empty()) {
    package_url = package_name.at(0);
    if (!package_url.empty()) {
      return true;
    }
  }
  return false;
}

} // namespace orchestrator
} // namespace control
} // namespace next

/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next
 * ========================== NEXT Project ==================================
 */
/**
 * @file     next.cpp
 * @brief    functions of the Next Component Interface
 *
 **/

#include <next/next.hpp>

#include <algorithm>

#include <next/sdk/logger/logger.hpp>

#include "config.h"
#include "next_impl.hpp"

using config = next::appsupport::ConfigClient;

namespace next {

NextComponentInterface::NextComponentInterface(const std::string &component_name)
    : NextCallbacks(component_name), next::appsupport::LifecycleBase(component_name), CmdOptions(component_name) {
  impl_ = std::make_unique<NextComponentInterfaceImpl>(component_name);
}

NextComponentInterface::~NextComponentInterface() {}

int NextComponentInterface::SetupInput(const std::string &adapter_dll_path,
                                       const std::vector<ComponentInputPortConfig> &input_ports) {
  return impl_->SetupInput(adapter_dll_path, input_ports);
}

uint32_t NextComponentInterface::ConvertVersionNumber(const std::string &version) {
  return impl_->ConvertVersionNumber(version);
}

int NextComponentInterface::SetupOutput(const std::vector<std::string> &sdl_filepath,
                                        const std::vector<ComponentOutputPortConfig> &output_ports) {
  return impl_->SetupOutput(sdl_filepath, output_ports);
}

int NextComponentInterface::SetupTriggers(const std::vector<std::pair<std::string, triggerCallback>> &triggers) {
  return impl_->SetupTriggers(triggers,
                              std::bind(&NextComponentInterface::OnTriggerCallback, this, std::placeholders::_1,
                                        std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
}

int NextComponentInterface::SetupSyncref(
    [[maybe_unused]] const std::vector<std::pair<std::string, SyncMapping>> &syncref_mapping) {
  return 0;
}

// TODO Need to align on function name
// TODO !! ADD Description !!
DataPackage NextComponentInterface::getTriggerAsyncPush(const std::string &input_port_name) {
  return impl_->getTriggerAsyncPush(input_port_name);
}

bool NextComponentInterface::isDataAvailable(const std::string &input_port_name) {
  return impl_->isDataAvailable(input_port_name);
}

void NextComponentInterface::triggerAsyncPush(std::string output_port_name, DataPackage data, bool append_device) {
  return impl_->triggerAsyncPush(output_port_name, data, append_device);
}

void NextComponentInterface::triggerAsyncPush(const uint64_t virtual_address, DataPackage data, bool append_device) {
  return impl_->triggerAsyncPush(virtual_address, data, append_device);
}

std::vector<ComponentInputPortConfig> NextComponentInterface::getInputPorts() { return impl_->getInputPorts(); }
std::vector<ComponentOutputPortConfig> NextComponentInterface::getOutputPorts() { return impl_->getOutputPorts(); }
next::SimulationStatus NextComponentInterface::getStatus() { return impl_->getStatus(); }

/*!
 * @brief returns current time in microseconds
 */
uint64_t NextComponentInterface::getSystemTimeMicroseconds(void) { return impl_->getSystemTimeMicroseconds(); }

void NextComponentInterface::OnTriggerCallback(next::control::orchestrator::OrchestratorErrors &err,
                                               std::string &err_msg, const std::string &method_name,
                                               const triggerCallback &client_callback) {

  if (next::appsupport::LifecycleBase::startExecution()) {
    impl_->OnTrigger(err, err_msg, method_name, client_callback);
  }
  next::appsupport::LifecycleBase::exitExecution();
}

bool NextComponentInterface::userEnterReset() { return impl_->userEnterReset(); }

bool NextComponentInterface::userEnterConfigOutput() { return impl_->userEnterConfigOutput(); }
bool NextComponentInterface::userEnterConfigInput() { return impl_->userEnterConfigInput(); }

bool NextComponentInterface::isUpdated(const std::string &port_name) { return impl_->isUpdated(port_name); }

std::list<TriggerReturn> NextComponentInterface::invokeTrigger(Trigger &trigger, const std::string &timestamp,
                                                               const std::string &flow_id) {
  return impl_->invokeTrigger(trigger.node_name, trigger.methode_name, timestamp, flow_id);
}

std::list<TriggerReturn> NextComponentInterface::invokeTrigger(Trigger &trigger) {
  return impl_->invokeTrigger(trigger.node_name, trigger.methode_name);
}

} // namespace next

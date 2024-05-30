/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     control_data_generator.cpp
 * @brief    generation of control data for components
 *
 **/

#include <fstream>

#include <boost/filesystem.hpp>
#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include <next/sdk/logger/logger.hpp>
#include "control_data_generator.hpp"

#include "base_ctrl_data_pool.hpp"
#include "tree_extractor_publisher.h"

using namespace next::appsupport;

namespace next {
namespace control {
namespace orchestrator {

ControlDataGenerator::ControlDataGenerator(std::string flow_id) : flow_id_(flow_id), data_pool_(flow_id) {}

ControlDataGenerator::~ControlDataGenerator() {}

bool ControlDataGenerator::initControlDataGenerator() {

  auto config = BaseDataReadOutConfig::CreateInfoFromFlowId(flow_id_);

  if (boost::filesystem::exists(config.sdl_file)) {
    data_pool_.SetupInput(config.contol_data_urls);

    for (auto &component_config : config.contol_data_urls) {
      auto comp_name = component_config.component_name;
      data_publishers_[comp_name] = std::make_shared<TreeExtractorPublisher>(flow_id_, comp_name);
      if (!data_publishers_[comp_name]->Setup(config.sdl_file)) {
        warn(__FILE__, "Could not create the publisher for ControlData requested by component: {}", comp_name);
      }
    }
  } else {
    warn(__FILE__, "Could not find SDL file specified in configuration of the orchestrator: {0}", config.sdl_file);
  }
  return true;
}

bool ControlDataGenerator::generateAndSendControlData(uint64_t timestamp, const std::string &component,
                                                      const std::string &method) {

  bool success;
  auto current_signals = data_pool_.GetNewData(component, method, success);
  if (!success) {
    return success;
  }

  auto tree_publisher = data_publishers_.find(component);
  if (tree_publisher == data_publishers_.end()) {
    return false;
  }
  if (!tree_publisher->second->IsReady()) {
    return false;
  }
  bool all_send = true;
  for (auto &signal : current_signals.fixed_signal) {
    all_send &= tree_publisher->second->SetValue(signal.second, signal.first);
  }
  for (auto &signal : current_signals.connected_signal) {
    all_send &=
        tree_publisher->second->SetValueFromValueUnion(signal.second.value, signal.second.signal_type, signal.first);
  }
  all_send &= tree_publisher->second->Publish(timestamp);
  return all_send;
}

bool ControlDataGenerator::isConnected() { return data_pool_.IsConnected(); }

} // namespace orchestrator
} // namespace control
} // namespace next

/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next
 * ========================== NEXT Project ==================================
 */
/**
 * @file     sync_manager.cpp
 * @brief    functions of the Next Component Interface to synchronise data flow
 *
 **/

#include "sync_manager.hpp"

#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include <next/sdk/logger/logger.hpp>

#include <next/udex/helper_data_extractor.h>
#include <next/udex/explorer/signal_explorer.hpp>

using config = next::appsupport::ConfigClient;

namespace next {

SyncManager::SyncManager(const std::string &component_name) { component_name_ = component_name; }
SyncManager::~SyncManager() {}

next::udex::SyncAlgoCfg
SyncManager::SetSyncConfig(const ComponentInputPortConfig &it_input_port, const std::string &subscribe_url,
                           std::string &signal_url, std::shared_ptr<next::udex::extractor::DataExtractor> extractor,
                           std::shared_ptr<next::udex::subscriber::DataSubscriber> data_subscriber) {
  AddPortInfo(it_input_port, subscribe_url, signal_url, extractor);

  next::udex::SyncAlgoCfg sync_algo_cfg{};
  SyncPortConfig sync_port_config{};
  SyncConfig sync_config;
  SyncConfigReader sync_config_reader;
  sync_config_reader.ReadSyncConfig(component_name_, it_input_port, sync_config);

  sync_port_config.sync_mode = sync_config.mode;

  this->m_sync_port_config_.insert(std::pair<std::string, SyncPortConfig>(it_input_port.port_name, sync_port_config));

  sync_algo_cfg.algorythm_type = sync_config.algo_port_level.algorythm_type;

  std::string port_url = config::getConfig()->get_string(
      next::appsupport::configkey::getCompositionPortUrlKey(component_name_, it_input_port.port_name), "");

  if (sync_port_config.sync_mode == next::SyncMode::SYNC_SYNCID ||
      sync_port_config.sync_mode == next::SyncMode::SYNC_TIMESTAMP) {
    sync_algo_cfg.composition_sync_signal_url = sync_config.algo_port_level.composition_sync_signal_url;
    SetupSyncValue(data_subscriber);

  } else if (sync_port_config.sync_mode == next::SyncMode::SYNC_SYNC_REF_SYNCID ||
             sync_port_config.sync_mode == next::SyncMode::SYNC_SYNC_REF_TIMESTAMP) {
    SetupSyncRef(data_subscriber);

    SetupSyncRefPortInfo(it_input_port.port_name, sync_config);

    sync_algo_cfg.algorythm_type = next::udex::SyncAlgoType::FIND_EXACT_VALUE;
    sync_algo_cfg.composition_sync_signal_url = sync_config.algo_port_level.composition_sync_signal_url;

  } else if (sync_port_config.sync_mode == SYNC_UNSUPPORTED) {
    warn(__FILE__, "Sync Mode from config is not supported!");
    warn(__FILE__, "Supported values are: timestamp, sync_id, sync_ref_timestamp, sync_ref_sync_id");
    warn(__FILE__, "Given value is: {0}",
         config::getConfig()->get_string(
             next::appsupport::configkey::getSyncModeKey(component_name_, it_input_port.port_name), ""));
  }

  return sync_algo_cfg;
} // namespace next

next::udex::SyncValue SyncManager::GetSyncValue(const std::string &port_name) { return m_sync_value_[port_name]; }

int SyncManager::FillReferenceValueAndData(next::control::events::OrchestratorTrigger &trigger_info) {
  next::udex::SyncValue cycle_id;
  next::udex::SyncValue timestamp;
  current_trigger_ = trigger_info;
  // get_value_from_trigger
  // TODO implement
  bool result = this->UpdateSyncValues(trigger_info, cycle_id, timestamp);
  if (!result) {
    error(__FILE__, "Problem occured while extracting trigger message information!");
    return -1;
  }
  for (auto &it_port : this->m_sync_port_config_) {
    next::udex::SyncValue sync_value;
    switch (it_port.second.sync_mode) {
    case SYNC_SYNCID:
      sync_value = cycle_id;
      break;
    case SYNC_TIMESTAMP:
      sync_value = timestamp;
      break;
    case SYNC_SYNC_REF_TIMESTAMP:
      sync_value = this->GetSyncValueFromSyncRef(it_port.first);
      break;
    case SYNC_SYNC_REF_SYNCID:
      sync_value = this->GetSyncValueFromSyncRef(it_port.first);
      break;
    case SYNC_UNSUPPORTED:
      warn(__FILE__, "The sync mode of \"{}\"is set to \"unsupported\".", it_port.first);
      return -1;
    }
    m_sync_value_[it_port.first] = sync_value;
    FillPortSyncedData(it_port.first, sync_value);
  }
  return 0;
}

void SyncManager::FillPortSyncedData(const std::string &port_name, next::udex::SyncValue sync) {
  std::vector<next::udex::SyncValue> sync_values;
  sync_values.push_back(sync);
  if (m_port_info_.find(port_name) == m_port_info_.end()) {
    warn(__FILE__, "There is no port info inside the SyncManager for port name: {0}", port_name);
    return;
  }
  auto &port_info_temp = m_port_info_[port_name];
  if (port_info_temp.extractor_->isDataAvailable(port_info_temp.subscribed_url_)) {
    // if no data is available we only insert the dummy packet when requetsed for that port
    next::udex::TimestampedDataPacket synchronized_extracted_data =
        port_info_temp.extractor_->GetSyncExtractedData(port_info_temp.subscribed_url_, sync_values);

    if (synchronized_extracted_data.data == nullptr) {
      // there is data in the output queue but we cannot sync it
      // TODO: check if this is realy desired by the component or they want a nullptr returned?
      m_weak_shadow_buffer_[port_name] =
          port_info_temp.extractor_->InsertEmptyDataPacket(port_info_temp.subscribed_url_, port_info_temp.port_size_);
      fillBufferUsingOffset(port_info_temp, m_weak_shadow_buffer_[port_name]);
      port_info_temp.port_update_state_ = true;
      return;
    }
    auto it = m_weak_shadow_buffer_.find(port_name);
    if (it == m_weak_shadow_buffer_.end()) {
      m_weak_shadow_buffer_[port_name] = synchronized_extracted_data;
      if (synchronized_extracted_data.timestamp != 0) {
        fillBufferUsingOffset(port_info_temp, m_weak_shadow_buffer_[port_name]);
      }
      port_info_temp.port_update_state_ = true;
      return;
    }

    auto IsNewPackage = [=]() {
      return ((it->second.data != synchronized_extracted_data.data) ||
              (it->second.timestamp != synchronized_extracted_data.timestamp));
    };
    if (IsNewPackage()) {
      // we extracted data from the Queue and the data is new so we copy it in the shallow buffer
      it->second = synchronized_extracted_data;
      fillBufferUsingOffset(port_info_temp, it->second);
      port_info_temp.port_update_state_ = true;
    } else {
      // same data we do not update it
      port_info_temp.port_update_state_ = false;
    }
  }
}

bool SyncManager::UpdateSyncValues([[maybe_unused]] next::control::events::OrchestratorTrigger &trigger_info,
                                   next::udex::SyncValue &cycle_id, next::udex::SyncValue &timestamp) {
  // Dummy Implementation
  test_sync_value_ += 10;
  timestamp.signal_type = next::udex::SignalType::SIGNAL_TYPE_UINT64;
  timestamp.signal_value = test_sync_value_;

  cycle_id.signal_type = next::udex::SignalType::SIGNAL_TYPE_UINT64;
  cycle_id.signal_value = test_sync_value_;

  return true;
}

next::udex::SyncValue SyncManager::GetSyncValueFromSyncRef(const std::string &port_name) {
  // we select the correct timestamp to be used to extract the sync ref package
  std::string current_flow = current_trigger_.flow_id;
  uint64_t trigger_ts = std::stoull(current_trigger_.trigger_timestamp);
  return syncref_handler_map_[current_flow]->getSyncValue(port_name, trigger_ts);
}

void SyncManager::Reset() {
  m_sync_port_config_.clear();
  m_sync_value_.clear();
  m_port_info_.clear();
  m_weak_shadow_buffer_.clear();
  synced_data_.clear();
  sync_ref_set_up_ = false;
  syncref_handler_map_.clear();
}

next::udex::TimestampedDataPacket SyncManager::getSyncedPacket(const std::string &port_name) {
  if (m_weak_shadow_buffer_.find(port_name) == m_weak_shadow_buffer_.end()) {
    if (m_port_info_.find(port_name) == m_port_info_.end()) {
      error(__FILE__, "Port {0} not found in the map, couldn't return synced package!", port_name);
      return next::udex::TimestampedDataPacket{nullptr, 0, 0};
    }
    auto &port_info_temp = m_port_info_[port_name];
    if (!port_info_temp.extractor_) {
      error(__FILE__, "Extractor is nullptr for {0} port, couldn't extract synced package!", port_name);
      return next::udex::TimestampedDataPacket{nullptr, 0, 0};
    }
    m_weak_shadow_buffer_[port_name] =
        port_info_temp.extractor_->InsertEmptyDataPacket(port_info_temp.subscribed_url_, port_info_temp.port_size_);
    fillBufferUsingOffset(port_info_temp, m_weak_shadow_buffer_[port_name]);
    m_port_info_[port_name].port_update_state_ = true;
  }

  return m_weak_shadow_buffer_[port_name];
}
void SyncManager::fillBufferUsingOffset(const PortInfo &port_info, next::udex::TimestampedDataPacket &buffer_element) {
  if (buffer_element.size >= port_info.signal_info_.offset) {
    buffer_element.data = (char *)buffer_element.data + port_info.signal_info_.offset;
    buffer_element.size = buffer_element.size - port_info.signal_info_.offset;
  }
}

bool SyncManager::isUpdated(const std::string &port_name) {
  if (m_port_info_.find(port_name) != m_port_info_.end()) {
    return m_port_info_[port_name].port_update_state_;
  }
  warn(__FILE__, "Port {0} is not present in the update state map.", port_name);
  return false;
}

void SyncManager::SetupSyncValue(
    [[maybe_unused]] std::shared_ptr<next::udex::subscriber::DataSubscriber> data_subscriber) {
  if (!sync_ref_set_up_) {
    auto flows =
        next::appsupport::ConfigClient::getConfig()->getChildren(next::appsupport::configkey::orchestrator::getFlows());
    // TODO: remove once getChildren from server is working
    for (auto &flow : flows) {
      std::string syncref_selector_url = next::appsupport::ConfigClient::getConfig()->get_string(
          next::appsupport::configkey::orchestrator::getRefValue(flow), "");
      // sync_value_handler_map_[flow] = std::make_shared<SyncValueHandler>(syncref_selector_url, data_subscriber);
    }
    sync_ref_set_up_ = true;
  }
}

void SyncManager::SetupSyncRef(std::shared_ptr<next::udex::subscriber::DataSubscriber> data_subscriber) {
  if (!sync_ref_set_up_) {
    auto flows =
        next::appsupport::ConfigClient::getConfig()->getChildren(next::appsupport::configkey::orchestrator::getFlows());
    // TODO: remove once getChildren from server is working
    for (auto &flow : flows) {
      std::string syncref_selector_url = next::appsupport::ConfigClient::getConfig()->get_string(
          next::appsupport::configkey::orchestrator::getRefValue(flow), "");
      syncref_handler_map_[flow] = std::make_shared<SyncRefHandler>(flow, syncref_selector_url, data_subscriber);
    }
    sync_ref_set_up_ = true;
  }
}

void SyncManager::SetupSyncRefPortInfo(const std::string &port_name, const SyncConfig &sync_config) {
  std::string signal_sync_url;

  if (sync_config.mode == SYNC_SYNC_REF_TIMESTAMP) {
    signal_sync_url = sync_config.timestamp.port_member_data_url;
  } else if (sync_config.mode == SYNC_SYNC_REF_SYNCID) {
    signal_sync_url = sync_config.sync_id.port_member_data_url;
  } else {
    signal_sync_url = sync_config.timestamp.port_member_data_url;
  }

  if (sync_config.sync_value.syncref_sync_header_url == "") {
    warn(__FILE__, "The SyncrefSyncHeaderUrl url is not set. Please set it in the configuration at component level or "
                   "port level.");
  }
  for (auto flow : syncref_handler_map_) {
    syncref_handler_map_[flow.first]->setupPortSyncData(port_name, sync_config.sync_value.syncref_sync_header_url,
                                                        signal_sync_url);
  }
}

void SyncManager::AddPortInfo(const ComponentInputPortConfig &it_input_port, const std::string &subscribe_url,
                              std::string &signal_url,
                              std::shared_ptr<next::udex::extractor::DataExtractor> extractor) {
  PortInfo port_info;
  port_info.extractor_ = extractor;
  port_info.port_size_ = it_input_port.port_size;
  port_info.subscribed_url_ = subscribe_url;
  port_info.port_update_state_ = false;
  port_info.signal_info_ = port_info.extractor_->GetInfo(signal_url);
  m_port_info_[it_input_port.port_name] = port_info;
}

} // namespace next

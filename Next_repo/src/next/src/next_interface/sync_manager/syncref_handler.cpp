/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next
 * ========================== NEXT Project ==================================
 */
/**
 * @file     syncref_handler.cpp
 * @brief    functions of the Next Component Interface to synchronise data flow
 *
 **/

#include "syncref_handler.hpp"

#include <next/sdk/logger/logger.hpp>

namespace next {
SyncRefHandler::SyncRefHandler(std::string flow_name, std::string selection_url,
                               std::shared_ptr<next::udex::subscriber::DataSubscriber> data_subscriber)
    : flow_name_(flow_name), data_subscriber_(data_subscriber) {
  syncref_selector_extractor_ = std::make_shared<next::udex::extractor::DataExtractor>();
  data_subscriber_->Subscribe(selection_url, syncref_selector_extractor_);

  syncref_selection_url_.signal_url = selection_url;
  std::vector<std::string> urls = signal_explorer_->GetPackageUrl(selection_url);
  if (urls.size() > 0) {
    syncref_selection_url_.package_url = urls[0];
    next::udex::SyncAlgoCfg ref_value_sync_cfg;
    ref_value_sync_cfg.algorythm_type = next::udex::SyncAlgoType::FIND_EXACT_HEADER_TIMESTAMP;
    syncref_selector_extractor_->SetSyncAlgo(syncref_selection_url_.package_url, ref_value_sync_cfg);
  } else {
    syncref_selection_url_.package_url = "";
  }
}

void SyncRefHandler::setupPortSyncData(std::string port_name, std::string sync_ref_url, std::string port_sync_info) {
  SyncRefPortInfo port_syncref_info;
  bool found_sync_extractor = false;
  for (auto sync_data : sync_info_map_) {
    if (sync_data.second.syncref_info.signal_url == sync_ref_url) {
      found_sync_extractor = true;
      port_syncref_info.syncref_extractor = sync_data.second.syncref_extractor;
      break;
    }
  }
  if (!found_sync_extractor) {
    port_syncref_info.syncref_extractor = std::make_shared<SyncRefExtractor>();
    port_syncref_info.syncref_extractor->syncref_data_extractor =
        std::make_shared<next::udex::extractor::DataExtractor>();
    port_syncref_info.syncref_extractor->data_extracted = false;
  }
  port_syncref_info.syncref_extractor->data_extracted = false;
  port_syncref_info.syncref_info.signal_url = sync_ref_url;
  data_subscriber_->Subscribe(sync_ref_url, port_syncref_info.syncref_extractor->syncref_data_extractor);

  port_syncref_info.syncref_selection_url = port_sync_info;
  std::vector<std::string> urls = signal_explorer_->GetPackageUrl(port_syncref_info.syncref_info.signal_url);
  if (urls.size() > 0) {
    port_syncref_info.syncref_info.package_url = urls[0];
    next::udex::SyncAlgoCfg ref_value_sync_cfg;
    ref_value_sync_cfg.algorythm_type = next::udex::SyncAlgoType::FIND_EXACT_VALUE;
    ref_value_sync_cfg.composition_sync_signal_url = port_syncref_info.syncref_info.signal_url;
    port_syncref_info.syncref_extractor->syncref_data_extractor->SetSyncAlgo(port_syncref_info.syncref_info.package_url,
                                                                             ref_value_sync_cfg);
    port_syncref_info.syncref_selection_url = urls[0] + "." + port_syncref_info.syncref_selection_url;
  }
  sync_info_map_[port_name] = port_syncref_info;
}

next::udex::SyncValue SyncRefHandler::getSyncValue(std::string port_name, uint64_t ts) {
  if (old_slection_ts_ != ts) {
    sync_info_map_[port_name].syncref_extractor->data_extracted = false;
    old_slection_ts_ = ts;
    next::udex::SyncValue sync_ref_value;
    sync_ref_value.signal_type = next::udex::SignalType::SIGNAL_TYPE_UINT64;
    sync_ref_value.signal_value = ts;
    std::vector<next::udex::SyncValue> sync_values{sync_ref_value};

    if (syncref_selection_url_.package_url == "") {
      std::vector<std::string> urls = signal_explorer_->GetPackageUrl(syncref_selection_url_.signal_url);
      if (urls.size() > 0) {
        syncref_selection_url_.package_url = urls[0];
        next::udex::SyncAlgoCfg ref_value_sync_cfg;
        ref_value_sync_cfg.algorythm_type = next::udex::SyncAlgoType::FIND_EXACT_HEADER_TIMESTAMP;
        syncref_selector_extractor_->SetSyncAlgo(syncref_selection_url_.package_url, ref_value_sync_cfg);
      }
    }
    syncref_selector_extractor_->GetSyncExtractedData(syncref_selection_url_.package_url, sync_values);
    syncref_selector_extractor_->GetValue(syncref_selection_url_.signal_url, next::udex::SignalType::SIGNAL_TYPE_UINT64,
                                          syncref_selection_ts_);
  }
  // we select the correct syncref package
  next::udex::SyncValue sync_ref_value;
  sync_ref_value.signal_type = next::udex::SignalType::SIGNAL_TYPE_UINT64;
  sync_ref_value.signal_value = std::get<uint64_t>(syncref_selection_ts_);
  std::vector<next::udex::SyncValue> sync_values{sync_ref_value};

  if (sync_info_map_[port_name].syncref_info.package_url == "") {
    std::vector<std::string> urls = signal_explorer_->GetPackageUrl(sync_info_map_[port_name].syncref_info.signal_url);
    if (urls.size() > 0) {
      sync_info_map_[port_name].syncref_info.package_url = urls[0];
      sync_info_map_[port_name].syncref_selection_url = urls[0] + "." + sync_info_map_[port_name].syncref_selection_url;
      next::udex::SyncAlgoCfg ref_value_sync_cfg;
      ref_value_sync_cfg.algorythm_type = next::udex::SyncAlgoType::FIND_EXACT_VALUE;
      ref_value_sync_cfg.composition_sync_signal_url = sync_info_map_[port_name].syncref_info.signal_url;
      sync_info_map_[port_name].syncref_extractor->syncref_data_extractor->SetSyncAlgo(
          sync_info_map_[port_name].syncref_info.package_url, ref_value_sync_cfg);
    }
  }

  next::udex::SyncValue sync_value;
  if (!sync_info_map_[port_name].syncref_extractor->data_extracted) {
    next::udex::TimestampedDataPacket package =
        sync_info_map_[port_name].syncref_extractor->syncref_data_extractor->GetSyncExtractedData(
            sync_info_map_[port_name].syncref_info.package_url, sync_values);

    if (package.data == nullptr && package.size == 0 && package.timestamp == 0) {
      warn(__FILE__, "Could not get sync value for port {0} using the signal url: {1}", port_name,
           sync_info_map_[port_name].syncref_selection_url);

      sync_value.signal_value = (uint64_t)0;
      sync_value.signal_type = next::udex::SignalType::SIGNAL_TYPE_UINT64;
      return sync_value;
    }
    sync_info_map_[port_name].syncref_extractor->data_extracted = true;
  }

  // extract the ts to sync the port data
  next::udex::SignalInfo signal_info = sync_info_map_[port_name].syncref_extractor->syncref_data_extractor->GetInfo(
      sync_info_map_[port_name].syncref_selection_url);

  // we use next::udex::SignalType::SIGNAL_TYPE_UINT64 because SyncAlgo_ExactValue expects the valus as an uint64_t
  bool ret = sync_info_map_[port_name].syncref_extractor->syncref_data_extractor->GetValue(
      sync_info_map_[port_name].syncref_selection_url, next::udex::SignalType::SIGNAL_TYPE_UINT64,
      sync_value.signal_value, &signal_info);
  sync_value.signal_type = signal_info.signal_type;
  if (!ret) {
    warn(__FILE__, "Could not get sync value for port {0} using the signal url: {1}", port_name,
         sync_info_map_[port_name].syncref_selection_url);

    sync_value.signal_value = (uint64_t)0;
    sync_value.signal_type = next::udex::SignalType::SIGNAL_TYPE_UINT64;
  }
  return sync_value;
}

} // namespace next

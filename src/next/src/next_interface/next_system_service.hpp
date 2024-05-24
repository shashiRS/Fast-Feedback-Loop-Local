/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     next_system_service.hpp
 * @brief    provides system service functions
 *
 */
#ifndef NEXT_NEXT_SYSTEM_SERVICE_HPP
#define NEXT_NEXT_SYSTEM_SERVICE_HPP

#include <next/next.hpp>

#include "next_inport.hpp"

#include <next/sdk/logger/logger.hpp>

#include <next/udex/extractor/data_extractor.hpp>

#include <next/control/event_types/player_state_event.h>

namespace next {
namespace system_service {

class TimeService {
public:
  TimeService(const std::string component_name) : component_name_(component_name) {
    data_subscriber_ = std::make_shared<next::udex::subscriber::DataSubscriber>(component_name_);
    unsigned short queue_size = 100u;
    timestamp_extractor_ = std::make_shared<next::udex::extractor::DataExtractor>(queue_size);
  }
  virtual ~TimeService() {}
  uint64_t getSystemTimeMicroseconds() { return current_timestamp_; }

  void SetupSystemService() {
    using config = next::appsupport::ConfigClient;
    std::string sys_service_mode =
        config::getConfig()->get_string(appsupport::configkey::getTimestampModeKey(component_name_), "", true);
    if (sys_service_mode == "") {
      config::getConfig()->put(appsupport::configkey::getTimestampModeKey(component_name_), "signal");
    }
    std::string sys_service_url =
        config::getConfig()->get_string(appsupport::configkey::getTimestampUrlKey(component_name_), "", true);
    if (sys_service_url == "") {
      config::getConfig()->put(appsupport::configkey::getTimestampUrlKey(component_name_), "undefined");
    }
    std::string sys_service_unit =
        config::getConfig()->get_string(appsupport::configkey::getTimestampUnitKey(component_name_), "", true);
    if (sys_service_unit == "") {
      config::getConfig()->put(appsupport::configkey::getTimestampUnitKey(component_name_), "us");
    }
    next::udex::SyncAlgoCfg timestamp_sync_cfg{next::udex::SyncAlgoType::GET_LAST_ELEMENT};
    std::vector<std::string> url_list =
        config::getConfig()->getStringList(appsupport::configkey::getTimestampUrlKey(component_name_), {});

    timestamp_data_.subscribe_url = inport::NextInport::getValidSubscribeURL(
        url_list, timestamp_data_.signal_url, "ECALTimedll.timestampall." + component_name_);
    timestamp_extractor_->SetSyncAlgo(timestamp_data_.subscribe_url, timestamp_sync_cfg);
    data_subscriber_->Subscribe(timestamp_data_.subscribe_url, timestamp_extractor_);
    timestamp_data_.signal_info = timestamp_extractor_->GetInfo(timestamp_data_.signal_url);
    timestamp_data_.unit =
        config::getConfig()->get_string(next::appsupport::configkey::getTimestampUnitKey(component_name_), "us");
  }

  void FillCurrentTimestampMicroseconds() {
    auto timestamp_package = timestamp_extractor_->GetSyncExtractedData(timestamp_data_.subscribe_url, {});
    if (timestamp_package.size == 0) {
      current_timestamp_ = 0;
      return;
    }
    if (timestamp_package.size < timestamp_data_.signal_info.offset + timestamp_data_.signal_info.signal_size) {
      current_timestamp_ = 0;
      return;
    }
    char *value_location = (((char *)timestamp_package.data) + timestamp_data_.signal_info.offset);

    switch (timestamp_data_.signal_info.signal_type) {
    case next::udex::SignalType::SIGNAL_TYPE_CHAR: {
      char value = ((char *)value_location)[0];
      current_timestamp_ = TransformTimestampToMicroseconds(static_cast<uint64_t>(value));
      break;
    }
    case next::udex::SignalType::SIGNAL_TYPE_UINT8: {
      uint8_t value = ((uint8_t *)value_location)[0];
      current_timestamp_ = TransformTimestampToMicroseconds(static_cast<uint64_t>(value));
      break;
    }
    case next::udex::SignalType::SIGNAL_TYPE_INT8: {
      int8_t value = ((int8_t *)value_location)[0];
      current_timestamp_ = TransformTimestampToMicroseconds(static_cast<uint64_t>(value));
      break;
    }
    case next::udex::SignalType::SIGNAL_TYPE_UINT16: {
      uint16_t value = ((uint16_t *)value_location)[0];
      current_timestamp_ = TransformTimestampToMicroseconds(static_cast<uint64_t>(value));
      break;
    }
    case next::udex::SignalType::SIGNAL_TYPE_INT16: {
      int16_t value = ((int16_t *)value_location)[0];
      current_timestamp_ = TransformTimestampToMicroseconds(static_cast<uint64_t>(value));
      break;
    }
    case next::udex::SignalType::SIGNAL_TYPE_UINT32: {
      uint32_t value = ((uint32_t *)value_location)[0];
      current_timestamp_ = TransformTimestampToMicroseconds(static_cast<uint64_t>(value));
      break;
    }
    case next::udex::SignalType::SIGNAL_TYPE_INT32: {
      int32_t value = ((int32_t *)value_location)[0];
      current_timestamp_ = TransformTimestampToMicroseconds(static_cast<uint64_t>(value));
      break;
    }
    case next::udex::SignalType::SIGNAL_TYPE_UINT64: {
      uint64_t value = ((uint64_t *)value_location)[0];
      current_timestamp_ = TransformTimestampToMicroseconds(static_cast<uint64_t>(value));
      break;
    }
    case next::udex::SignalType::SIGNAL_TYPE_INT64: {
      int64_t value = ((int64_t *)value_location)[0];
      current_timestamp_ = TransformTimestampToMicroseconds(static_cast<uint64_t>(value));
      break;
    }
    case next::udex::SignalType::SIGNAL_TYPE_FLOAT: {
      float value = ((float *)value_location)[0];
      current_timestamp_ = TransformTimestampToMicroseconds(static_cast<uint64_t>(value));
      break;
    }
    case next::udex::SignalType::SIGNAL_TYPE_DOUBLE: {
      double value = ((double *)value_location)[0];
      current_timestamp_ = TransformTimestampToMicroseconds(static_cast<uint64_t>(value));
      break;
    }
    default:
      current_timestamp_ = 0;
      break;
    }
  }

private:
  uint64_t TransformTimestampToMicroseconds(uint64_t timestamp) {
    if (timestamp_data_.unit == "s") {
      return timestamp * 1000 * 1000;
    }
    if (timestamp_data_.unit == "ms") {
      return timestamp * 1000;
    }
    if (timestamp_data_.unit == "us") {
      return timestamp;
    }
    if (timestamp_data_.unit == "ns") {
      return timestamp / 1000;
    }
    next::sdk::logger::debug(__FILE__, "No supported timestamp signal unit selected.");
    next::sdk::logger::debug(__FILE__, "Value taken from configuration: {}", timestamp_data_.unit);
    next::sdk::logger::debug(__FILE__, "Supported Units:");
    next::sdk::logger::debug(__FILE__, "Seconds: \"s\"");
    next::sdk::logger::debug(__FILE__, "Milliseconds: \"ms\"");
    next::sdk::logger::debug(__FILE__, "Microseconds (default): \"us\"");
    next::sdk::logger::debug(__FILE__, "Nanoseconds: \"ns\"");
    return 0;
  }

private:
  struct TimestampSignalData {
    std::string subscribe_url;
    std::string signal_url;
    std::string unit;
    next::udex::SignalInfo signal_info;
  };
  std::string component_name_;
  TimestampSignalData timestamp_data_;
  std::shared_ptr<next::udex::extractor::DataExtractor> timestamp_extractor_;
  uint64_t current_timestamp_ = 0;
  std::shared_ptr<next::udex::subscriber::DataSubscriber> data_subscriber_;

public:
  friend class TimeServiceTester;
};

class SimStatusService {
public:
  SimStatusService() : player_event_subscriber_("") {
    auto hook = std::bind(&SimStatusService::UpdateSimStatus, this);
    player_event_subscriber_.addEventHook(hook);
    player_event_subscriber_.subscribe();
  }
  virtual ~SimStatusService() {}
  next::SimulationStatus getStatus() { return sim_status_; }

private:
  void UpdateSimStatus() {
    auto received_msg = player_event_subscriber_.getEventData();
    sim_status_.rec_name = received_msg.recording_name;
  }
  next::control::events::PlayerStateEvent player_event_subscriber_;
  next::SimulationStatus sim_status_;
};

} // namespace system_service
} // namespace next
#endif // NEXT_NEXT_SYSTEM_SERVICE_HPP

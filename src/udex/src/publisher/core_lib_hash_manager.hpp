/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */

#ifndef NEXT_CORE_LIB_HASH_MANAGER_H
#define NEXT_CORE_LIB_HASH_MANAGER_H

#include <fstream>

#include <next/sdk/sdk_macros.h>

NEXT_DISABLE_DEPRECATED_WARNING
#include <mts/runtime/udex/extractor_proxy.hpp>
#include <mts/runtime/udex/signal_storage_proxy.hpp>
NEXT_RESTORE_WARNINGS

#ifndef NO_USE_LOGGER_NAMESPACE
#define UNDEF_NO_USE_LOGGER_NAMESPACE
#endif
#define NO_USE_LOGGER_NAMESPACE
#include <next/sdk/logger/logger.hpp>
#ifdef UNDEF_NO_USE_LOGGER_NAMESPACE
#undef NO_USE_LOGGER_NAMESPACE
#undef UNDEF_NO_USE_LOGGER_NAMESPACE
#endif

#include "hash_manager.hpp"

#define DEFAULT_DATA_SOURCE_NAME "SIM VFB"
#define DEFAULT_SOURCE_ID 87
#define DEFAULT_INSTANCE_NUMBER 37
#define DEFAULT_FORMAT_IDENTIFIER "mts.mta"
#define CAN_FORMAT_IDENTIFIER "mts.can"
#define FIBEX_FORMAT_IDENTIFIER "mts.eth"
namespace next {
namespace udex {

class CoreLibHashManager {
public:
  CoreLibHashManager() {}

  virtual ~CoreLibHashManager() {}

  void Initialize() {
    SetDataSourceInfo(DEFAULT_DATA_SOURCE_NAME, DEFAULT_SOURCE_ID, DEFAULT_INSTANCE_NUMBER, DEFAULT_FORMAT_IDENTIFIER);

    auto current_time =
        std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch());

    if (!udex_extractor_ptr_) {
      std::ostringstream ss;
      ss << std::this_thread::get_id() << current_time.count();
      std::string default_storage_name = "file:" + ss.str() + "?mode=memory";
      udex_extractor_ptr_ = std::make_shared<mts::runtime::udex::extractor_proxy>(default_storage_name);
      hash_manager_.Initialize(udex_extractor_ptr_);
    }
  }

  void Terminate() {
    // First terminate the aggregated object,
    // so it will release its extractor proxy reference
    hash_manager_.Terminate();

    SetDataSourceInfo(DEFAULT_DATA_SOURCE_NAME, DEFAULT_SOURCE_ID, DEFAULT_INSTANCE_NUMBER, DEFAULT_FORMAT_IDENTIFIER);

    if (udex_extractor_ptr_) {
      udex_extractor_ptr_->unregister_sources();
      try {
        udex_extractor_ptr_.reset();
      } catch (std::exception &e) {
        std::cout << "Exception while shutting down mts core lib: " << e.what() << std::endl;
      }
    }
  }

  bool SetDataSourceInfo(const std::string &data_source_name, uint16_t source_id, uint32_t instance_number,
                         const std::string &format_identifier) {
    data_source_name_ = data_source_name;
    source_id_ = source_id;
    instance_number_ = instance_number;
    format_identifier_ = format_identifier;

    return true;
  }

  bool RegisterDataSources(const std::string &file_name, const std::string &file_format) {
    std::string sdl;

    std::ifstream fstream(file_name, std::ios_base::in | std::ios_base::binary);

    if (!fstream) {
      next::sdk::logger::warn(__FILE__, "Can not open {}. Unable to register data source.", file_name);
      return false;
    }

    fstream.seekg(0, std::ios::end);
    const auto length = fstream.tellg();
    if (length > 0) {
      fstream.seekg(0, std::ios::beg);

      sdl.resize((size_t)length);
      fstream.read(&(sdl[0]), length);

      fstream.close();

      return RegisterDataSources(file_name, sdl, file_format);
    }
    next::sdk::logger::error(__FILE__, "Failed to read content of {}.", file_name);
    return false;
  }

  bool RegisterDataSources(const std::string &file_name, const std::string &binary_description,
                           const std::string &data_format) {

    auto data_source_name = data_source_name_;
    auto source_id = source_id_;
    auto instance_number = instance_number_;
    auto format_identifier = format_identifier_;

    if (data_format == "dbc") {
      source_id = mts::extensibility::vehicle_bus_can;
      format_identifier = CAN_FORMAT_IDENTIFIER;
    } else if (data_format == "fibex") {
      source_id = mts::extensibility::vehicle_bus_ethernet;
      format_identifier = FIBEX_FORMAT_IDENTIFIER;
    }

    std::vector<std::string> signatures;

    if (binary_description.length() == 0) {
      if (!RegisterEmptyDataDescription(signatures, data_source_name, source_id, instance_number)) {
        return false;
      }
    } else {
      if (!RegisterBinaryDataDescription(signatures, file_name, binary_description, data_format, data_source_name,
                                         source_id, instance_number)) {
        return false;
      }
    }

    if (signatures.empty()) {
      return false;
    }
    hash_manager_.GenerateHashesAndUrls(data_source_name, source_id, instance_number, format_identifier);
    return true;
  }

  bool RegisterDataSources(const std::string &port_name, const size_t port_size) {
    hash_manager_.GenerateHashesAndUrls(port_name, port_size, data_source_name_, source_id_, instance_number_,
                                        format_identifier_);

    return true;
  }

  size_t GetTopicHash(const uint64_t vaddr) { return hash_manager_.GetHashByVaddr(vaddr); }

  size_t GetTopicHash(const std::string &port_name, bool append_device) {
    return hash_manager_.GetHashByPortname(data_source_name_, port_name, append_device);
  }

  std::string GetAppendedName(const std::string &port_name) { return data_source_name_ + "." + port_name; }

  bool GetSchema(const std::string &url, const uint16_t source_id, std::string &schema) {
    return hash_manager_.get_schema(url, source_id, schema);
  }

  // function contains only the currently registered topics
  const std::unordered_map<std::string, HashManager::HashTopicInformation> &GetNewRegisteredTopicsList() {
    return hash_manager_.GetNewRegisteredTopicsList();
  }

  // function contains only the currently registered topics
  const std::unordered_map<std::string, size_t> &GetTopicsAndHashes() { return hash_manager_.GetTopicsAndHashes(); }

  // clear of the curently registered sources to prepare for next RegisterDataDescription
  void ClearNewRegisteredTopicsList() { hash_manager_.ClearNewRegisteredTopicsList(); }

  std::string GetFilterExpressionFromURLs(const std::vector<std::string> &urls) const {
    std::string filter_expression;

    try {
      for (const auto &url : urls) {
        udex_extractor_ptr_->subscribe(url);
      }
      filter_expression = udex_extractor_ptr_->get_reader_filter_expression();
      udex_extractor_ptr_->unsubscribe_all();
    } catch (std::exception &e) {
      next::sdk::logger::warn(__FILE__, "Failed to get reader expression: {0}", e.what());
    }

    return filter_expression;
  }

private:
  bool RegisterEmptyDataDescription(std::vector<std::string> &signatures, const std::string data_source_name,
                                    const uint16_t source_id, const uint32_t instance_number) {

    try {

      mts::runtime::udex::data_source source{source_id, instance_number, 0, 0, "", data_source_name, ""};
      signatures = udex_extractor_ptr_->register_source(source, nullptr, true);

      return true;
    } catch (std::exception &e) {
      next::sdk::logger::warn(__FILE__, "Failed to register source with {0} using data from the memory. {1}",
                              data_source_name, e.what());
      return false;
    }
  }

  bool RegisterBinaryDataDescription(std::vector<std::string> &signatures, const std::string &file_name,
                                     const std::string &binary_description, const std::string &data_format,
                                     const std::string data_source_name, const uint16_t source_id,
                                     const uint32_t instance_number) {

    try {
      // register source with file format and get data description signature
      signatures = udex_extractor_ptr_->register_source_with_data_description(
          source_id, instance_number, data_source_name.c_str(), file_name, data_format,
          (uint8_t *)binary_description.c_str(), binary_description.length());
      return true;
    } catch (std::exception &e) {
      next::sdk::logger::warn(__FILE__, "Failed to register source with {0} using data from the memory. {1}",
                              data_format, e.what());
      return false;
    }
  }

private:
  std::string data_source_name_;
  uint16_t source_id_;
  uint32_t instance_number_;
  std::string format_identifier_;

  std::shared_ptr<mts::runtime::udex::extractor_proxy> udex_extractor_ptr_;

  next::udex::HashManager hash_manager_;
};

} // namespace udex
} // namespace next
#endif // NEXT_CORE_LIB_HASH_MANAGER_H

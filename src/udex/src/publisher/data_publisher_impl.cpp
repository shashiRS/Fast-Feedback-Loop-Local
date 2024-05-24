/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
/**
 * @file     data_publisher_impl.cpp
 * @brief    Class to handle the registration of a new data provider
 *           to enable publishing data via the middleware network
 */

#include "data_publisher_impl.h"

#include <mts/toolbox/can/can_types.h>
#include <mts/toolbox/ecu/ecu_types.h>
#include <mts/toolbox/can/can.hpp>
#include <mts/toolbox/ecu/ecu.hpp>
#include <mts/toolbox/ethernet/ethernet.hpp>
#include <mts/toolbox/gps/gps.hpp>

#include "next/appsupport/config/config_client.hpp"
#include "next/appsupport/config/config_key_generator.hpp"

#include <next/sdk/profiler/profile_tags.hpp>
#include <next/sdk/sdk.hpp>
#include <next/sdk/types/package_data_types.hpp>

namespace next {
namespace udex {
namespace publisher {

static const std::unordered_map<DescriptionType, std::string> description_type_ = {
    {DESCRIPTION_TYPE_SDL, "sdl"},
    {DESCRIPTION_TYPE_CDL, "cdl"},
    {DESCRIPTION_TYPE_DBC, "dbc"},
    {DESCRIPTION_TYPE_SWC, "swc"},
    {DESCRIPTION_TYPE_FIBEX, "fibex"},
    {DESCRIPTION_TYPE_ARXML, "arxml"},
    {DESCRIPTION_TYPE_GPS_SDL, "gps-sdl"},
    {DESCRIPTION_TYPE_SW_CONTAINER, "sw_container"},
    {DESCRIPTION_TYPE_REFERENCE_CAMERA_SDL, "reference-camera-sdl"}};

DataPublisherImpl::DataPublisherImpl(const std::string &data_publisher_name)
    : data_publisher_name_(data_publisher_name) {
  core_lib.Initialize();
}

DataPublisherImpl::~DataPublisherImpl() {

  core_lib.Terminate();
  url_hash_map_.clear();

  deletePublishers();
}

bool DataPublisherImpl::Reset(void) {

  core_lib.Terminate();
  url_hash_map_.clear();
  core_lib.Initialize();

  deletePublishers();

  return true;
}

std::string DataPublisherImpl::getName() { return data_publisher_name_; }

bool DataPublisherImpl::SetDataSourceInfo(const std::string &data_source_name, uint16_t source_id,
                                          uint32_t instance_number, const std::string &format_type) {
  auto ret = false;

  ret = core_lib.SetDataSourceInfo(data_source_name, source_id, instance_number, format_type);

  return ret;
}

void DataPublisherImpl::createPublishersFromTopicList(const bool create_publishers, bool use_type_schema,
                                                      statusCallbackT callback) {
  if (callback)
    callback("DataPublisherImpl", "Function [createPublishersFromTopicList] start", false, 0, 0);

  auto topic_list = core_lib.GetNewRegisteredTopicsList();
  size_t index = 0u;
  std::vector<std::string> add_topics_update;
  for (const auto &topic : topic_list) {
    index++;
    auto hash = topic.second.hash;
    if (callback)
      callback("createPublishersFromTopicList", "Creating topic: " + topic.first, true, index, topic_list.size());

    if (create_publishers) {
      auto pub = std::make_shared<ecal_util::DynamicPublisher>(true);
      publishers_[hash] = pub;

      next::udex::SignalDescription description;
      description.basic_info.parent_url = topic.first;
      description.basic_info.data_source_name = topic.second.data_source_name;
      description.basic_info.view_name = get_view_name(topic.first);
      description.basic_info.group_name = get_group_name(topic.first);
      description.basic_info.format_type = next::sdk::types::getPackageFormatTypeEnum(topic.second.format_type_string);
      description.basic_info.cycle_id = topic.second.cycle_id;
      description.basic_info.vaddr = topic.second.vaddr;
      description.basic_info.instance_id = topic.second.instance_id;
      description.basic_info.source_id = topic.second.source_id;

      // Access type schema if it is expected that this topic will have a schema
      if (use_type_schema) {
        std::string schema;
        if (core_lib.GetSchema(description.basic_info.parent_url, description.basic_info.source_id, schema)) {
          description.binary_data_description.assign(schema.begin(), schema.end());
        }
      }

      if (!pub->CreateTopic(topic.first, description)) {
        sdk::logger::error(__FILE__, "Failed to create topic: {}", topic.first);
      }
      publisher_creation_time_[hash] =
          std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
              .count();
      add_topics_update.push_back(topic.first);
    }
    url_hash_map_.insert(std::make_pair(topic.first, hash));
  }
  if (create_publishers) {
    NotifyUpdateTopicCache(add_topics_update, UpdateType::ADD_TOPICS);
  }
  core_lib.ClearNewRegisteredTopicsList();
  if (callback)
    callback("DataPublisherImpl", "Function [createPublishersFromTopicList] end", false, 0, 0);
}

bool DataPublisherImpl::RegisterDataDescription(const std::string &description_filepath, const bool create_publishers,
                                                DescriptionType description_type, statusCallbackT callback) {
  std::string format = "sdl";
  const auto found_type = description_type_.find(description_type);
  if (found_type != description_type_.end()) {
    format = description_type_.at(description_type);
  }

  if (!core_lib.RegisterDataSources(description_filepath, format)) {
    return false;
  }

  createPublishersFromTopicList(create_publishers, true, callback);

  return true;
}

bool DataPublisherImpl::RegisterDataDescription(const char *file_name, void *binary_description,
                                                const size_t content_size, const bool create_publishers,
                                                DescriptionType description_type, statusCallbackT callback) {

  if (!core_lib.RegisterDataSources(std::string(file_name), std::string((const char *)binary_description, content_size),
                                    description_type_.at(description_type))) {
    return false;
  }

  createPublishersFromTopicList(create_publishers, true, callback);

  return true;
}

bool DataPublisherImpl::RegisterDataDescription(const next::sdk::types::DataDescription &data_description,
                                                statusCallbackT callback) {
  const std::unordered_map<next::sdk::types::DataDescriptionFormatType, DescriptionType> player_format_to_udex = {
      {next::sdk::types::DESCRIPTION_FORMAT_TYPE_UNKNOWN, DESCRIPTION_TYPE_SDL},
      {next::sdk::types::DESCRIPTION_FORMAT_TYPE_SDL, DESCRIPTION_TYPE_SDL},
      {next::sdk::types::DESCRIPTION_FORMAT_TYPE_CDL, DESCRIPTION_TYPE_CDL},
      {next::sdk::types::DESCRIPTION_FORMAT_TYPE_DBC, DESCRIPTION_TYPE_DBC},
      {next::sdk::types::DESCRIPTION_FORMAT_TYPE_SWC, DESCRIPTION_TYPE_SWC},
      {next::sdk::types::DESCRIPTION_FORMAT_TYPE_FIBEX, DESCRIPTION_TYPE_FIBEX},
      {next::sdk::types::DESCRIPTION_FORMAT_TYPE_ARXML, DESCRIPTION_TYPE_ARXML},
      {next::sdk::types::DESCRIPTION_FORMAT_TYPE_GPS_SDL, DESCRIPTION_TYPE_GPS_SDL},
      {next::sdk::types::DESCRIPTION_FORMAT_TYPE_SW_CONTAINER, DESCRIPTION_TYPE_SW_CONTAINER},
      {next::sdk::types::DESCRIPTION_FORMAT_TYPE_REFERENCE_CAMERA_SDL, DESCRIPTION_TYPE_REFERENCE_CAMERA_SDL}};
  bool success = true;
  try {
    success &=
        SetDataSourceInfo(data_description.device_name, data_description.source_id, data_description.instance_number,
                          next::sdk::types::getPackageFormatTypeString(data_description.device_format_type));

    success &=
        RegisterDataDescription(data_description.filename.c_str(), (void *)(data_description.binary_description.data()),
                                data_description.binary_description.size(), true,
                                player_format_to_udex.find(data_description.description_format_type)->second, callback);
  } catch (std::exception &e) {
    warn(__FILE__, "Failed to register sources to udex service: \"{}\"", e.what());
    return false;
  }
  return success;
}

bool DataPublisherImpl::RegisterDataDescription(const std::string &port_name, size_t port_size,
                                                const bool create_publishers,
                                                [[maybe_unused]] const std::string &description_type,
                                                statusCallbackT callback) {

  if (!core_lib.RegisterDataSources(port_name, port_size)) {
    return false;
  }
  // Type schema should not be used in this case because
  // the port does not have a data description associated with it
  createPublishersFromTopicList(create_publishers, false, callback);

  return true;
}
const std::unordered_map<std::string, size_t> &DataPublisherImpl::getTopicsAndHashes() {
  return core_lib.GetTopicsAndHashes();
}

bool DataPublisherImpl::publishPackage(std::unique_ptr<sdk::types::IPackage> &package) {
  bool success = publishProcessorPackages(package);
  // publish now the original unprocessed package
  success &= publishPackageInternal(package);
  return success;
}

bool DataPublisherImpl::publishProcessorPackages(std::unique_ptr<sdk::types::IPackage> &package) {
  bool success = true;
  std::vector<std::unique_ptr<sdk::types::IPackage>> processor_packages;
  int watchdog_package_not_stuck = 0;
  while (package->PayloadAvailable()) {
    if (watchdog_package_not_stuck == 98) {
      warn(__FILE__, "limit of 100 subpackages exceeded. Please create smaller packages");
    }
    if (watchdog_package_not_stuck > 100) {
      return false;
    }
    watchdog_package_not_stuck++;
    size_t hash;
    std::string package_name;
    GetPackageNameAndHash(package, hash, package_name);
    auto foundProcessors = udex_manager.GetProcessorByHash(hash);
    if (foundProcessors.empty()) {
      // skip to the next part of the package
      package->GetPayload();
    } else {
      for (const auto &processor : foundProcessors) {
        auto new_package = processor->processPackage(package);
        processor_packages.push_back(std::move(new_package));
      }
    }
  }
  package->ResetPayloads();

  for (const auto &processor_package : processor_packages) {
    success &= publishPackageInternal(processor_package);
  }
  return success;
}

bool DataPublisherImpl::publishPackageInternal(const std::unique_ptr<sdk::types::IPackage> &package) {

  bool success = true;
  int watchdog_package_not_stuck = 0;
  while (package->PayloadAvailable()) {
    if (watchdog_package_not_stuck == 98) {
      warn(__FILE__, "limit of 100 subpackages exceeded. Please create smaller packages");
    }
    if (watchdog_package_not_stuck > 100) {
      return false;
    }
    watchdog_package_not_stuck++;
    size_t hash;
    std::string package_name;
    GetPackageNameAndHash(package, hash, package_name);

    auto it_publisher_map = publishers_.find(hash);
    if (it_publisher_map == publishers_.end()) {
      // dummy load the payload to get rid of it
      package->GetPayload();
      continue;
    }

    size_t now_since_epoc_ms = waitForCreation(static_cast<size_t>(publisher_creation_time_[it_publisher_map->first]));
    std::shared_ptr<ecal_util::DynamicPublisher> pub = it_publisher_map->second;
    success = SendPackage(package, pub, now_since_epoc_ms);
  }
  return success;
}
void DataPublisherImpl::GetPackageNameAndHash(const std::unique_ptr<sdk::types::IPackage> &package, size_t &hash,
                                              std::string &package_name) {
  hash = 0;
  package_name = "";
  switch (package->GetMetaType()) {
  case sdk::types::IPackage::PACKAGE_META_TYPE_NAME:
    package_name = package->GetPackageName();
    hash = core_lib.GetTopicHash(package_name, false);
    break;
  case sdk::types::IPackage::PACKAGE_META_TYPE_INFO:
    hash = sdk::types::PackageHash::hashMetaInfo(package->GetMetaInfo());
    break;
  case sdk::types::IPackage::PACKAGE_META_TYPE_VADDR:
    hash = core_lib.GetTopicHash(package->GetPackageVaddr());
    break;
  default:
    error(__FILE__, "Unknown package type {0}- handling not implemented", static_cast<int>(package->GetMetaType()));
  }
}

size_t DataPublisherImpl::waitForCreation(const size_t creation_time) const {
  size_t now = static_cast<size_t>(
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count());
  while (now - creation_time < PUBLISHER_INIT_TO_SEND_DELAY) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    now = static_cast<size_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count());
  }
  return now;
}

bool DataPublisherImpl::SendPackage(const std::unique_ptr<sdk::types::IPackage> &package,
                                    const std::shared_ptr<ecal_util::DynamicPublisher> &pub, size_t fallback_time) {
  size_t package_time = package->GetPackageHeader().timestamp;
  if (package_time == 0) {
    package_time = fallback_time;
  }

  ecal_util::TopicSendMessage msg;
  msg.package_info_in = nullptr;
  msg.package_info_size = 0;
  auto payload = package->GetPayload();
  msg.binary_blob_size = payload.size;
  msg.binary_blob_in = payload.mem_pointer;

  // THIS IS DUMMY VLAUES -> SHOULD BE ADAPTED IN THE META HEADER
  msg.meta.mts_timestamp = package_time;
  msg.meta.format_type = package->GetPackageHeader().format;
  msg.meta.cycle_state = 4; // no needed but set right now
  msg.meta.dynamic_package = true;
  msg.meta.mode = PACKAGE_MODE_SDL;                 // dummy
  msg.meta.package_id = pub->getCurrentMsgNumber(); // dummy
  msg.meta.processor_required = package->processor_required_;

  return pub->SendTopic(msg);
}

bool DataPublisherImpl::publishData(const uint64_t vaddr, const void *binary_data, const size_t binary_size,
                                    const uint64_t trigger_timestamp) {
  auto found_hash = core_lib.GetTopicHash(vaddr);
  auto it_publisher_map = publishers_.find(found_hash);
  if (it_publisher_map == publishers_.end()) {
    return false;
  }
  auto pub = it_publisher_map->second;
  bool send_res = true;
  size_t now_since_epoc_ms = waitForCreation(static_cast<size_t>(publisher_creation_time_[it_publisher_map->first]));
  send_res &= SendMsg(binary_data, binary_size, trigger_timestamp, pub, now_since_epoc_ms);
  return send_res;
}

bool DataPublisherImpl::publishData(const std::string &port_name, const void *binary_data, const size_t binary_size,
                                    bool append_device, const uint64_t trigger_timestamp) {

  auto found_hash = core_lib.GetTopicHash(port_name, append_device);
  auto it_publisher_map = publishers_.find(found_hash);
  if (it_publisher_map == publishers_.end()) {
    return false;
  }
  std::shared_ptr<ecal_util::DynamicPublisher> pub = it_publisher_map->second;
  size_t now_since_epoc_ms = waitForCreation(static_cast<size_t>(publisher_creation_time_[it_publisher_map->first]));
  return SendMsg(binary_data, binary_size, trigger_timestamp, pub, now_since_epoc_ms);
}

bool DataPublisherImpl::SendMsg(const void *binary_data, const size_t binary_size, const uint64_t trigger_timestamp,
                                const std::shared_ptr<ecal_util::DynamicPublisher> &pub,
                                size_t now_since_epoc_ms) const {
  ecal_util::TopicSendMessage msg;
  msg.package_info_in = nullptr;
  msg.package_info_size = 0;
  msg.binary_blob_size = binary_size;
  msg.binary_blob_in = binary_data;

  // THIS IS DUMMY VLAUES -> SHOULD BE ADAPTED IN THE META HEADER
  if (trigger_timestamp != 0) {
    msg.meta.mts_timestamp = trigger_timestamp;
  } else {
    msg.meta.mts_timestamp = static_cast<uint64_t>(now_since_epoc_ms);
  }
  msg.meta.format_type = sdk::types::getPackageFormatTypeEnum("mts.mta.sw");
  msg.meta.cycle_state = mts_package_cycle_body;
  msg.meta.dynamic_package = true;
  msg.meta.package_id = 123456778;  // dummy
  msg.meta.mode = PACKAGE_MODE_SDL; // dummy

  return pub->SendTopic(msg);
}

std::string DataPublisherImpl::GetFilterExpressionFromURLs(const std::vector<std::string> &urls) const {
  return core_lib.GetFilterExpressionFromURLs(urls);
}

void DataPublisherImpl::deletePublishers(void) {
  if (!publishers_.empty()) {
    std::vector<std::string> rmv_topics;
    for (auto publisher_list : publishers_) {
      auto pub = publisher_list.second;
      rmv_topics.push_back(pub->GetTopicName());
      pub->ResetTopic();
    }

    publishers_.clear();
    NotifyUpdateTopicCache(rmv_topics, UpdateType::REMOVE_TOPICS);
  }
}

bool DataPublisherImpl::LoadProcessors(const std::string &path) {
  udex_manager.loadProcessorDlls(path);

  for (auto processor : udex_manager.GetNewProcessors()) {

    const auto &package_infos = processor.processor_instance->provideRequestedPackageInfo();
    const auto &descriptions = processor.processor_instance->provideDataDescription();

    for (const next::sdk::types::DataDescription &description : descriptions) {
      RegisterDataDescription(description);
    }

    int counter = 0;
    for (const auto &meta_info : package_infos) {
      size_t hash_temp = next::sdk::types::PackageHash::hashMetaInfo(meta_info);
      SetProcessorInfoToConfig(processor.name + std::to_string(counter), meta_info);
      counter++;
      if (publishers_.find(hash_temp) == publishers_.end()) {
        next::sdk::logger::info(__FILE__, "No publisher found for requested processor info \n{}", hash_temp,
                                meta_info.instance_number, meta_info.cycle_id, meta_info.source_id);
      }
    }
  }
  return true;
}

void DataPublisherImpl::SetProcessorInfoToConfig(
    const std::string &processor_name, const next::sdk::types::IPackage::PackageMetaInfo &package_infos) const {
  auto config = next::appsupport::ConfigClient::getConfig();
  std::string component_name = config->getRootName();
  using namespace next::appsupport::configkey;
  config->put(appsupport::configkey::getProcessorInstanceCycleId(component_name, processor_name),
              std::to_string(package_infos.cycle_id));
  config->put(appsupport::configkey::getProcessorInstanceInstanceNumber(component_name, processor_name),
              std::to_string(package_infos.instance_number));
  config->put(appsupport::configkey::getProcessorInstanceSourceId(component_name, processor_name),
              std::to_string(package_infos.source_id));
  config->put(appsupport::configkey::getProcessorInstanceVirtualAddress(component_name, processor_name),
              std::to_string(package_infos.virtual_address));
}

void DataPublisherImpl::NotifyUpdateTopicCache(std::vector<std::string> &topic_list, UpdateType op_type) {
  next::sdk::events::UpdateTopicCacheMessage msg;
  if (op_type == UpdateType::REMOVE_TOPICS) {
    msg.topics = topic_list;
  }
  update_topic_cache_event_.publish(msg);
}

} // namespace publisher
} // namespace udex
} // namespace next

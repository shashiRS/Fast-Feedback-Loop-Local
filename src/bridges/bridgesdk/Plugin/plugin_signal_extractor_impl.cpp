/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_signal_extractor_impl.cpp
 * @brief    implements the signal extractor for the plugin
 **/

#include "plugin_signal_extractor_impl.hpp"
#include "plugin_private.h"

namespace next {
namespace bridgesdk {
namespace plugin_addons {

PluginSignalExtractorImpl::PluginSignalExtractorImpl(next::bridgesdk::plugin::Plugin *base_plugin) {
  base_plugin_ = base_plugin;
}

UdexDataPackage PluginSignalExtractorImpl::GetRawData(const std::string &topic_name) {
  next::udex::UdexDataPackage udex_data_package = base_plugin_->impl_->getExtractorInstance()->GetRawData(topic_name);
  UdexDataPackage local_udex_data_package;
  memcpy(const_cast<void *>(local_udex_data_package.Data), udex_data_package.Data, udex_data_package.Size);
  local_udex_data_package.format_type = static_cast<FormatType>(udex_data_package.info.format_type);
  local_udex_data_package.PackageUrl = udex_data_package.info.PackageUrl;
  local_udex_data_package.Size = udex_data_package.Size;
  local_udex_data_package.Time = udex_data_package.info.Time;
  return local_udex_data_package;
}

bool PluginSignalExtractorImpl::isDataAvailable(const std::string &topic_name) {
  return base_plugin_->impl_->getExtractorInstance()->isDataAvailable(topic_name);
}

bool PluginSignalExtractorImpl::GetValue(const std::string &signal_url, const SignalType type, dataType &return_value,
                                         SignalInfo *signal_info) {
  next::udex::SignalType udex_type = static_cast<next::udex::SignalType>(type);
  return base_plugin_->impl_->getExtractorInstance()->GetValue(signal_url, udex_type, return_value,
                                                               (next::udex::SignalInfo *)signal_info);
}

TypeConversionInfo PluginSignalExtractorImpl::GetValueWithTypeChecking(const std::string &signal_url,
                                                                       const SignalType type, dataType &return_value,
                                                                       SignalInfo *signal_info) {
  auto type_info = base_plugin_->impl_->getExtractorInstance()->GetInfo(signal_url);
  if ((type > next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_STRUCT) ||
      (type_info.signal_type > next::udex::SignalType::SIGNAL_TYPE_STRUCT)) {
    error(__FILE__, "Conversion Matrix index out of bounds");
    return next::bridgesdk::plugin_addons::TypeConversionInfo::INVALID;
  }
  if ((type == next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_STRUCT) ||
      (type_info.signal_type == next::udex::SignalType::SIGNAL_TYPE_STRUCT)) {
    warn(__FILE__, "No conversion available for type Struct");
    return next::bridgesdk::plugin_addons::TypeConversionInfo::INVALID;
  }
  auto conversion_info =
      KtypeMatrix.at(KdataTypeIndex.at(type)).at(KdataTypeIndex.at(static_cast<SignalType>(type_info.signal_type)));
  next::udex::SignalType udex_type = static_cast<next::udex::SignalType>(type);

  if (base_plugin_->impl_->getExtractorInstance()->GetValue(signal_url, udex_type, return_value,
                                                            (next::udex::SignalInfo *)signal_info)) {
    return conversion_info;
  } else {
    return TypeConversionInfo::INVALID;
  }
}

bool PluginSignalExtractorImpl::GetVectorValue(const std::string &signal_url, const SignalType type,
                                               std::vector<dataType> &return_vector,
                                               [[maybe_unused]] SignalInfo *signal_info) {
  next::udex::SignalType udex_type = static_cast<next::udex::SignalType>(type);
  return base_plugin_->impl_->getExtractorInstance()->GetVectorValue(signal_url, udex_type, return_vector);
}

bool PluginSignalExtractorImpl::GetValueAndInfo(const std::string &signal_url, SignalInfo &signal_info,
                                                std::vector<dataType> &return_vector) {
  next::udex::SignalInfo udex_info;
  bool res = base_plugin_->impl_->getExtractorInstance()->GetValueAndInfo(signal_url, udex_info, return_vector);
  signal_info = {static_cast<SignalType>(udex_info.signal_type), udex_info.signal_size, udex_info.array_size,
                 udex_info.offset};

  return res;
}

SignalInfo PluginSignalExtractorImpl::GetInfo(const std::string &signal_url) {
  next::udex::SignalInfo result = base_plugin_->impl_->getExtractorInstance()->GetInfo(signal_url);
  SignalInfo bridges_signal_info = {static_cast<SignalType>(result.signal_type), result.signal_size, result.array_size,
                                    result.offset};
  return bridges_signal_info;
}

void PluginSignalExtractorImpl::ClearExtractedQueue(const std::string &topic_name) {
  return base_plugin_->impl_->getExtractorInstance()->ClearExtractedQueue(topic_name);
}

void *PluginSignalExtractorImpl::GetExtractedData(const std::string &topic_name, size_t &out_size, time_t &time) {
  auto package = base_plugin_->impl_->getExtractorInstance()->GetExtractedData(topic_name);
  out_size = package.size;
  time = package.timestamp;
  return package.data;
}

std::list<std::shared_ptr<next::bridgesdk::TimestampedDataPacket>>
PluginSignalExtractorImpl::GetExtractedQueue(const std::string &topic_name) {
  std::list<std::shared_ptr<next::bridgesdk::TimestampedDataPacket>> extracted_que;
  for (const auto &it : base_plugin_->impl_->getExtractorInstance()->GetExtractedQueueWithTimestamp(topic_name)) {
    next::bridgesdk::TimestampedDataPacket insert_elemt;
    insert_elemt.data = it->data;
    insert_elemt.size = it->size;
    insert_elemt.timestamp = it->timestamp;
    extracted_que.push_back(std::make_shared<next::bridgesdk::TimestampedDataPacket>(insert_elemt));
  }
  return extracted_que;
}

} // namespace plugin_addons
} // namespace bridgesdk
} // namespace next

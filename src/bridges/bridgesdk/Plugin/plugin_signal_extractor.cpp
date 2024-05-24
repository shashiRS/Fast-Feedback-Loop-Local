/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_signal_extractor.cpp
 * @brief    implements the signal extractor for the plugin
 **/

#include <next/bridgesdk/plugin_addons/plugin_signal_extractor.hpp>

#include "plugin_signal_extractor_impl.hpp"

namespace next {
namespace bridgesdk {
namespace plugin_addons {

PluginSignalExtractor::PluginSignalExtractor(next::bridgesdk::plugin::Plugin *base_plugin) {
  impl_ = std::make_unique<PluginSignalExtractorImpl>(base_plugin);
}

PluginSignalExtractor::~PluginSignalExtractor() {}

TypeConversionInfo PluginSignalExtractor::GetValueWithTypeChecking(const std::string &signal_url, const SignalType type,
                                                                   dataType &return_value, SignalInfo *signal_info) {
  return impl_->GetValueWithTypeChecking(signal_url, type, return_value, signal_info);
}

UdexDataPackage PluginSignalExtractor::GetRawData(const std::string &topic_name) {
  return impl_->GetRawData(topic_name);
}

bool PluginSignalExtractor::isDataAvailable(const std::string &topic_name) {
  return impl_->isDataAvailable(topic_name);
}

bool PluginSignalExtractor::GetValue(const std::string &signal_url, const SignalType type, dataType &return_value,
                                     SignalInfo *signal_info) {
  return impl_->GetValue(signal_url, type, return_value, signal_info);
}

bool PluginSignalExtractor::GetVectorValue(const std::string &signal_url, const SignalType type,
                                           std::vector<dataType> &return_vector, SignalInfo *signal_info) {
  return impl_->GetVectorValue(signal_url, type, return_vector, signal_info);
}
bool PluginSignalExtractor::GetValueAndInfo(const std::string &signal_url, SignalInfo &signal_info,
                                            std::vector<dataType> &return_vector) {
  return impl_->GetValueAndInfo(signal_url, signal_info, return_vector);
}
void PluginSignalExtractor::ClearExtractedQueue(const std::string &topic_name) {
  return impl_->ClearExtractedQueue(topic_name);
}

void *PluginSignalExtractor::GetExtractedData(const std::string &topic_name, size_t &out_size, time_t &time) {
  return impl_->GetExtractedData(topic_name, out_size, time);
}
SignalInfo PluginSignalExtractor::GetInfo(const std::string &signal_url) { return impl_->GetInfo(signal_url); }
std::list<std::shared_ptr<next::bridgesdk::TimestampedDataPacket>>
PluginSignalExtractor::GetExtractedQueue(const std::string &topic_name) {
  return impl_->GetExtractedQueue(topic_name);
}

} // namespace plugin_addons
} // namespace bridgesdk
} // namespace next

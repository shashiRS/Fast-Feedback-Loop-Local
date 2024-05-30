/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_datapublisher.cpp
 * @brief    implements the datapublisher for the plugin
 **/

#include <next/bridgesdk/plugin_addons/plugin_datapublisher.hpp>

#include "plugin_datapublisher_impl.hpp"

namespace next {
namespace bridgesdk {
namespace plugin_addons {

PluginDataPublisher::PluginDataPublisher(next::bridgesdk::plugin::Plugin *base_plugin) {
  impl_ = std::make_unique<PluginDataPublisherImpl>(base_plugin);
}

void PluginDataPublisher::sendResultData(const uint8_t *payload, const size_t size,
                                         const std::vector<size_t> &session_ids,
                                         const std::chrono::milliseconds timeout, const bool force_no_cache) {
  impl_->sendResultData(payload, size, session_ids, timeout, force_no_cache);
}

void PluginDataPublisher::sendFlatbufferData(const std::string &path_to_fbs,
                                             const next::bridgesdk::plugin::FlatbufferPackage &flatbuffer,
                                             const next::bridgesdk::plugin::SensorInfoForFbs &sensor_info,
                                             const std::pair<std::string, std::string> &structure_version_pair) {
  impl_->sendFlatbufferData(path_to_fbs, flatbuffer, sensor_info, structure_version_pair);
}

PluginDataPublisher::~PluginDataPublisher() {}

} // namespace plugin_addons
} // namespace bridgesdk
} // namespace next

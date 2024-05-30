/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_datapublisher_impl.cpp
 * @brief    implements the datapublisher for the plugin
 **/

#include "plugin_datapublisher_impl.hpp"
#include "plugin_private.h"

namespace next {
namespace bridgesdk {
namespace plugin_addons {

PluginDataPublisherImpl::PluginDataPublisherImpl(next::bridgesdk::plugin::Plugin *base_plugin) {
  base_plugin_ = base_plugin;
}

void PluginDataPublisherImpl::sendResultData(const uint8_t *payload, const size_t size,
                                             [[maybe_unused]] const std::vector<size_t> &session_ids,
                                             const std::chrono::milliseconds timeout, const bool force_no_cache) {
  base_plugin_->impl_->sendResultData("hello", payload, size, timeout, force_no_cache);
}

void PluginDataPublisherImpl::sendFlatbufferData(
    const std::string &path_to_fbs, const next::bridgesdk::plugin::FlatbufferPackage &flatbuffer,
    const next::bridgesdk::plugin::SensorInfoForFbs &sensor_info,
    const std::pair<std::string, std::string> &structure_version_pair) const {
  if (base_plugin_->impl_->data_receiver_) {
    base_plugin_->impl_->data_receiver_->PublishFlatbufferDataToClient(path_to_fbs, flatbuffer, sensor_info,
                                                                       structure_version_pair);
  }
}

} // namespace plugin_addons
} // namespace bridgesdk
} // namespace next

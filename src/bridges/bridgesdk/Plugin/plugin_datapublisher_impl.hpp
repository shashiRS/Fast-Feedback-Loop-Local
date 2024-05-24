/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_datapublisher_impl.hpp
 * @brief    implements the datapublisher for the plugin
 **/

#ifndef NEXT_BRIDGES_PLUGIN_DATAPUBLISHER_IMPL_H_
#define NEXT_BRIDGES_PLUGIN_DATAPUBLISHER_IMPL_H_

#include <next/bridgesdk/plugin.h>

namespace next {
namespace bridgesdk {
namespace plugin_addons {

class PluginDataPublisherImpl {

private:
  PluginDataPublisherImpl() = delete;

public:
  PluginDataPublisherImpl(next::bridgesdk::plugin::Plugin *base_plugin);
  virtual ~PluginDataPublisherImpl() = default;

  void sendResultData(const uint8_t *payload, const size_t size, const std::vector<size_t> &session_ids = {},
                      const std::chrono::milliseconds timeout = std::chrono::milliseconds(1000),
                      const bool force_no_cache = false);

  void sendFlatbufferData(const std::string &path_to_fbs, const next::bridgesdk::plugin::FlatbufferPackage &flatbuffer,
                          const next::bridgesdk::plugin::SensorInfoForFbs &sensor_info,
                          const std::pair<std::string, std::string> &structure_version_pair) const;

  next::bridgesdk::plugin::Plugin *base_plugin_{};
};

} // namespace plugin_addons
} // namespace bridgesdk
} // namespace next

#endif // NEXT_BRIDGES_PLUGIN_DATAPUBLISHER_IMPL_H_

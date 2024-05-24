/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_datapublisher.hpp
 * @brief    utility addon to output data
 **/

#ifndef NEXT_BRIDGESDK_PLUGIN_DATAPUBLISHER_H_
#define NEXT_BRIDGESDK_PLUGIN_DATAPUBLISHER_H_

#include <chrono>
#include <string>
#include <utility>
#include <vector>

#include <next/sdk/sdk_macros.h>

#include <next/bridgesdk/plugin.h>

namespace next {
namespace bridgesdk {
namespace plugin_addons {

class PluginDataPublisherImpl;

class DECLSPEC_plugin_config PluginDataPublisher {
private:
  PluginDataPublisher() = delete;

  NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS
  std::unique_ptr<PluginDataPublisherImpl> impl_;
  NEXT_RESTORE_WARNINGS_WINDOWS

public:
  PluginDataPublisher(next::bridgesdk::plugin::Plugin *base_plugin);
  virtual ~PluginDataPublisher();

  //! Sends data to all registered recipients
  /*!
   *  Iterates over all registered recipients and calls their data receiving function.
   *  Internally it uses PluginPrivate to do this.
   *  The timeout specifies how long the underlaying sender is waiting for a free memory block which is used to send
   *  the data. Don't use the value 0 since this could mean that your data might not be send.
   */
  virtual void sendResultData(const uint8_t *payload, const size_t size, const std::vector<size_t> &session_ids = {},
                              const std::chrono::milliseconds timeout = std::chrono::milliseconds(1000),
                              const bool force_no_cache = false);

  //! Sends flatbuffer data to all registered recipients
  /*!
   *  Iterates over all registered recipients and calls their data receiving function.
   *  Internally it uses sendResultData to do this.
   */
  virtual void sendFlatbufferData(const std::string &path_to_fbs,
                                  const next::bridgesdk::plugin::FlatbufferPackage &flatbuffer,
                                  const next::bridgesdk::plugin::SensorInfoForFbs &sensor_info,
                                  const std::pair<std::string, std::string> &structure_version_pair);
};

} // namespace plugin_addons
} // namespace bridgesdk
} // namespace next

#endif // NEXT_BRIDGESDK_PLUGIN_DATAPUBLISHER_H_

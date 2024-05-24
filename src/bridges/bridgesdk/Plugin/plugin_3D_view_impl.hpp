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

#ifndef NEXT_BRIDGES_PLUGIN_3D_VIEW_IMPL_H_
#define NEXT_BRIDGES_PLUGIN_3D_VIEW_IMPL_H_

#include <next/bridgesdk/plugin.h>
#include <next/bridgesdk/plugin_addons/plugin_3D_view.hpp>

namespace next {
namespace bridgesdk {
namespace plugin_addons {

class Plugin3dViewImpl {

private:
  Plugin3dViewImpl() = delete;

public:
  Plugin3dViewImpl(next::bridgesdk::plugin::Plugin *base_plugin);
  virtual ~Plugin3dViewImpl() = default;

  //! see Plugin::registerAndPublishSensor
  uid_t registerAndPublishSensor(const next::bridgesdk::plugin::SensorInfoForGUI &sensorInfo) const;

  //! see Plugin::unregisterAndPublishSensor
  void unregisterAndPublishSensor(const uid_t uid) const;

  next::bridgesdk::plugin::Plugin *base_plugin_ = {};
};

} // namespace plugin_addons
} // namespace bridgesdk
} // namespace next

#endif // NEXT_BRIDGES_PLUGIN_3D_VIEW_IMPL_H_

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

#include "plugin_3D_view_impl.hpp"
#include "plugin_private.h"

namespace next {
namespace bridgesdk {
namespace plugin_addons {

Plugin3dViewImpl::Plugin3dViewImpl(next::bridgesdk::plugin::Plugin *base_plugin) { base_plugin_ = base_plugin; }

uid_t Plugin3dViewImpl::registerAndPublishSensor(const next::bridgesdk::plugin::SensorInfoForGUI &sensorInfo) const {
  return base_plugin_->impl_->registerAndPublishSensor(sensorInfo);
}

void Plugin3dViewImpl::unregisterAndPublishSensor(const uid_t uid) const {
  base_plugin_->impl_->unregisterAndPublishSensor(uid);
}

} // namespace plugin_addons
} // namespace bridgesdk
} // namespace next

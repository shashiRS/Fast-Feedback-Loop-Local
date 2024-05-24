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

#include <next/bridgesdk/plugin_addons/plugin_3D_view.hpp>

#include "plugin_3D_view_impl.hpp"

namespace next {
namespace bridgesdk {
namespace plugin_addons {

Plugin3dView::Plugin3dView(next::bridgesdk::plugin::Plugin *base_plugin) {
  impl_ = std::make_unique<Plugin3dViewImpl>(base_plugin);
}

//! Register the Plugin / Sensor name in specific datastream and sets the global UID
/*!
 * if one plugin wants to publish data for multiple sensors, this method can be called multiple times
 * but the plugin needs to take care of multiple UIDs
 * this call wil also publish the sensorname to GUI
 * If it is already registered, than the known UID will be returned. It will not be added again
 * Internally it uses PluginPrivate to do this.
 * @param name the sensor / plugin name which will be visible in GUI
 * @param stream the stream name for GUI which this sensors belongs to
 * @return uid_t
 */
uid_t Plugin3dView::registerAndPublishSensor(const next::bridgesdk::plugin::SensorInfoForGUI &sensorInfo) const {
  return (impl_->registerAndPublishSensor(sensorInfo));
}

//! Unregister the Plugin / Sensor name from list
/*!
 * this call wil also publish the updated sensorlist to GUI
 *  Internally it uses PluginPrivate to do this.
 * @param uid_t the sensor UID to be removed
 */
void Plugin3dView::unregisterAndPublishSensor(const uid_t uid) const { impl_->unregisterAndPublishSensor(uid); }

Plugin3dView::~Plugin3dView() {}

} // namespace plugin_addons
} // namespace bridgesdk
} // namespace next

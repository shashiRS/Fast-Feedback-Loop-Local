/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_3D_view.hpp
 * @brief    utility addon to output data
 **/

#ifndef NEXT_BRIDGESDK_PLUGIN_3DVIEW_H
#define NEXT_BRIDGESDK_PLUGIN_3DVIEW_H

#include <atomic>
#include <map>
#include <memory>
#include <mutex>

#include <next/bridgesdk/plugin.h>

namespace next {
namespace bridgesdk {
namespace plugin_addons {

class Plugin3dViewImpl;

//! defines type for all plugin UIDs
using uid_t = uint32_t;

//! provides all Informations about registered SensorInfos at GUI
struct RegisteredSensors {
  std::atomic<uid_t> uid_counter{1};
  std::recursive_mutex sensor_list_mutex;
  std::map<uid_t, next::bridgesdk::plugin::SensorInfoForGUI> sensor_list{};
};

class DECLSPEC_plugin_config Plugin3dView {
private:
  Plugin3dView() = delete;

  NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS
  std::unique_ptr<Plugin3dViewImpl> impl_;
  NEXT_RESTORE_WARNINGS_WINDOWS

public:
  Plugin3dView(next::bridgesdk::plugin::Plugin *base_plugin);
  virtual ~Plugin3dView();

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
  uid_t registerAndPublishSensor(const next::bridgesdk::plugin::SensorInfoForGUI &sensorInfo) const;

  //! Unregister the Plugin / Sensor name from list
  /*!
   * this call wil also publish the updated sensorlist to GUI
   *  Internally it uses PluginPrivate to do this.
   * @param uid_t the sensor UID to be removed
   */
  void unregisterAndPublishSensor(const uid_t uid) const;
};

} // namespace plugin_addons
} // namespace bridgesdk
} // namespace next

#endif // NEXT_BRIDGESDK_PLUGIN_3DVIEW_H

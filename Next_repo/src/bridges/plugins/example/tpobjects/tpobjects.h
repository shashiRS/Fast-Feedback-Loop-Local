/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     tpobjects.h
 * @brief    DO NOT USE. Still work in progress.
 *
 * Development is ongoing.
 *
 **/

#ifndef NEXT_PLUGINS_TPF_BASE_TPOBJECTS_H_
#define NEXT_PLUGINS_TPF_BASE_TPOBJECTS_H_

#include <map>
#include <memory>
#include <string>

#include <next/bridgesdk/plugin.h>
#include <next/bridgesdk/plugin_addons/plugin_3D_view.hpp>
#include <next/bridgesdk/plugin_addons/plugin_datapublisher.hpp>
#include <next/bridgesdk/plugin_addons/plugin_datasubscriber.hpp>
#include <next/bridgesdk/plugin_addons/plugin_signal_explorer.hpp>
#include <next/bridgesdk/plugin_addons/plugin_signal_extractor.hpp>

#include <next/plugins_utils/plugins_types.h>

namespace next {
namespace plugins {

class TpObjects : public next::bridgesdk::plugin::Plugin {
public:
  TpObjects(const char *);
  virtual ~TpObjects();

  //! initialization
  bool init() override;

  //! not in use currently (later for resetting after successfull simulation)
  bool enterReset() override;
  bool exitReset() override;

  /*! provides plugin description to allow for selection in GUI
   *
   * @param config selection of keys and values
   * @return true if successfull
   */
  bool getDescription(next::bridgesdk::plugin_config_t &config_fields) const override;

  /*! adds a selected configuration provided by the GUI
   *
   * @param config selection of keys and values
   * @return true if successfull
   */
  bool addConfig(const next::bridgesdk::plugin_config_t &config) override;

  /*! removes a selected configuration provided by the GUI
   *
   * @param config selection of keys and values
   * @return true if successfull
   */
  bool removeConfig(const next::bridgesdk::plugin_config_t &config) override;

  /*! Internally used callback to transform incoming data to output
   *
   * @param topic_name of the data
   * @param datablock containing the data of this topic
   * @param sensor_config contains the sensor configuration for which this callback was subscribed
   * @param naming_map contains the list of signal names which are used from this topic
   */
  void ProcessTPObjectsCallback(const std::string topic_name, const next::bridgesdk::ChannelMessagePointers *datablock,
                                const next::plugins_utils::SensorConfig sensor_config,
                                std::map<std::string, std::string> naming_map);

  /*! returns the number of active sensor configurations in the plugin
   *
   * @return number of active sensor configurations
   */
  inline size_t numOfActiveConfig() { return active_sensors.size(); }

private:
  // path to *.fbs file which describes the plugin interface as flatbuffer schema file
  std::string fbs_path_;

  // list of sensor configurations used by the GUI
  std::list<next::plugins_utils::SensorConfig> active_sensors;

  /**********************************************/
  //********** plugin utility classes ***********/
  /**********************************************/
  // utility addon to subscribe data from UDex
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataSubscriber> plugin_data_subscriber_;
  // utility addon to send data to all registered clients (usually the GUI)
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataPublisher> plugin_publisher_;
  // utility addon to retrieve information from UDex
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExplorer> plugin_signal_explorer_;
  // utility addon to retrieve data from UDex
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExtractor> plugin_signal_extractor_;
  // utility addon to register and publish sensors at GUI
  std::shared_ptr<next::bridgesdk::plugin_addons::Plugin3dView> plugin_3D_view_;
};

} // namespace plugins
} // namespace next

extern "C" NEXTPLUGINSHARED_EXPORT plugin_create_t create_plugin(const char *path);

#endif // NEXT_PLUGINS_TPF_BASE_TPOBJECTS_H_

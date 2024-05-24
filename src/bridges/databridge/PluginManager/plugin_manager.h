/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_manager.h
 * @brief    Helper to manage all the plugins.
 *
 * This manager is assisting you to manage all the plugins. It is parsing a
 * given directory and checks there for plugins. It helps to load the libs,
 * assists to create instances of the functionalities and makes it easy to
 * provide data to the plugins.
 *
 **/

#ifndef NEXT_DATABRIDGE_PLUGIN_MANAGER_H_
#define NEXT_DATABRIDGE_PLUGIN_MANAGER_H_

#include <functional>
#include <list>
#include <memory>
#include <string>
#include <vector>

#include <next/control/event_types/player_state_event.h>
#include <next/sdk/events/status_code_event.hpp>
#include <next/udex/explorer/signal_explorer.hpp>
#include <next/udex/subscriber/data_subscriber.hpp>

#include <next/bridgesdk/plugin.h>
#include <next/bridgesdk/data_manager.hpp>

#include "plugin_config_handler.h"
#include "process_manager.h"

// helper class for testing the plugin manager
namespace next {
namespace test {
class PluginManagerTestHelper;
} // namespace test
} // namespace next

namespace next {
namespace databridge {
namespace plugin_manager {

typedef std::function<void(const next::bridgesdk::ChannelMessagePointers &)> plugin_data_process_function;
// TODO switch to something like this
// using plugin_data_process_function = std::function<decltype(&next::bridgesdk::plugin::Plugin::processData)>;

typedef plugin_create_t(plugin_constructor_t)(const char *);

typedef std::map<std::string, std::shared_ptr<next::bridgesdk::plugin::Plugin>> map_instances_per_source;

struct plugin_information {
  std::function<plugin_constructor_t> creator;
  std::list<std::string> input_urls;         // needed input, as URLs
  std::list<std::string> output_flatbuffers; // provided data types as flatbuffers (filenames)
  // contains: [string]: plugin source -> [string]: plugin instance name, [shared_ptr]: pointer to plugin instance
  std::map<std::string, map_instances_per_source> instances;
  std::string path;
};

//! The PluginManager holds all loaded plugins and provides
class PluginManager {
public:
  //! friend classes for testing
  friend class next::test::PluginManagerTestHelper;

  PluginManager();
  virtual ~PluginManager();

  //! Init the plugin manager
  /*!
   * @param path the directory to scan for plugins
   * @return false on error, true otherwise
   */
  bool init(const std::string &path) noexcept;

  //! Get a list of availble plugins
  /*!
   *  Get a list of available plugins. The name of a plugin is simply the folder name where its dll/so file
   *  is placed in.
   */
  std::list<std::string> getAvailablePlugins() const;

  //! connect the Pluginmanager to the backend so new plugins can be connected as well
  /*!
   * save pointers to current output data manager so plugins are able to send data
   * @param data_output_manager shared pointer to Data Manager
   * @return
   */
  bool connectToOutputDataManager(std::shared_ptr<next::databridge::data_manager::IDataManager> data_output_manager);

  //! Check whether the requested plugin is available.
  /*!
   *  The list of plugins is checked whether the name is found.
   *  Note: The check is case sensitive!
   */
  bool isAvailable(const std::string &plugin_name) const;

  //! Check for compatible plugins based on the flat buffer schema specified in data_class
  void getCompatiblePlugins(const std::string data_class, const std::string version,
                            std::list<std::string> &compatible_plugins);

  //! Adds all plugins for matching data class request for the specified source
  bool updatePluginConfigs(const std::string data_class, const std::string version, std::string source_name,
                           const std::vector<std::string> user_parameter);

  //! searches all plugins for matching data class and sends all plugin descriptions
  bool findAndSendDescription(const std::string data_class, const std::string version);

  //! searches all plugins for matching data class and set plugin configs for the specified source
  bool findAndSetConfig(const std::vector<bridgesdk::plugin_config_t> &config, std::string plugin_name,
                        std::string source_name);

  //! Create a instance of the named plugin with the given instance name.
  /*!
   *  Note: Instance names must be unique.
   *  When from the named plugin a instance with the same name already exists the function will return true.
   *  When the instance name is not used so far, and the plugin name is valid, a instance will be crated and the
   *  function returns true. If the plugin name is not valid the function returns false.
   */
  bool addPluginInstance(const std::string &plugin_name, const std::string &source_name,
                         const std::string &instance_name);

  //! Destroy the given plugin
  bool destroyPlugin(const std::string &plugin_name);

  //! shutdown plugin manager
  /*!
   *  Note: Destroys all plugin instances
   */
  void shutdown();

  //! Get the version of the plugin
  bool getPluginVersion(const std::string &plugin_name, const std::string &source_name,
                        const std::string &instance_name, std::string &plugin_version);

  //! Initialize the plugin with instance name
  bool initPlugin(const std::string &plugin_name, const std::string &source_name, const std::string &instance_name);

  //! Update the plugin with instance name
  bool updatePlugin(const std::string &plugin_name, const std::string &source_name, const std::string &instance_name,
                    std::vector<std::string> user_arg);

  //! Set Synchronization for all loaded plugins
  bool SetSynchronizationMode(bool enable);

  //! Set Synchronization for a specific instance of a plugin
  bool SetSynchronizationMode(const std::string &plugin_name, const std::string &source_name,
                              const std::string &instance_name, bool enable);

  //! Set PackageDrop for all loaded plugins
  bool SetDataPackageDropMode(bool enable);

  //! Set PackageDrop for a specific instance of a plugin
  bool SetDataPackageDropMode(const std::string &plugin_name, const std::string &source_name,
                              const std::string &instance_name, bool enable);

  //! returns a list of strings of the required input urls of a named plugin
  const std::list<std::string> &requiredInputUrls(const std::string &plugin_name) const;

  //! returns a list of filenames (strings) of the output data as flatbuffers
  const std::list<std::string> &providedOutputFlatbuffers(const std::string &plugin_name) const;

  //! returns the name of the plugin which can provide the requested output data
  const std::string &dataCanBeProvidedFlatbuffers(const std::string &filename) const;

  //! Add or remove configs for the plugins in a specified source
  void addAndRemoveConfigs(const std::vector<bridgesdk::plugin_config_t> &config, std::string plugin_name,
                           std::string source_name);

private:
  void scanDirectory(const std::string &path, int depth = 0);

  static bool readDescription(plugin_information &pi);

  //! check if player state requires a reset of the plugin
  bool checkForPluginReset(const next::control::events::PlayerMetaInformationMessage &message);

  //! call reset on all available plugin from plugin list
  void resetAllPlugins();

  bool getPluginInstancesOfSource(const std::string &plugin_name, const std::string &source_name,
                                  map_instances_per_source &source_instances);

  std::shared_ptr<next::bridgesdk::plugin::Plugin>
  getPluginInstance(const std::string &plugin_name, const std::string &source_name, const std::string &instance_name);
  std::shared_ptr<next::bridgesdk::plugin::Plugin> createPluginInstance(plugin_information &plugin_info);

  void notifyReset();

  //! plugins_ holds the available plugins (keys) and their information (values)
  std::map<std::string, plugin_information> plugins_;

  std::shared_ptr<next::databridge::data_manager::IDataManager> output_data_manager_{nullptr};
  // std::shared_ptr<next::databridge::input_port_controller::IInputPortController> input_port_controller_;
  std::shared_ptr<next::udex::subscriber::DataSubscriber> data_subscriber_{nullptr};
  std::shared_ptr<next::udex::explorer::SignalExplorer> signal_explorer_{nullptr};
  std::shared_ptr<next::databridge::plugin_util::ProcessManager> process_manager_{nullptr};
  std::shared_ptr<next::databridge::plugin_util::PluginConfigHandler> plugin_config_handler_{nullptr};
  std::unique_ptr<next::control::events::PlayerStateEvent> player_event_{nullptr};

  bool current_gui_sync_option_{false};
  bool current_gui_drop_option_{false};

  std::mutex plugin_reset_{};

  std::thread reset_monitoring_thread_;
  std::condition_variable reset_monitoring_cv_;
  std::mutex reset_monitoring_mutex_;
  std::atomic_bool stop_reset_monitoring_thread_{false};
  std::atomic_int64_t reset_call_time_;
  std::atomic_int should_reset_counter_{0};
  next::sdk::events::StatusCodeEvent status_code_event_;
  int resetWaitPeriodMs_{1000};
};
} // namespace plugin_manager
} // namespace databridge
} // namespace next

#endif // NEXT_DATABRIDGE_PLUGIN_MANAGER_H_

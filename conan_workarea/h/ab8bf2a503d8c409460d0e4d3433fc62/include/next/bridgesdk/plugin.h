/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin.h
 * @brief    interface for birdge plugins
 *
 * A plugin provides the functionlality of data collection, converting
 * and forwarding. This functionality is called a bridge. This header is
 * the interface of such a bridge.
 *
 **/

#ifndef NEXT_BRIDGESDK_PLUGIN_H_
#define NEXT_BRIDGESDK_PLUGIN_H_

#if defined(_WIN32) && defined(BRIDGESDKSHARED)
#define NEXTPLUGINSHARED_EXPORT __declspec(dllexport)
#else
#define NEXTPLUGINSHARED_EXPORT
#endif

#include <memory>
#include <string>
#include <vector>

#include <next/sdk/sdk_macros.h>
#include <next/sdk/logger/logger.hpp>

#include <next/bridgesdk/datatypes.h>

#include "plugin_config.h"

namespace next {

namespace udex {
namespace explorer {
class SignalExplorer;
} // namespace explorer
namespace subscriber {
class DataSubscriber;
} // namespace subscriber
} // namespace udex

namespace databridge {

namespace data_manager {
//! forward declaration of internal structures of data bridge
class IDataManager;
} // namespace data_manager
} // namespace databridge
} // namespace next

namespace next::plugins_utils {
struct SensorConfig;
}

namespace next {
namespace bridgesdk {
namespace plugin_addons {
class PluginDataPublisherImpl;
class PluginDataSubscriberImpl;
class Plugin3dViewImpl;
class PluginSignalExplorerImpl;
class PluginSignalExtractorImpl;
} // namespace plugin_addons
} // namespace bridgesdk
} // namespace next

NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS

namespace next {
namespace bridgesdk {
namespace plugin {

class PluginPrivate;

/*!
 *  This is the interface of a plugin. It provides several interface and
 *  helper functions.
 *
 *  A plugin is shipped along with a config file. This configuration file
 *  contains the information for the plugin manager which input data is
 *  consumed by the plugin and which output data is provided. It can also
 *  contain some configuration information (optional).
 *
 *  After creating a instance of the plugin the init function is called.
 *  Afterwards the plugin manger registers the data receiving functions in the
 *  plugin and then the data providing functions.
 *  Note: Since a plugin can provide data to another plugin, it is not defined
 *  whether the plugin output functions or the input functions are first
 *  registered. The plugin MUST be able to handle a random registration of the
 *  input and output functions.
 *  It can also happen that during runtime new plugins are loaded or unloaded,
 *  such that data provider or consumer are added or removed during runtime.
 */
class DECLSPEC_plugin_config Plugin {

public:
  //! friend classes used for addons
  friend class next::bridgesdk::plugin_addons::PluginDataPublisherImpl;
  friend class next::bridgesdk::plugin_addons::PluginDataSubscriberImpl;
  friend class next::bridgesdk::plugin_addons::Plugin3dViewImpl;
  friend class next::bridgesdk::plugin_addons::PluginSignalExplorerImpl;
  friend class next::bridgesdk::plugin_addons::PluginSignalExtractorImpl;

  // ######################################## BASE ################################
  Plugin(const char *path);
  Plugin() = delete;
  Plugin(const Plugin &other) = delete;
  Plugin &operator=(const Plugin &other) = delete;
  Plugin(Plugin &&other) = delete;
  Plugin &operator=(Plugin &&other) = delete;

  virtual ~Plugin();

  //! Get the name of the plugin instance
  std::string getName() const;

  //! Set the plugin instance name
  void setName(const std::string &name);

  //! Get the version of the plugin.
  /*!
   *  The version is in format <major>.<minor>[.patch][-<additionalText>]
   */
  std::string getVersion() const;

  //! Set the version of the plugin.
  /*!
   *  The given version MUST be in format of <major>.<minor>[.patch][-<additionalText>].
   *  Currently the function is not checking the format, this will be added in future.
   */
  void setVersion(const std::string &version);

  //! Init the plugin
  /*!
   *  This function shall be used by the user to init the internal
   *  functionality of the plugin.
   *
   *  On error appropriate exceptions are thrown (e.g. if a special config
   *  file is not found)
   *
   *  The base implementation does nothing.
   */
  virtual bool init();

  //! Update the plugin with user input via strings
  /*!
   * This function is called if new events take place in the GUI regarding the plugin
   * the event parameters are transferred into the list of strings
   * @param user_input vector of the input strings from the GUI events
   * @return
   */

  virtual bool update(std::vector<std::string> user_input);

  //! Reset the internal state
  /*!
   *  This function is called to reset the internal state, e.g. to restart
   *  a simulation. The purpos is to clear internal buffers and reset variables
   *  to default values.
   *
   *  Note: All provided input MUST be ignored until exitReset() is called!
   *
   *  The base implementation does nothing
   */
  virtual bool enterReset();

  //! Finish the reset phase
  /*!
   *  This function is called by the plugin manager to signal that resetting
   *  all of the plugins finished. From now on the provided input data shall be
   *  processed and output data can be provided.
   *
   *  The base implementation does nothing.
   */
  virtual bool exitReset();

  //! Get possible configuration description
  virtual bool getDescription(next::bridgesdk::plugin_config_t &config_fields);

  //! Set the config send by GUI to be used in plugin's code to subscribe to topic or extracting data from payload.
  virtual bool addConfig(const next::bridgesdk::plugin_config_t &config);

  //! Remove current active config used in plugin.
  virtual bool removeConfig(const next::bridgesdk::plugin_config_t &config);

  //! Set Synchronization mode
  /**
   * Use this to enable the synchronization of this plugin.
   * !! CAUTION !! this will drastically impact the simulation performance if enabled
   * @param enabled true if synchronization should be enabled
   * @return true if successful, failed if Mode could not be set
   */
  bool setSynchronizationMode(bool enabled);

  //! Set Package Drop Mode
  /**
   * Use this to enable package losses for this plugin.
   * !! CAUTION !! this will drastically impact the simulation performance if enabled
   * !! NOT ALL DATA WILL BE PROCESSED IF ENABLED !!
   * Packages will be dropped within UDex to ensure a fast simulation performance. If a callback has not yet finished,
   * but a new data package is received, it will be dropped.
   * @param enabled
   * @return true if successful, failed if Mode could not be set
   */
  bool setPackageDropMode(bool enabled);

  //! Connect the plugin to databridge
  /*!
   * Internally used function to connect the user plugin to the backend
   * @param input_port_controller pointer to input port controller to be connected
   * @param data_manager pointer to broadcaster to be used later
   */
  virtual void connectPlugin(std::shared_ptr<next::udex::subscriber::DataSubscriber> &data_subscriber,
                             std::shared_ptr<next::databridge::data_manager::IDataManager> &data_manager,
                             std::shared_ptr<next::udex::explorer::SignalExplorer> &signal_explorer);

private:
  std::unique_ptr<next::bridgesdk::plugin::PluginPrivate> impl_;
};

NEXT_RESTORE_WARNINGS_WINDOWS

} // namespace plugin
} // namespace bridgesdk
} // namespace next

typedef next::bridgesdk::plugin::Plugin *plugin_create_t;

#endif // NEXT_BRIDGESDK_PLUGIN_H_

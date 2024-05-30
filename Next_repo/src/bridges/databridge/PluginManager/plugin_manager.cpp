/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_manager.cpp
 * @brief    Helper to manage all the plugins.
 *
 * See plugin_manager.h
 *
 **/

#include "plugin_manager.h"

#include <chrono>
#include <functional>

#include <boost/dll/import.hpp>
#include <boost/filesystem.hpp>
#include <boost/function.hpp>

#include <json/json.h>

#include <next/sdk/sdk_macros.h>
#include <next/sdk/logger/logger.hpp>

#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include <next/appsupport/next_status_code/next_status_code.hpp>

#include "OutputSchema/output_schema_checker.h"

#ifdef _WIN32
#include <Windows.h>
#include "libloaderapi.h"
#endif

namespace next {
namespace databridge {
namespace plugin_manager {

PluginManager::PluginManager() {

  next::appsupport::ConfigClient *config = next::appsupport::ConfigClient::getConfig();
  if (config) {
    resetWaitPeriodMs_ = config->get_int(next::appsupport::configkey::getPluginManagerResetPeriodWait("GENERIC"), 1000);
  }
  reset_monitoring_thread_ = std::thread([this]() {
    while (!stop_reset_monitoring_thread_) {
      std::unique_lock lk(reset_monitoring_mutex_);
      reset_monitoring_cv_.wait(lk, [this]() { return (should_reset_counter_ > 0 || stop_reset_monitoring_thread_); });
      should_reset_counter_--;
      if (stop_reset_monitoring_thread_) {
        break;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(resetWaitPeriodMs_ + 200));
      int64_t now =
          std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
              .count();
      if (now - reset_call_time_ > resetWaitPeriodMs_) {
        resetAllPlugins();
      }
    }
  });

  status_code_event_.addEventHook([this]() {
    sdk::events::StatusCodeMessage msg = status_code_event_.getEventData();
    // once the origin is set correctly to SIM_NODE we can test for it
    int expected_status_code = (int)next::sdk::events::StatusOrigin::SIM_NODE * 1000 +
                               (int)next::appsupport::StatusCode::SUCC_COMP_CONFIG_OUTPUT_FINISHED;
    if (msg.toInt() == expected_status_code) {
      debug(__FILE__, "{0} status code received, trigger plugins reset.", msg.toInt());
      notifyReset();
    }
  });
  status_code_event_.subscribe();
}

PluginManager::~PluginManager() {
  stop_reset_monitoring_thread_ = true;
  reset_monitoring_cv_.notify_one();
  if (reset_monitoring_thread_.joinable()) {
    reset_monitoring_thread_.join();
  }
}

bool PluginManager::init(const std::string &path) noexcept {
  try {
    scanDirectory(path, 2);
  } catch (std::exception const &e) {
    error(__FILE__, "failed to scan directory '{0}': {1}", path, e.what());
    return false;
  }
  debug(__FILE__, "found {0} plugins", plugins_.size());

  try {
    data_subscriber_ = std::make_shared<next::udex::subscriber::DataSubscriber>("Next-DataBridge-EcalNode");
  } catch (std::exception const &e) {
    error(__FILE__, "Instantiating the data subscriber failed: {0}", e.what());
    return false;
  }
  try {
    signal_explorer_ = std::make_shared<next::udex::explorer::SignalExplorer>();
    signal_explorer_->EnableFastSearch();
  } catch (std::exception const &e) {
    error(__FILE__, "Failed to instantiate the signal explorer: {0}", e.what());
    return false;
  }

  try {
    process_manager_ = std::make_shared<next::databridge::plugin_util::ProcessManager>();
  } catch (std::exception const &e) {
    error(__FILE__, "Failed to instantiate the process manager: {0}", e.what());
    return false;
  }

  try {
    plugin_config_handler_ = std::make_shared<next::databridge::plugin_util::PluginConfigHandler>();
  } catch (std::exception const &e) {
    error(__FILE__, "Failed to instantiate the plugin config handler: {}", e.what());
    return false;
  }

  try {
    player_event_ = std::make_unique<next::control::events::PlayerStateEvent>("");
    auto pluginResetCallback = [this]() {
      next::control::events::PlayerMetaInformationMessage message = player_event_->getEventData();
      if (checkForPluginReset(message)) {
        notifyReset();
      }
    };
    player_event_->addEventHook(pluginResetCallback);
    player_event_->subscribe();
  } catch (std::exception const &e) {
    error(__FILE__, "Failed to instantiate the player state event: {}", e.what());
    return false;
  }

  return true;
}

void PluginManager::scanDirectory(const std::string &path, int depth) {
  if (-1 > depth)
    return;
  debug(__FILE__, "  scanning folder {}", path);
  if (boost::filesystem::exists(path) && boost::filesystem::is_directory(path)) {
    auto pos = path.find_last_of("\\");
    std::string plugin_name;
    if (pos != std::string::npos) {
      plugin_name = path.substr(pos + 1, path.length());
    } else {
      pos = path.find_last_of("/");
      if (pos != std::string::npos) {
        plugin_name = path.substr(pos + 1, path.length());
      } else
        plugin_name = path;
    }

    for (boost::filesystem::directory_entry &entry : boost::filesystem::directory_iterator(path)) {
      debug(__FILE__, "   checking {}", entry.path().string());
      if (boost::filesystem::is_regular_file(entry)) {
        if (boost::filesystem::extension(entry.path()) == kSharedLibFileExtension) {
          debug(__FILE__, "   found library file: {}", entry.path().string());
          try {
#ifdef _WIN32
            // This is to prevent error mode dialogs from hanging the application.
            SetErrorMode(0x0001);

            // use windows lib loader flags to fill boost dllLoadMode
            // boost doesn't define them, so use Windows definitions
            // see libloaderapi.h from Windows API
            boost::dll::load_mode::type dllLoadMode =
                boost::dll::load_mode::type(LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
#else
            boost::dll::load_mode::type dllLoadMode = boost::dll::load_mode::default_mode;
#endif

            boost::dll::shared_library lib(entry.path(), dllLoadMode);

#ifdef _WIN32
            // Set Error Mode to default
            SetErrorMode(0);
#endif
            if (!lib.has("create_plugin")) {
              debug(__FILE__, " {} is not a compatible plugin", entry.path().string());
              continue;
            } else {
              if (plugins_.count(plugin_name)) {
                warn(__FILE__, "    A plugin with the name {0} was already added. Plugin is ignored.", plugin_name);
              } else {
                debug(__FILE__, "  {0} seems to be compatible, adding as {1}", entry.path().string(), plugin_name);
                boost::function<plugin_constructor_t> creator =
                    boost::dll::import_symbol<plugin_constructor_t>(boost::move(lib), "create_plugin");
                plugins_[plugin_name].creator = creator;
                auto abs_path = boost::filesystem::absolute(path).string();
                debug(__FILE__, "  absolute path: {}", abs_path);
                plugins_[plugin_name].path = abs_path;
                readDescription(plugins_[plugin_name]);
              }
            }
          } catch (const boost::system::system_error &e) {
            warn(__FILE__, "Could not load plugin {0}; Reason [{1}]. Plugin is ignored.", plugin_name, e.what());
            continue;
          }
        } else {
          debug(__FILE__, "  ignoring file: {}", entry.path().string());
        }
      } else if (boost::filesystem::is_directory(entry) && 0 <= depth) {
        debug(__FILE__, "  looking into directory", entry.path().string());
        scanDirectory(entry.path().string(), depth - 1);
      }
    }
  }
}

bool PluginManager::connectToOutputDataManager(
    std::shared_ptr<next::databridge::data_manager::IDataManager> data_output_manager) {
  output_data_manager_ = data_output_manager;
  return true;
}

bool PluginManager::isAvailable(const std::string &plugin_name) const {
  if (plugins_.find(plugin_name) != plugins_.end()) {
    return true;
  } else {
    return false;
  }
}

std::shared_ptr<next::bridgesdk::plugin::Plugin> PluginManager::createPluginInstance(plugin_information &plugin_info) {
  std::shared_ptr<next::bridgesdk::plugin::Plugin> plugin = nullptr;
  if (!data_subscriber_) {
    error(__FILE__, "Data subscriber is not set.");
    return plugin;
  }

  if (!output_data_manager_) {
    error(__FILE__, "Output data manager is not set.");
    return plugin;
  }

  if (!signal_explorer_) {
    error(__FILE__, "Signal explorer is not set.");
    return plugin;
  }

  plugin = std::shared_ptr<next::bridgesdk::plugin::Plugin>(plugin_info.creator(plugin_info.path.c_str()));
  plugin->connectPlugin(data_subscriber_, this->output_data_manager_, signal_explorer_);
  return plugin;
}

bool PluginManager::addPluginInstance(const std::string &plugin_name, const std::string &source_name,
                                      const std::string &instance_name) {

  plugin_reset_.lock();
  // make sure that plugin exist
  auto it_plugin = plugins_.find(plugin_name);
  if (it_plugin == plugins_.end()) {
    warn(__FILE__, "Plugin {0} not found to create instance. ", plugin_name);
    plugin_reset_.unlock();
    return false;
  }
  plugin_information &plugin_info = it_plugin->second;
  plugin_reset_.unlock();

  auto it_source_instances = plugin_info.instances.find(source_name);
  // add empty map for plugin instance of this source if not existing
  if (it_source_instances == plugin_info.instances.end()) {
    map_instances_per_source empty_instance_map{};
    debug(__FILE__, "New list to store plugin instances created for source {0} of plugin {1}.", source_name,
          plugin_name);

    auto result = plugin_info.instances.emplace(source_name, empty_instance_map);
    it_source_instances = result.first;
  }

  auto &list_of_instances_for_source = it_source_instances->second;
  auto it_instance = list_of_instances_for_source.find(instance_name);
  if (it_instance != list_of_instances_for_source.end()) {
    debug(__FILE__, "Instance {0} for source {1} of plugin {2} exist already. No new instance created.", instance_name,
          source_name, plugin_name);
    return true;
  }
  std::shared_ptr<next::bridgesdk::plugin::Plugin> plugin_instance = createPluginInstance(plugin_info);
  debug(__FILE__, "New plugin instance {0} created for source {1} of plugin {2}.", instance_name, source_name,
        plugin_name);
  if (plugin_instance == nullptr) {
    return false;
  }
  list_of_instances_for_source.insert_or_assign(instance_name, plugin_instance);
  return true;
}

bool PluginManager::destroyPlugin(const std::string &plugin_name) {
  debug(__FILE__, "Try to destroy plugin {0}", plugin_name);
  bool plugin_in_process = true;
  int counter = 0;
  int maxTries = 12;
  // wait till plugin processing is finished
  while (plugin_in_process && counter < maxTries) {
    debug(__FILE__, " Check if plugin {0} is in process. Tries: {1}/{2}", plugin_name, std::to_string(counter + 1),
          std::to_string(maxTries));
    if (counter > 0) {
      debug(__FILE__, " Wait before checking again if plugin {0} is in process.", plugin_name);
      std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    }

    plugin_in_process = process_manager_->isProcessActive(plugin_name);
    ++counter;
  }

  if (!plugin_in_process) {
    std::lock_guard<std::mutex> lck{plugin_reset_};

    auto it = plugins_.find(plugin_name);
    if (plugins_.end() != it) {
      it->second.instances.clear();
      plugins_.erase(it);
      return true;
    } else {
      warn(__FILE__, "could not find plugin {0} to destroy. ", plugin_name);
      return false;
    }
  } else {
    warn(__FILE__, "Plugin {0} is still in process and can't be destroyed.", plugin_name);
    return false;
  }
}

void PluginManager::shutdown() {
  auto tmpPlugins = plugins_;
  for (auto p : tmpPlugins) {
    destroyPlugin(p.first);
  }
}

bool PluginManager::getPluginInstancesOfSource(const std::string &plugin_name, const std::string &source_name,
                                               map_instances_per_source &source_instances) {
  source_instances = {};
  std::lock_guard<std::mutex> lck{plugin_reset_};
  auto it_plugin = plugins_.find(plugin_name);
  if (plugins_.end() != it_plugin) {
    const auto it_source_instances = it_plugin->second.instances.find(source_name);
    if (it_plugin->second.instances.end() != it_source_instances) {
      source_instances = it_source_instances->second;
      return true;
    }
  }
  return false;
}

std::shared_ptr<next::bridgesdk::plugin::Plugin> PluginManager::getPluginInstance(const std::string &plugin_name,
                                                                                  const std::string &source_name,
                                                                                  const std::string &instance_name) {
  map_instances_per_source source_instances;
  if (getPluginInstancesOfSource(plugin_name, source_name, source_instances)) {
    const auto it_instance = source_instances.find(instance_name);
    if (source_instances.end() != it_instance) {
      return it_instance->second;
    }
  }
  return nullptr;
}

bool PluginManager::initPlugin(const std::string &plugin_name, const std::string &source_name,
                               const std::string &instance_name) {
  auto plugin = getPluginInstance(plugin_name, source_name, instance_name);
  if (nullptr == plugin) {
    warn(__FILE__, "No instance {0} for source {1} of plugin {2} found to initialize plugin.", instance_name,
         source_name, plugin_name);
    return false;
  }
  return plugin->init();
}

bool PluginManager::updatePlugin(const std::string &plugin_name, const std::string &source_name,
                                 const std::string &instance_name, std::vector<std::string> user_arg) {
  auto plugin = getPluginInstance(plugin_name, source_name, instance_name);
  if (nullptr == plugin) {
    warn(__FILE__, "No instance {0} for source {1} of plugin {2} found to update plugin.", instance_name, source_name,
         plugin_name);
    return false;
  }
  bool ret = plugin->update(user_arg);
  if (!ret) {
    warn(__FILE__, "Update plugin failed for instance {0} for source {1} of plugin {2} found to update plugin.",
         instance_name, source_name, plugin_name);
    return ret;
  }
  return ret;
}

bool PluginManager::SetSynchronizationMode(bool enable) {
  bool success = true;
  current_gui_sync_option_ = enable;
  for (auto &p : plugins_) {
    for (auto &source : p.second.instances) {
      for (auto &plugin : source.second) {
        success &= plugin.second->setSynchronizationMode(enable);
      }
    }
  }
  return success;
}

bool PluginManager::SetSynchronizationMode(const std::string &plugin_name, const std::string &source_name,
                                           const std::string &instance_name, bool enable) {
  auto plugin = getPluginInstance(plugin_name, source_name, instance_name);
  if (nullptr == plugin) {
    error(__FILE__, "No instance {0} for source {1} of plugin {2} found to SetSynchronizationMode of plugin.",
          instance_name, source_name, plugin_name);
    return false;
  }
  return plugin->setSynchronizationMode(enable);
}

bool PluginManager::SetDataPackageDropMode(bool enable) {
  bool success = true;
  current_gui_drop_option_ = enable;
  for (auto &p : plugins_) {
    for (auto &source : p.second.instances) {
      for (auto &plugin : source.second) {
        success &= plugin.second->setPackageDropMode(enable);
      }
    }
  }
  return success;
}

bool PluginManager::SetDataPackageDropMode(const std::string &plugin_name, const std::string &source_name,
                                           const std::string &instance_name, bool enable) {
  auto plugin = getPluginInstance(plugin_name, source_name, instance_name);
  if (nullptr == plugin) {
    error(__FILE__, "No instance {0} for source {1} of plugin {2} found to setPackageDropMode of plugin.",
          instance_name, source_name, plugin_name);
    return false;
  }
  return plugin->setPackageDropMode(enable);
}

bool PluginManager::getPluginVersion(const std::string &plugin_name, const std::string &source_name,
                                     const std::string &instance_name, std::string &plugin_version) {
  plugin_version = "";
  auto plugin = getPluginInstance(plugin_name, source_name, instance_name);
  if (nullptr == plugin) {
    error(__FILE__, "No instance {0} for source {1} of plugin {2} found to setPackageDropMode of plugin.",
          instance_name, source_name, plugin_name);
    return false;
  }
  plugin_version = plugin->getVersion();
  return true;
}

void PluginManager::getCompatiblePlugins(const std::string data_class, const std::string version,
                                         std::list<std::string> &compatible_plugins) {
  next::databridge::plugin_manager::OutputSchemaChecker fbs_checker;
  // first call output check and receive list of possible plugins
  for (auto plugin_temp : plugins_) {
    for (auto fbs_file : plugin_temp.second.output_flatbuffers) {
      if (fbs_checker.CheckFlatbufferSchema(fbs_file, data_class, version)) {
        compatible_plugins.push_back(plugin_temp.first);
        debug(__FILE__, " plugin {0} compatible to flatbuffer {1}", plugin_temp.first, fbs_file);
        break;
      } else {
        debug(__FILE__, " plugin {0} not compatible to flatbuffer {1}", plugin_temp.first, fbs_file);
      }
    }
  }
}

bool PluginManager::updatePluginConfigs(const std::string data_class, const std::string version,
                                        std::string source_name, const std::vector<std::string> user_parameter) {

  bool is_success = true;
  debug(__FILE__, "checking for {0} version {1}", data_class, version);
  std::list<std::string> compatible_plugins;
  getCompatiblePlugins(data_class, version, compatible_plugins);
  if (compatible_plugins.empty()) {
    warn(__FILE__, "No compatible plugins found to update data class {0}", data_class);
    return false;
  }

  for (auto plugin_name : compatible_plugins) {
    map_instances_per_source source_instances;
    bool found_plugin_instances = getPluginInstancesOfSource(plugin_name, source_name, source_instances);
    if (!found_plugin_instances || source_instances.empty()) {
      debug(__FILE__, "starting up new Plugin {0}", plugin_name);
      // Use 'Update_Instance' as instance name for the update config if no other instance exists of this plugin
      std::string instance_name = "Update_Instance";
      if (addPluginInstance(plugin_name, source_name, instance_name)) {
        if (!initPlugin(plugin_name, source_name, instance_name)) {
          warn(__FILE__, "Initializing Plugin {0} of source {1} for instance {2} failed", plugin_name, source_name,
               instance_name);
          continue;
        }
        // !!! only intermediate solution to hand over plugin instance name to the plugin by calling addConfig!!!
        // CAUTION: addConfig isn't considered by plugin_config_handler. Therefore Plugins which only need update(...)
        // function shall not add any content in addConfig which needs to be cleaned up because removeConfig will never
        // be called for them!!!
        // TODO Refactor GUI so that always addConfig is called to setup the plugin before the first time the upate()
        // function of a plugin is called. That makes sure that the plugin configuration is dealt with by
        // plugin_config_handler
        auto plugin = getPluginInstance(plugin_name, source_name, instance_name);
        if (plugin != nullptr) {
          std::string plugin_instance_name = plugin_name + "::" + source_name;
          bridgesdk::plugin_config_t config_value;
          config_value.insert({"DATA_VIEW", {bridgesdk::ConfigType::FIXED, plugin_instance_name}});
          // don't check return value of addConfig becaues return value will be false for plugins which haven't
          // overwritten this function
          plugin->addConfig(config_value);
        }
        if (!SetSynchronizationMode(plugin_name, source_name, instance_name, current_gui_sync_option_)) {
          warn(__FILE__, "Setting Synchronization Mode for Plugin {0} of source {1} for instance {2} failed",
               plugin_name, source_name, instance_name);
        }
        if (!SetDataPackageDropMode(plugin_name, source_name, instance_name, current_gui_drop_option_)) {
          warn(__FILE__, "Setting PackageDrop Mode for Plugin {0} of source {1} for instance {2} failed", plugin_name,
               source_name, instance_name);
        }
        found_plugin_instances = getPluginInstancesOfSource(plugin_name, source_name, source_instances);
      } else {
        error(__FILE__, "failed to create an instance of {0} for source {1}", plugin_name, source_name);
      }
    }
    debug(__FILE__, "updating Plugin {0}", plugin_name);

    if (found_plugin_instances && !source_instances.empty()) {
      for (auto &instance : source_instances) {
        if (instance.second == nullptr) {
          warn(__FILE__, "Instance not available for {0} of {1}.", source_name, plugin_name);
          continue;
        }
        std::string process_name = plugin_name + "::" + source_name;
        if (source_name.empty()) {
          warn(__FILE__, "source name is empty {0} for the plugin", plugin_name);
        }
        is_success &= process_manager_->invokeCall(
            std::bind(&PluginManager::updatePlugin, this, plugin_name, source_name, instance.first, user_parameter),
            process_name);
      }
    } else {
      warn(__FILE__, "failed to update Plugin {0} of source {1}. Missing instances", plugin_name, source_name);
      return false;
    }
  }
  return is_success;
}

bool PluginManager::findAndSendDescription(const std::string data_class, const std::string version) {

  debug(__FILE__, "checking for {0} version {1}", data_class, version);
  if (!output_data_manager_) {
    throw std::runtime_error("Output data manager is not set");
  }
  // first call output check and receive list of possible plugins
  std::list<std::string> compatible_plugins;
  getCompatiblePlugins(data_class, version, compatible_plugins);
  bool success = true;
  for (auto plugin_name_temp : compatible_plugins) {
    auto it = plugins_.find(plugin_name_temp);
    if (it != plugins_.end()) {
      // Create the plugin instance to get the plugin description. Plugin instance is destroyed afterwards.
      std::shared_ptr<next::bridgesdk::plugin::Plugin> plugin_instance = createPluginInstance(it->second);

      if (plugin_instance == nullptr) {
        success = false;
        continue;
      }
      success = plugin_instance->init();
      if (!success) {
        continue;
      }
      next::bridgesdk::plugin_config_t description;
      plugin_instance->getDescription(description);
      output_data_manager_->PublishPluginDescriptionToClient(data_class, plugin_name_temp, description);
      debug(__FILE__, "plugin description of plugin {0} published ", plugin_name_temp);

    } else {
      success = false;
      warn(__FILE__, "plugin {0} not found to get the description", plugin_name_temp);
    }
  }
  return success;
}

void PluginManager::addAndRemoveConfigs(const std::vector<bridgesdk::plugin_config_t> &config, std::string plugin_name,
                                        std::string source_name) {
  plugin_reset_.lock();
  auto it_plugin_info = plugins_.find(plugin_name); // plugin_information
  plugin_reset_.unlock();
  if (it_plugin_info == plugins_.end()) {
    error(__FILE__, "Plugin info is not available for the plugin {0}", plugin_name);
    return;
  }
  plugin_util::PluginConfigUpdates plugin_config_updates;
  // Get configs to add and remove
  plugin_config_handler_->updateConfigs(config, plugin_name, source_name, plugin_config_updates);
  // Remove the configs to be removed
  if (!plugin_config_updates.configs_to_remove.empty()) {
    auto it_instance_src = it_plugin_info->second.instances.find(source_name);
    if (it_instance_src == it_plugin_info->second.instances.end()) {
      warn(__FILE__, "Instance not available for {0} of {1}.No instances are removed.", source_name, plugin_name);

    } else {
      for (const auto &[key, config_value] : plugin_config_updates.configs_to_remove) {
        auto it_plugin_instance = it_instance_src->second.find(key);
        if (it_plugin_instance == it_instance_src->second.end()) {
          warn(__FILE__, "Plugin instance {0} doesn't exist for {1} of {2}. No instance removed.", key, source_name,
               plugin_name);
          continue;
        }

        std::shared_ptr<next::bridgesdk::plugin::Plugin> plugin_pointer = it_plugin_instance->second;
        if (plugin_pointer == nullptr) {
          warn(__FILE__, "Plugin instance {0} is nullptr for {1} of {2}. No instance removed.", key, source_name,
               plugin_name);
        } else {
          plugin_pointer->removeConfig(config_value);
        }
        it_instance_src->second.erase(it_plugin_instance);
      }
    }
  }
  for (const auto &[key, config_value] : plugin_config_updates.configs_to_add) {
    bool addpluggin_success = addPluginInstance(plugin_name, source_name, key);
    bool initplugin_success = false;
    if (addpluggin_success) {
      initplugin_success = initPlugin(plugin_name, source_name, key);
    }

    if (!addpluggin_success || !initplugin_success) {
      error(__FILE__, "failed to create/init an instance of {0} {1} {2}", key, source_name, plugin_name);
      plugin_config_handler_->removeConfig(plugin_name, source_name, key);

      try {
        plugins_.at(plugin_name).instances.at(source_name).erase(key);
      } catch (...) {
        warn(__FILE__, "Instance of {0} {1} {2} not available for removal after create/init failed", key, source_name,
             plugin_name);
      }
    }
    auto plugin = getPluginInstance(plugin_name, source_name, key);
    if (plugin != nullptr) {
      if (!plugin->addConfig(config_value)) {
        // Don't remove config if adding wasn't succesfull.
        // removeConfig() function in the plugins has to be able to handle it, if addConfig failed before.
        // Config adding has to be retriggered via resetAllPlugins() by enterReset/exitReset
        warn(__FILE__, "Could not add the config for the pluging {0}", plugin_name);
      }

    } else {
      error(__FILE__, "addRemoveConfigs failed to get instance of plugin: {0}", plugin_name);
    }
  }

  SetSynchronizationMode(current_gui_sync_option_);
  SetDataPackageDropMode(current_gui_drop_option_);
}

bool PluginManager::findAndSetConfig(const std::vector<bridgesdk::plugin_config_t> &config, std::string plugin_name,
                                     std::string source_name) {
  bool is_success = true;
  std::string process_name = plugin_name + "::" + source_name;
  if (source_name.empty()) {
    warn(__FILE__, "source name is empty {0} for the plugin", plugin_name);
  }
  is_success &= process_manager_->invokeCall(
      std::bind(&PluginManager::addAndRemoveConfigs, this, config, plugin_name, source_name), process_name);
  return is_success;
}

const std::list<std::string> &PluginManager::requiredInputUrls(const std::string &plugin_name) const {
  static std::list<std::string> empty_list;
  auto it = plugins_.find(plugin_name);
  if (plugins_.end() != it) {
    return it->second.input_urls;
  }
  return empty_list;
}

const std::list<std::string> &PluginManager::providedOutputFlatbuffers(const std::string &plugin_name) const {
  static std::list<std::string> empty_list;
  auto it = plugins_.find(plugin_name);
  if (plugins_.end() != it) {
    return it->second.output_flatbuffers;
  }
  return empty_list;
}

const std::string &PluginManager::dataCanBeProvidedFlatbuffers(const std::string &filename) const {
  static std::string empty_string;
  for (auto &p : this->plugins_) {
    for (auto &f : p.second.output_flatbuffers) {
      if (filename == f) {
        return p.first;
      }
    }
  }
  return empty_string;
}

bool PluginManager::readDescription(plugin_information &p) {

  for (boost::filesystem::directory_entry &entry : boost::filesystem::directory_iterator(p.path)) {
    if (boost::filesystem::is_regular_file(entry)) {
      const std::string extension = entry.path().extension().string();
      if (".fbs" == extension) {
        debug(__FILE__, "Found .fbs {0}", entry.path().string());

        p.output_flatbuffers.push_back(entry.path().string());
      }
    }
  }
  return true;
}

bool PluginManager::checkForPluginReset(const next::control::events::PlayerMetaInformationMessage &message) {
  switch (message.status) {
  case next::control::events::PlayerState::EXIT_OPENING:
    return true;
  case next::control::events::PlayerState::EXIT_SKIP_TO:
    if (message.min_timestamp == message.timestamp) {
      return true;
    } else {
      return false;
    }
    break;
  default:
    return false;
  }
}

void PluginManager::notifyReset() {
  reset_call_time_ =
      std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
          .count();
  should_reset_counter_++;
  reset_monitoring_cv_.notify_one();
}

void PluginManager::resetAllPlugins() {
  // can be called by asynchron by player event callback
  // lock all cases where plugins_ is manipulated in other code parts
  std::lock_guard<std::mutex> lck{plugin_reset_};

  info(__FILE__, "Resetting plugins...");

  for (auto plugin : plugins_) {
    if (!plugin.second.instances.empty()) {
      for (auto src_instance : plugin.second.instances) {
        for (auto instance : src_instance.second) {
          if (instance.second) {
            std::string process_name = plugin.first + "::" + src_instance.first;
            auto pluginResetCalls = [](std::shared_ptr<next::bridgesdk::plugin::Plugin> instance_ptr) {
              instance_ptr->enterReset();
              instance_ptr->exitReset();
            };
            process_manager_->invokeCall(std::bind(pluginResetCalls, instance.second), process_name);
          }
        }
      }
    }
  }
}

std::list<std::string> PluginManager::getAvailablePlugins() const {
  std::list<std::string> tmp_list;
  for (auto &p : plugins_) {
    tmp_list.push_back(p.first);
  }
  return tmp_list;
}

} // namespace plugin_manager
} // namespace databridge
} // namespace next

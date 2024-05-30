/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     config_client.hpp
 * @brief    Singleton class which extends Config_Base to use ptree functionalities
 *
 * Include this file and derive from the class, if you want to use the Next configuration handling in a component.
 *
 **/

#ifndef CONFIG_CLIENT_H
#define CONFIG_CLIENT_H

#include "config_base.hpp"

#include <condition_variable>
#include <thread>

#include <next/sdk/events/request_config_from_client.h>
#include <next/sdk/events/request_config_from_server.h>
#include <next/sdk/events/request_config_value_from_server.h>
#include <next/sdk/events/send_config_tags_to_server.h>
#include <next/sdk/events/send_config_to_client.h>
#include <next/sdk/events/send_config_to_server.h>
#include <next/sdk/events/send_config_value_to_client.h>
#include <next/sdk/sdk_macros.h>

namespace next {
namespace appsupport {

/*!
 * @brief  Configuration Client class
 * @brief Implementing the singleton to be used in the components to store and retreive the config.
 *
 * The configuration handling is providing a space to store all the information, needed by the application,
 * to run. After initialization, the config handler can be called from everywhere in the process,
 * to store and retreive data.
 * The data is organized in a tree like structure and can be accessed by a 'URL' (dot separated string)
 */

class DECLSPEC_appsupport ConfigClient : public ConfigBase {
  DISABLE_COPY_MOVE(ConfigClient)

private:
  ConfigClient(std::string name);
  ~ConfigClient();

  // move later to config_client impl
  friend class CmdOptions_Impl;

public:
  //! Initialize the config client
  /*!
   * It is not allowed to call the config client before this call.
   * The function will preset all known keys with default values.
   * After that, it will retreive the config from the global config, if available.
   * It will block any automatical updates to the config server.
   *
   * @param name     Component name
   */
  static bool do_init(std::string name);

  //! Finishes the config client initialization
  /*!
   * After this the configuration client is initialized.
   * It will unblock all server comunication.
   *
   */
  static bool finish_init();

  //! teardown the config client
  /*!
   * It is not allowed to call the config client after this call.
   * TODO describe the function - saving config / pushing to global / disconnection
   *
   */
  static void do_reset();

  //! Access the config functions
  /*!
   * Get a pointer to the singleton to be used to access the configuration values.
   * This can be used to access the configuration everywhere in the process, after the do_init call.
   *
   * @return       Reference to the configuration handler singleton
   */
  static ConfigClient *getConfig();

  void safeWaitForResponse();

  //! Access the config using lists as results for more flexible request values
  std::vector<std::string> getStringList(const std::string &key, const std::vector<std::string> &defaults,
                                         bool block_active, bool &value_available) const override;

  inline std::vector<std::string> getStringList(const std::string &key, const std::vector<std::string> &defaults,
                                                bool block_active = false) const {
    bool _;
    return getStringList(key, defaults, block_active, _);
  }

  std::vector<std::string> getChildren(const std::string &key, bool full_path = false) override;

  int get_int(const std::string &key, const int &default_value, bool block_active,
              bool &value_available) const override;

  inline int get_int(const std::string &key, const int &default_value, bool block_active = false) const {
    bool _;
    return get_int(key, default_value, block_active, _);
  }

  float get_float(const std::string &key, const float &default_value, bool block_active,
                  bool &value_available) const override;

  inline float get_float(const std::string &key, const float &default_value, bool block_active = false) const {
    bool _;
    return get_float(key, default_value, block_active, _);
  }

  std::string get_string(const std::string &key, const std::string &default_value, bool block_active,
                         bool &value_available) const override;
  inline std::string get_string(const std::string &key, const std::string &default_value,
                                bool block_active = false) const {
    bool _;
    return get_string(key, default_value, block_active, _);
  }

  bool get_option(const std::string &key, const bool &default_value, bool block_active,
                  bool &value_available) const override;
  inline bool get_option(const std::string &key, const bool &default_value, bool block_active = false) const {
    bool _;
    return get_option(key, default_value, block_active, _);
  }

  void SetValidConfigServer(const std::string &serverName);
  inline void put_option(const std::string &key, const bool &value) { put_option(key, value, ConfigSource::Component); }
  inline void put(const std::string &key, const std::string &value) { put(key, value, ConfigSource::Component); }
  inline void put(const std::string &key, const float &value) { put(key, value, ConfigSource::Component); }
  inline void put(const std::string &key, const int &value) { put(key, value, ConfigSource::Component); }

  inline void putCfg(const std::string &filePath) { putCfg(filePath, ConfigSource::Component); }

  inline void insert(const std::string &json_str, bool filterOutPrivateKeys = false, bool resetActiveTag = false) {
    insert(json_str, filterOutPrivateKeys, resetActiveTag, ConfigSource::Component);
  }

  /*void setTag(const std::string &key, next::appsupport::configtags::TAG tag) override;
  void setTag(const std::string &key, const std::string &tag) override;*/
  /*void removeTag(const std::string &key, next::appsupport::configtags::TAG tag) override;
  void removeTag(const std::string &key, const std::string &tag) override;*/
  // void removeAllTagsForKey(const std::string &key) override;

  //! Set the config-server send time
  /*!
   * This functions allows set the time how often this client
   * is sending the config to the server.
   *
   * @param name     time_in_ms
   */
  void setConfigServerSendTime(uint16_t time_in_ms);

protected:
  //! callback for the request event
  /*!
   * called, whenever a request event is received.
   * TODO describe the request event
   *
   */
  void triggerConfigSendToServer(const std::string &requester, bool sendConfigTags = true);
  //! callback for the overwrite event
  /*!
   * called, whenever an overwrite event is received.
   * TODO describe the overwrite event
   *
   */
  void onConfigReceivedFromServerEventCalled(const next::sdk::events::SendConfigToClientInformationMessage &message);

  //! callback for the config value event
  /*!
   * called, whenever an config value event is received from the server
   *
   */
  void onConfigValueReceivedFromServerEventCalled(
      const next::sdk::events::SendConfigValueToClientInformationMessage &message);

  //! thread function for publishing config changes
  /*!
   * runs periodically and publishes the whole config in case there were changes
   *
   */
  void publishConfigEvent();

  // void sendTagListToServer();

private:
  void put_option(const std::string &key, const bool &value, const ConfigSource &source) override;
  void put(const std::string &key, const std::string &value, const ConfigSource &source) override;
  void put(const std::string &key, const float &value, const ConfigSource &source) override;
  void put(const std::string &key, const int &value, const ConfigSource &source) override;

  void putCfg(const std::string &filePath, const ConfigSource &source) override;

  void insert(const std::string &json_str, bool filterOutPrivateKeys, bool resetActiveTag,
              const ConfigSource &source) override;

  void registerActiveServers(const std::string &serverName);
  bool isRequestForDifferentComponent(const std::string &key);
  static ConfigClient *_self;
  bool server_comunication_enabled_ = false;
  bool response_from_server_received_ = false;
  next::sdk::events::RequestConfigFromServer _request_config_from_server_event{""};
  next::sdk::events::RequestConfigValueFromServer _request_config_value_from_server_event{""};
  next::sdk::events::RequestConfigFromClient _request_config_from_client_event{""};
  next::sdk::events::SendConfigToClient _send_config_to_client_event{""};
  next::sdk::events::SendConfigValueToClient _send_config_value_to_client_event{""};
  next::sdk::events::SendConfigToServer _send_config_to_server_event{""};
  next::sdk::events::SendConfigTagsToServer _send_config_tags_to_server_event{""};
  mutable next::sdk::events::RequestConfigValueFromServerInformationMessage last_request_message_{};
  NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS
  std::string valid_config_server_name_ = "";
  std::vector<std::string> active_config_servers_;
  mutable std::mutex server_communication_mutex_{};
  std::atomic_bool send_config_data_{true};
  std::atomic_bool config_data_changed{false};
  std::atomic_bool tags_changed{false};
  std::mutex mutex_wait_send_data_;
  std::condition_variable cond_wait_send_data_;
  std::atomic<std::chrono::milliseconds> wait_time_;
  std::chrono::milliseconds default_wait_time = std::chrono::milliseconds(2000);
  std::thread send_config_thread_;
  NEXT_RESTORE_WARNINGS_WINDOWS
};
} // namespace appsupport
} // namespace next

#endif // CONFIG_CLIENT_H

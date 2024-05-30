/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     config_server.hpp
 * @brief    Centralized Configuration Carrier
 *
 * Requests all the configurations from the nodes in network, aggregates them and builds up the
 * centralized configurations, and also send the centralized configurations to the nodes in the
 * network
 **/

#ifndef CONFIG_SERVER_H
#define CONFIG_SERVER_H

#include <atomic>
#include <string>

#include "config_base.hpp"

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
 * @brief  Configuration Server class
 * @brief Implementing the functions for the global configuration.
 *
 * TODO describe the usage of the config server
 * The data is organized in a tree like structure and can be accessed by a 'URL' (dot separated string)
 */

class DECLSPEC_appsupport ConfigServer : public ConfigBase {
public:
  //! Constructor of the config server
  /*!
   * Config server is not a singleton.
   *
   * @param name     TODO what is the name here?
   */
  ConfigServer(const std::string &name);

  //! Push the global configuration to the clients
  /*!
   * TODO describe the function
   */
  void pushGlobalConfig(const std::string &requester = "", bool resetActive = false);

  //! Push the update configuration to the clients
  /*!
   *
   */
  void pushConfig(const std::string &jsonConfig);

  //! request the local configuration from the clients
  /*!
   * TODO Is this always requesting the config from all clients?
   */
  void requestLocalConfig(void);

  //! write the global configuration to a JSON string
  /*!
   * TODO is thewre any way of filtering? Or should there be?
   *
   * @return           the JSON string of the full global config tree
   */
  std::string dumpConfigJson();

  //! request the local configuration from the clients
  /*!
   */
  void probe_clients_for_configuration();

  /*! Clears the Configurations
   */
  void clearConfig() const;

private:
  void pushConfigValue(const next::sdk::events::RequestConfigValueFromServerInformationMessage &message);
  // void pushReceivedTagList(const next::sdk::events::SendConfigTagsToServerInformationMessage &message);
  void onRequestEventHandlerCalled();

  next::sdk::events::RequestConfigFromServer _request_config_from_server_event{""};
  next::sdk::events::RequestConfigValueFromServer _request_config_value_from_server_event{""};
  next::sdk::events::RequestConfigFromClient _request_config_from_client_event{""};

  next::sdk::events::SendConfigToClient _send_config_to_client_event{""};
  next::sdk::events::SendConfigValueToClient _send_config_value_to_client_event{""};
  next::sdk::events::SendConfigToServer _send_config_to_server_event{""};
  next::sdk::events::SendConfigTagsToServer _send_config_tags_to_server_event{""};
  NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS
  mutable std::mutex server_communication_mutex_{};
  NEXT_RESTORE_WARNINGS_WINDOWS
  long long _LIMIT_MILISECONDS = 100;
};

} // namespace appsupport
} // namespace next

#endif // BRIDGE_SDK_CONFIG_SERVER_H_

/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     config_server.cpp
 * @brief    implementation for ConfigServer
 *
 * See config_server.hpp.
 *
 **/

#include <regex>
#include <thread>

#include <next/appsupport/config/config_server.hpp>
#include <next/sdk/logger/logger.hpp>

namespace next {
namespace appsupport {

ConfigServer::ConfigServer(const std::string &name)
    : ConfigBase(name + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                                           std::chrono::system_clock::now().time_since_epoch())
                                           .count()),
                 false) {
  // TODO remove once ConfigServer doesn't derive anymore from ConfigBase
  // needed to set init to finished
  ConfigBase::finishInitialization();

  _request_config_from_server_event.addEventHook(
      [this]() { this->pushGlobalConfig(_request_config_from_server_event.getEventData().requester_name); });
  _request_config_from_server_event.subscribe();

  _request_config_value_from_server_event.addEventHook(
      [this]() { this->pushConfigValue(_request_config_value_from_server_event.getEventData()); });
  _request_config_value_from_server_event.subscribe();

  _send_config_to_server_event.addEventHook(std::bind(&ConfigServer::onRequestEventHandlerCalled, this));
  _send_config_to_server_event.subscribe();

  _send_config_tags_to_server_event.addEventHook([this]() {
    std::lock_guard<std::mutex> guard{server_communication_mutex_};
    // TODO  comment in after implementation in config class
    // this->pushReceivedTagList(_send_config_tags_to_server_event.getEventData());
  });
  _send_config_tags_to_server_event.subscribe();
  // publish a dummy event so the publishers are ready and avoid a 2 second delay creating them at runtime;
  // TODO:  update after https://jira-adas.zone2.agileci.conti.de/browse/SIMEN-7406 is done

  _send_config_value_to_client_event.initialize();
  _request_config_from_client_event.initialize();
}

void ConfigServer::probe_clients_for_configuration() {
  next::sdk::events::RequestConfigFromClientInformationMessage payload;

  payload.requester_name = getRootName();
  _request_config_from_client_event.publish(payload);
  std::this_thread::sleep_for(std::chrono::milliseconds(_LIMIT_MILISECONDS));
}

/*void ConfigServer::pushReceivedTagList(const next::sdk::events::SendConfigTagsToServerInformationMessage &message) {
  if (message.tags_json.size()) {
    this->updateTagsFromJsonString(message.tags_json);
  }
}*/
void ConfigServer::pushConfigValue(const next::sdk::events::RequestConfigValueFromServerInformationMessage &message) {
  if (message.request_key.size() == 0)
    return;
  std::lock_guard<std::mutex> guard{server_communication_mutex_};
  next::sdk::events::SendConfigValueToClientInformationMessage outputMessage;
  outputMessage.receiver_name = message.requester_name;

  outputMessage.sender_name = getRootName();
  outputMessage.request_key = message.request_key;
  outputMessage.request_type = message.request_type;
  if (message.request_type.compare("bool") == 0) {
    outputMessage.config_value = std::to_string(get_option(message.request_key, false));
  } else if (message.request_type.compare("int") == 0) {
    int defaultValue = 0;
    try {
      defaultValue = std::stoi(message.request_default_value);
    } catch (const std::exception &e) {
      next::sdk::logger::warn(__FILE__,
                              "Could not convert default value [{0}] to int, using implementation default. {1}",
                              message.request_default_value, e.what());
    }
    outputMessage.config_value = std::to_string(get_int(message.request_key, defaultValue));
  } else if (message.request_type.compare("float") == 0) {
    float defaultValue = 0.0f;
    try {
      defaultValue = std::stof(message.request_default_value);
    } catch (const std::exception &e) {
      next::sdk::logger::warn(__FILE__,
                              "Could not convert default value [{0}] to float, using implementation default. {1}",
                              message.request_default_value, e.what());
    }
    outputMessage.config_value = std::to_string(get_float(message.request_key, defaultValue));
  } else if (message.request_type.compare("string") == 0) {

    outputMessage.config_value = get_string(message.request_key, message.request_default_value);
  } else if (message.request_type.compare("stringList") == 0) {
    std::regex token("\\:\\$\\@");
    std::vector<std::string> defaults(std::sregex_token_iterator(message.request_default_value.begin(),
                                                                 message.request_default_value.end(), token, -1),
                                      std::sregex_token_iterator());
    std::vector<std::string> result = getStringList(message.request_key, defaults);
    outputMessage.config_value = "";
    bool firstElement = true;
    for (auto value : result) {
      if (!firstElement) {
        outputMessage.config_value.append(":$@");
      }
      outputMessage.config_value.append(value);
      firstElement = false;
    }
  } else if (message.request_type.compare("getChildren") == 0) {
    std::vector<std::string> result = getChildren(message.request_key);
    outputMessage.config_value = "";
    bool firstElement = true;
    for (auto value : result) {
      if (!firstElement) {
        outputMessage.config_value.append(":$@");
      }
      outputMessage.config_value.append(value);
      firstElement = false;
    }
  }
  _send_config_value_to_client_event.publish(outputMessage);
}

void ConfigServer::pushGlobalConfig(const std::string &requester, bool resetActive) {
  std::lock_guard<std::mutex> guard{server_communication_mutex_};
  auto all_configs_as_json_str = this->getAllConfigurationsAsJson();
  next::sdk::events::SendConfigToClientInformationMessage msg;
  msg.config_json = all_configs_as_json_str;
  msg.receiver_name = requester;
  msg.sender_name = getRootName();
  msg.reset_active = resetActive;
  this->_send_config_to_client_event.publish(msg);
}
void ConfigServer::pushConfig(const std::string &jsonConfig) {
  auto all_configs_as_json_str = jsonConfig;
  next::sdk::events::SendConfigToClientInformationMessage msg;
  msg.config_json = all_configs_as_json_str;
  msg.receiver_name = "";
  msg.sender_name = getRootName();
  this->_send_config_to_client_event.publish(msg);
}

void ConfigServer::requestLocalConfig() {
  next::sdk::events::RequestConfigFromServerInformationMessage para{};
  para.requester_name = getRootName();
  _request_config_from_server_event.publish(para);
  std::this_thread::sleep_for(std::chrono::milliseconds(_LIMIT_MILISECONDS));
}

std::string ConfigServer::dumpConfigJson() {
  probe_clients_for_configuration();
  std::lock_guard<std::mutex> guard{server_communication_mutex_};
  return this->getAllConfigurationsAsJson();
}

void ConfigServer::onRequestEventHandlerCalled() {
  auto sender_info = _send_config_to_server_event.getEventData();
  // adding everything without checking who sent it
  std::lock_guard<std::mutex> guard{server_communication_mutex_};
  this->insert(sender_info.config_json);
  // todo : discuss below mentioned ideas
  if (sender_info.config_json == "player") {
    // todo what to do ?
  } else if (sender_info.config_json == "gui") {
    // todo what to do ?
  } else if (sender_info.config_json == "bridge") {
    // todo what to do ?
  }
}

void ConfigServer::clearConfig() const { this->clearTree(); }

} // namespace appsupport
} // namespace next

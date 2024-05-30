/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     config_client.cpp
 * @brief    implementation for ConfigClient
 *
 * See config_client.hpp
 *
 **/
#include <cassert>
#include <regex>
#include <thread>

#include <boost/lexical_cast.hpp>
#include <boost/math/special_functions/relative_difference.hpp>

#include <next/appsupport/config/config_client.hpp>
#include <next/sdk/logger/logger.hpp>

namespace next {
namespace appsupport {

ConfigClient *ConfigClient::_self = nullptr;

ConfigClient::ConfigClient(std::string name) : ConfigBase(name, true) {
  _send_config_to_client_event.addEventHook(
      [this]() { this->onConfigReceivedFromServerEventCalled(_send_config_to_client_event.getEventData()); });
  _send_config_to_client_event.subscribe();

  _send_config_value_to_client_event.addEventHook([this]() {
    this->onConfigValueReceivedFromServerEventCalled(_send_config_value_to_client_event.getEventData());
  });
  _send_config_value_to_client_event.subscribe();

  _request_config_from_client_event.addEventHook(
      [this]() { this->triggerConfigSendToServer(_request_config_from_client_event.getEventData().requester_name); });
  _request_config_from_client_event.subscribe();

  // enableFilterByComponent(); //set via constructor of ConfigBase

  _request_config_value_from_server_event.initialize();
  _send_config_to_server_event.initialize();
  _send_config_tags_to_server_event.initialize();

  wait_time_.store(default_wait_time); // TODO: make settable via config
  send_config_thread_ = std::thread([&] { publishConfigEvent(); });
}

ConfigClient::~ConfigClient() {
  send_config_data_.store(false);
  cond_wait_send_data_.notify_all();
  send_config_thread_.join();
}

void ConfigClient::onConfigReceivedFromServerEventCalled(
    const next::sdk::events::SendConfigToClientInformationMessage &message) {
  if (server_comunication_enabled_) {
    registerActiveServers(message.sender_name);
    // if the server name is set and does not matches the sender name ignore the message
    if (valid_config_server_name_.size() && !valid_config_server_name_.compare(message.sender_name) == 0) {
      next::sdk::logger::warn(__FILE__,
                              "Expected message from server [{0}] and received from server [{1}]; Message ignored",
                              valid_config_server_name_, message.sender_name);
      return;
    }
    // empty receiver name means update for all
    if (message.receiver_name.empty() || getConfig()->isComponentInCurrentTree(message.receiver_name)) {
      // this->checkGenericConfig(message.config_json);

      // Hint: as soon as config initialization is finished, the current source will be automatically used instead of
      // the ConfigServer
      this->insert(message.config_json, true, false, ConfigSource::ConfigServer);
      this->setWaitForResponseFlag(true);
    }
  }
}
void ConfigClient::onConfigValueReceivedFromServerEventCalled(
    const next::sdk::events::SendConfigValueToClientInformationMessage &message) {
  if (server_comunication_enabled_) {
    registerActiveServers(message.sender_name);
    // if the server name is set and does not matches the sender name ignore the message
    if (valid_config_server_name_.size() && !valid_config_server_name_.compare(message.sender_name) == 0) {
      next::sdk::logger::warn(__FILE__,
                              "Expected message from server [{0}] and received from server [{1}]; Message ignored",
                              valid_config_server_name_, message.sender_name);
      return;
    }
    // make sure we receive what we requested

    if (getConfig()->isComponentInCurrentTree(message.receiver_name) &&
        message.request_key.compare(last_request_message_.request_key) == 0 &&
        message.request_type.compare(last_request_message_.request_type) == 0) {
      response_from_server_received_ = true;
      getConfig()->setConfigValueFromServer(message.config_value);
      this->setWaitForResponseFlag(true);
    }
  }
}
/*void ConfigClient::sendTagListToServer() {
  if (server_comunication_enabled_) {
    std::lock_guard<std::mutex> guard{server_communication_mutex_};
    next::sdk::events::SendConfigTagsToServerInformationMessage tagsPayload;
    tagsPayload.tags_json = getTagsAsJsonStr();
    tagsPayload.sender_componet_name = getRootName();
    _send_config_tags_to_server_event.publish(tagsPayload);
  }
}*/

void ConfigClient::publishConfigEvent() {
  // TODO: run this in separate thread?
  while (send_config_data_) {
    // if (config_data_changed)
    if (server_comunication_enabled_ && config_data_changed.load()) {
      config_data_changed.store(false);
      std::lock_guard<std::mutex> guard{server_communication_mutex_};
      next::sdk::events::SendConfigToServerInformationMessage payload;
      payload.sender_componet_name = getRootName();
      payload.config_json = this->getAllConfigurationsAsJson();
      _send_config_to_server_event.publish(payload);
    }
    if (tags_changed.load()) {
      // sendTagListToServer();
    }
    std::unique_lock<std::mutex> lock_wait(mutex_wait_send_data_);
    if (config_data_changed.load() == false) {
      cond_wait_send_data_.wait_for(lock_wait, wait_time_.load(), [this]() { return !send_config_data_; });
    }
  }
}

// setting only flag here
void ConfigClient::triggerConfigSendToServer(const std::string &requester, [[maybe_unused]] bool sendConfigTags) {
  if (server_comunication_enabled_) {
    registerActiveServers(requester);
    config_data_changed.store(true);
    cond_wait_send_data_.notify_all();
    tags_changed.store(false); // TODO: set with sendConfigTags flag
  }
}

void ConfigClient::setConfigServerSendTime(uint16_t time_in_ms) {
  wait_time_.store(std::chrono::milliseconds(time_in_ms));
}

bool ConfigClient::do_init(std::string name) {
  if (_self == nullptr) {
    _self = new ConfigClient(name);
  }
  getConfig()->addNewComponentToLocalTree(name);

  return true;
}

bool ConfigClient::finish_init() {
  if (_self) {
    // TO DO - this has to be reenabled once the crash using events is fixed
    next::sdk::events::RequestConfigFromServerInformationMessage message;
    message.requester_name = getConfig()->getRootName();
    getConfig()->server_comunication_enabled_ = true;
    getConfig()->setWaitForResponseFlag(false);
    getConfig()->_request_config_from_server_event.publish(message);
    // wait a few seconds to be sure we would receive something
    getConfig()->waitForResponse();
    getConfig()->server_comunication_enabled_ = false;
    getConfig()->finishInitialization();

    // propagate changes to server
    getConfig()->server_comunication_enabled_ = true;
    getConfig()->triggerConfigSendToServer("");
  }
  return true;
}
void ConfigClient::do_reset() {
  if (_self) {
    delete _self;
    _self = nullptr;
  }
}
void ConfigClient::put_option(const std::string &key, const bool &value, const ConfigSource &source) {
  if (getConfig()->isRequestForDifferentComponent(key))
    return;
  ConfigBase::put_option(key, value, source);
  if (ConfigBase::isInitFinished()) {
    getConfig()->triggerConfigSendToServer("", false);
  }
}
void ConfigClient::put(const std::string &key, const std::string &value, const ConfigSource &source) {
  if (getConfig()->isRequestForDifferentComponent(key))
    return;
  ConfigBase::put(key, value, source);
  if (ConfigBase::isInitFinished()) {
    getConfig()->triggerConfigSendToServer("", false);
  }
}
void ConfigClient::put(const std::string &key, const float &value, const ConfigSource &source) {
  if (getConfig()->isRequestForDifferentComponent(key))
    return;
  ConfigBase::put(key, value, source);
  if (ConfigBase::isInitFinished()) {
    getConfig()->triggerConfigSendToServer("", false);
  }
}
void ConfigClient::put(const std::string &key, const int &value, const ConfigSource &source) {
  if (getConfig()->isRequestForDifferentComponent(key))
    return;
  ConfigBase::put(key, value, source);
  if (ConfigBase::isInitFinished()) {
    getConfig()->triggerConfigSendToServer("", false);
  }
}

void ConfigClient::putCfg(const std::string &filePath, const ConfigSource &source) {
  ConfigBase::putCfg(filePath, source);
}

void ConfigClient::insert(const std::string &json_str, bool filterOutPrivateKeys, bool resetActiveTag,
                          const ConfigSource &source) {
  ConfigBase::insert(json_str, filterOutPrivateKeys, resetActiveTag, source);
}

// void ConfigClient::setTag([[maybe_unused]] const std::string &key,
//                          [[maybe_unused]] next::appsupport::configtags::TAG tag) {
//  /*if (getConfig()->isRequestForDifferentComponent(key))
//    return;
//  ConfigBase::setTag(key, tag);
//  sendTagListToServer();*/
//}
// void ConfigClient::setTag([[maybe_unused]] const std::string &key, [[maybe_unused]] const std::string &tag) {
//  /*if (getConfig()->isRequestForDifferentComponent(key))
//    return;
//  ConfigBase::setTag(key, tag);
//  sendTagListToServer();*/
//}
// void ConfigClient::removeTag([[maybe_unused]] const std::string &key,
//                             [[maybe_unused]] next::appsupport::configtags::TAG tag) {
//  /*if (getConfig()->isRequestForDifferentComponent(key))
//    return;
//  ConfigBase::removeTag(key, tag);
//  sendTagListToServer();*/
//}
// void ConfigClient::removeTag([[maybe_unused]] const std::string &key, [[maybe_unused]] const std::string &tag) {
//  /*if (getConfig()->isRequestForDifferentComponent(key))
//    return;
//  ConfigBase::removeTag(key, tag);
//  sendTagListToServer();*/
//}
// void ConfigClient::removeAllTagsForKey([[maybe_unused]] const std::string &key) {
/* if (getConfig()->isRequestForDifferentComponent(key))
   return;
 ConfigBase::removeAllTagsForKey(key);
 sendTagListToServer();*/
//}

std::vector<std::string> ConfigClient::getChildren(const std::string &key, bool full_path) {
  std::vector<std::string> returnValue;
  returnValue = ConfigBase::getChildren(key, full_path);
  if (returnValue.size() == 0 && getConfig()->isRequestForDifferentComponent(key)) {
    std::lock_guard<std::mutex> guard{server_communication_mutex_};
    response_from_server_received_ = false;
    last_request_message_.requester_name = getConfig()->getRootName();
    last_request_message_.request_key = key;
    last_request_message_.request_type = "getChildren";
    last_request_message_.request_default_value = "";
    getConfig()->setWaitForResponseFlag(false);
    getConfig()->_request_config_value_from_server_event.publish(last_request_message_);
    getConfig()->safeWaitForResponse();
    auto value = getConfig()->getConfigValueFromServer();
    if (value.size()) {
      try {
        std::regex token("\\:\\$\\@");
        auto fromServer = value;
        std::vector<std::string> list(std::sregex_token_iterator(fromServer.begin(), fromServer.end(), token, -1),
                                      std::sregex_token_iterator());
        returnValue = list;
      } catch (const std::exception &e) {
        next::sdk::logger::warn(__FILE__, "Could not convert value from server [{0}] to int. {1}", value, e.what());
      }
      getConfig()->setConfigValueFromServer("");
    }
  }
  return returnValue;
}

std::vector<std::string> ConfigClient::getStringList(const std::string &key, const std::vector<std::string> &defaults,
                                                     bool block_active, bool &value_available) const {

  std::vector<std::string> values;
  values = ConfigBase::getStringList(key, defaults, block_active, value_available);
  if (!value_available && getConfig()->isRequestForDifferentComponent(key)) {
    std::lock_guard<std::mutex> guard{server_communication_mutex_};
    last_request_message_.requester_name = getConfig()->getRootName();
    last_request_message_.request_key = key;
    last_request_message_.request_type = "stringList";
    std::string default_value = "";
    bool firstElement = true;
    for (auto defaultVal : defaults) {
      if (!firstElement) {
        default_value.append(":$@");
      }
      default_value.append(defaultVal);
      firstElement = false;
    }
    last_request_message_.request_default_value = default_value;
    getConfig()->setWaitForResponseFlag(false);
    getConfig()->_request_config_value_from_server_event.publish(last_request_message_);
    getConfig()->safeWaitForResponse();
    if (response_from_server_received_) {
      // reset default values as server value could be ""
      values.clear();
    }
    if (getConfig()->getConfigValueFromServer().size()) {
      try {
        std::regex token("\\:\\$\\@");
        auto fromServer = getConfig()->getConfigValueFromServer();
        std::vector<std::string> list(std::sregex_token_iterator(fromServer.begin(), fromServer.end(), token, -1),
                                      std::sregex_token_iterator());
        values = list;
      } catch (const std::exception &e) {
        next::sdk::logger::warn(__FILE__, "Could not convert value from server [{0}] to int. {1}",
                                getConfig()->getConfigValueFromServer(), e.what());
      }
      getConfig()->setConfigValueFromServer("");
    }
  }
  return values;
}

int ConfigClient::get_int(const std::string &key, const int &default_value, bool block_active,
                          bool &value_available) const {
  int result = ConfigBase::get_int(key, default_value, block_active, value_available);
  // check if it is requested from a different component
  if (!value_available && getConfig()->isRequestForDifferentComponent(key)) {
    std::lock_guard<std::mutex> guard{server_communication_mutex_};
    last_request_message_.requester_name = getConfig()->getRootName();
    last_request_message_.request_key = key;
    last_request_message_.request_type = "int";
    last_request_message_.request_default_value = std::to_string(default_value);
    getConfig()->setWaitForResponseFlag(false);
    getConfig()->_request_config_value_from_server_event.publish(last_request_message_);
    getConfig()->safeWaitForResponse();
    auto value = getConfig()->getConfigValueFromServer();
    if (value.size()) {
      try {
        result = std::stoi(value);
      } catch (const std::exception &e) {
        next::sdk::logger::warn(__FILE__, "Could not convert value from server [{0}] to int. {1}",
                                getConfig()->getConfigValueFromServer(), e.what());
      }
      getConfig()->setConfigValueFromServer("");
    }
  }
  return result;
}
float ConfigClient::get_float(const std::string &key, const float &default_value, bool block_active,
                              bool &value_available) const {
  float result = ConfigBase::get_float(key, default_value, block_active, value_available);
  // check if it is requested from a different component
  if (!value_available && getConfig()->isRequestForDifferentComponent(key)) {
    std::lock_guard<std::mutex> guard{server_communication_mutex_};
    last_request_message_.requester_name = getConfig()->getRootName();
    last_request_message_.request_key = key;
    last_request_message_.request_type = "float";
    last_request_message_.request_default_value = std::to_string(default_value);
    getConfig()->setWaitForResponseFlag(false);
    getConfig()->_request_config_value_from_server_event.publish(last_request_message_);
    getConfig()->safeWaitForResponse();
    auto value = getConfig()->getConfigValueFromServer();
    if (value.size()) {
      try {
        result = std::stof(getConfig()->getConfigValueFromServer());
      } catch (const std::exception &e) {
        next::sdk::logger::warn(__FILE__, "Could not convert value from server [{0}] to float. {1}",
                                getConfig()->getConfigValueFromServer(), e.what());
      }
      getConfig()->setConfigValueFromServer("");
    }
  }
  return result;
}
std::string ConfigClient::get_string(const std::string &key, const std::string &default_value, bool block_active,
                                     bool &value_available) const {
  std::string result = ConfigBase::get_string(key, default_value, block_active, value_available);
  // check if it is requested from a different component
  if (!value_available && getConfig()->isRequestForDifferentComponent(key)) {
    std::lock_guard<std::mutex> guard{server_communication_mutex_};
    last_request_message_.requester_name = getConfig()->getRootName();
    last_request_message_.request_key = key;
    last_request_message_.request_type = "string";
    last_request_message_.request_default_value = default_value;
    getConfig()->setWaitForResponseFlag(false);
    getConfig()->_request_config_value_from_server_event.publish(last_request_message_);
    getConfig()->safeWaitForResponse();
    if (response_from_server_received_) {
      // reset default values as server value could be ""
      result = "";
    }
    auto value = getConfig()->getConfigValueFromServer();
    if (value.size()) {
      result = value;
      getConfig()->setConfigValueFromServer("");
    }
  }
  return result;
}
bool ConfigClient::get_option(const std::string &key, const bool &default_value, bool block_active,
                              bool &value_available) const {
  bool result = ConfigBase::get_option(key, default_value, block_active, value_available);
  // check if it is requested from a different component
  if (!value_available && getConfig()->isRequestForDifferentComponent(key)) {
    std::lock_guard<std::mutex> guard{server_communication_mutex_};
    last_request_message_.requester_name = getConfig()->getRootName();
    last_request_message_.request_key = key;
    last_request_message_.request_type = "bool";
    last_request_message_.request_default_value = "";
    getConfig()->setWaitForResponseFlag(false);
    getConfig()->_request_config_value_from_server_event.publish(last_request_message_);
    getConfig()->safeWaitForResponse();
    auto value = getConfig()->getConfigValueFromServer();
    if (value.size()) {
      try {
        result = boost::lexical_cast<bool>(getConfig()->getConfigValueFromServer());

      } catch (boost::bad_lexical_cast const &e) {
        next::sdk::logger::warn(__FILE__, "Could not convert value from server [{0}] to bool. {1}",
                                getConfig()->getConfigValueFromServer(), e.what());
      }
      getConfig()->setConfigValueFromServer("");
    }
  }
  return result;
}
ConfigClient *ConfigClient::getConfig() {
  // todo: check if we should throw if _self is nullptr
  return _self;
}

bool ConfigClient::isRequestForDifferentComponent(const std::string &key) {
  bool retVal = false;
  auto componentNames = getConfig()->componentName();
  for (auto compName : componentNames) {
    if (key.compare(compName) == 0)
      return false;
    std::size_t found = key.find(compName);
    // component name should always be on the first position within the searched key
    if (found != 0) {
      retVal = true;
    } else if (key[compName.size()] != ':') {
      // check if ':' is the next character after component name
      // so there is no component:value - component1:value missmatch
      retVal = true;
    } else {
      retVal = false;
      break;
    }
  }

  return retVal;
}
void ConfigClient::SetValidConfigServer(const std::string &serverName) { valid_config_server_name_ = serverName; }

void ConfigClient::registerActiveServers(const std::string &serverName) {
  if (serverName.size()) {
    if (std::find(active_config_servers_.begin(), active_config_servers_.end(), serverName) ==
        active_config_servers_.end()) {
      active_config_servers_.push_back(serverName);
      if (active_config_servers_.size() > 1) {
        std::string logMessage = "Multiple Config servers detected; Active Config server list: ";
        bool firstElement = true;
        for (auto server : active_config_servers_) {
          if (!firstElement) {
            logMessage.append(", ");
          }
          logMessage.append(server);
          firstElement = false;
        }
        next::sdk::logger::warn(__FILE__, logMessage);
      }
    }
  }
}

void ConfigClient::safeWaitForResponse() {
  if (active_config_servers_.size() != 0) {
    getConfig()->waitForResponse();
  }
}
} // namespace appsupport
} // namespace next

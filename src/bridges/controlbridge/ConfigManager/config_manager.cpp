/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */

#include "config_manager.h"

#include <time.h>
#include <fstream>
#include <iomanip>
#include <sstream>

#include <boost/filesystem.hpp>

#include <json/json.h>

#include <next/sdk/logger/logger.hpp>

namespace next {
namespace bridges {

ConfigManager::ConfigManager(std::shared_ptr<next::bridgesdk::webserver::IWebServer> web_server,
                             std::shared_ptr<next::appsupport::ConfigServer> configServer,
                             const std::string &config_file_with_path)
    : _webserver(web_server) {
  _webserver->AddForwardMessageCallback(std::bind(&ConfigManager::payloadCallback, this, std::placeholders::_1));
  _config_server = configServer;
  init(config_file_with_path);
  _config_server->probe_clients_for_configuration();
}

void ConfigManager::init(const std::string &config_file_with_path) {
  if (!config_file_with_path.empty()) {
    _config_file_with_path = config_file_with_path;
  } else {
    auto cur_dir = boost::filesystem::current_path();
    _config_file_with_path = cur_dir.string() + "/next_backend_conf.json";
  }
  try {
    _config_server->putCfg(_config_file_with_path);
    _config_server->pushGlobalConfig("", true);
  } catch (std::exception &e) {
    std::cout << e.what();
  }
}

ConfigManager::~ConfigManager() { _config_server.reset(); }

void ConfigManager::dumpAsAJsonFile(const std::string &file_path) {
  std::ofstream write_file_handler(file_path);
  auto config_data_from_server = _config_server->getAllConfigurationsAsJson(true, true, true, true);
  write_file_handler << config_data_from_server;
  write_file_handler.close();
}
void ConfigManager::dumpAsAJsonFile(const std::string &file_path, const std::string &component_name) {
  auto config_data_from_server = _config_server->getComponentConfigurationsAsJson(component_name, true, true);
  if (config_data_from_server.size()) {
    std::ofstream write_file_handler(file_path);
    write_file_handler << config_data_from_server;
    write_file_handler.close();
  }
}

void ConfigManager::payloadCallback(std::vector<uint8_t> data) {
  // event from gui load and save
  std::string config_json_payload(data.begin(), data.end());

  Json::Value parsedFromString;
  Json::Reader reader;
  if (!reader.parse(config_json_payload, parsedFromString))
    return;

  if (!parsedFromString["event"] || parsedFromString["event"].isNull() || !parsedFromString["payload"].isObject() ||
      !parsedFromString["event"].isString()) {
    error(__FILE__, "controlbridge::config_manager:: json string is invalid, missing event or payload");
    return;
  }

  auto event_name = parsedFromString["event"].asString();

  if (event_name == "UserPressedSaveGlobalConfig") {
    auto file_path = parsedFromString["payload"]["filePath"].asString();
    dumpAsAJsonFile(file_path);
  } else if (event_name == "UserPressedSaveComponentConfig") {
    auto file_path = parsedFromString["payload"]["filePath"].asString();
    auto component_name = parsedFromString["payload"]["component"].asString();
    dumpAsAJsonFile(file_path, component_name);
  } else if (event_name == "UserPressedLoadGlobalConfig") {

    auto file_path = parsedFromString["payload"]["filePath"].asString();
    if (file_path.empty())
      return;
    _config_server->clearConfig();
    _config_server->putCfg(file_path);
    pushDataToNodes();
    _config_server->probe_clients_for_configuration();
    pushDataToGui();

  } else if (event_name == "UserPressedUpdateGlobalConfig") {
    _config_server->probe_clients_for_configuration();
    pushDataToGui();
  } else if (event_name == "UserPressedActivateGlobalConfig") {
    std::string jsonString = "";
    try {
      jsonString = parsedFromString["payload"]["jsonString"].asString();
    } catch (...) {
      error(__FILE__,
            "controlbridge::config_manager:: could not get jsonString from UserPressedActivateGlobalConfig message");
      return;
    }

    if (jsonString.size()) {
      auto update = _config_server->getDifferences(jsonString);
      _config_server->clearConfig();
      _config_server->insert(jsonString);

      _config_server->pushConfig(update);
      _config_server->probe_clients_for_configuration();
      pushDataToGui();
    }
  } else {
    debug(__FILE__, "controlbridge::config_manager:: ignore the eventtype");
  }
}

void ConfigManager::pushDataToNodes() { _config_server->pushGlobalConfig("", true); }

void ConfigManager::pushDataToGui() {
  std::string config_wrapper_start = "{ \"channel\": \"configuration\","
                                     "\"event\": \"updateGlobalConfig\","
                                     "\"source\": \"NextBridge\","
                                     "\"payload\":{\"jsonString\":";
  std::string config_wrapper_end = "}}";
  auto config_data_json = _config_server->getAllConfigurationsAsJson();
  std::string config_tags_data_json = ",\"configurationTags\":";
  // TODO replace once it's implemented in config class
  config_tags_data_json += "null\n";
  // config_tags_data_json += _config_server->getTagsAsJsonStrForGUI();
  std::string wrapped_msg = config_wrapper_start + config_data_json + config_tags_data_json + config_wrapper_end;
  _data_for_gui.assign(wrapped_msg.begin(), wrapped_msg.end());
  _webserver->BroadcastMessageWithNotification(
      _data_for_gui, 0, {}, [](size_t id) { std::cout << " notification callback called " << id << "\n"; });
}

} // namespace bridges
} // namespace next

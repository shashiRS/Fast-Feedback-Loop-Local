/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     next_ui_config_request_handler.cpp
 *  @brief    Implementation to receive and process the config request from GUI.
 */

#include "next_ui_config_request_handler.h"
#include <next/sdk/logger/logger.hpp>
#include "event_handler_helper.h"
#include "json_to_plugin_config.h"

namespace next {
namespace databridge {
namespace ui_request {

void NextUiConfigRequestHandler::ForwardNextUiConfigRequest(
    std::function<void(DataClassManager::ConfigDataRequestItem)> next_ui_config_req_fwd, Json::Value command) {
  auto event_enum = EventHandlerHelper::StringToEventType(command["event"].asString());
  switch (event_enum) {
  case EventType::e_used_data_classes_changed: {
    ParseJsonCommandUsedDataClass(next_ui_config_req_fwd, command);
  } break;
  case EventType::e_user_requested_available_data_classes: {
    ParseJsonCommandAvailableDataClass(next_ui_config_req_fwd, command);
  } break;
  default:
    error(__FILE__, "{0} event wasn't found. Event is ignored.", command["event"].asString());
    break;
  }
}

void NextUiConfigRequestHandler::ParseJsonCommandUsedDataClass(
    std::function<void(DataClassManager::ConfigDataRequestItem)> config_req_callback, Json::Value command) {
  if (!command["payload"].isMember("configuredDataStreams")) {
    error(__FILE__, "Config Event could not be parsed. payload doesn't contain "
                    "configuredDataStreams. Payload invalid. Event is ignored.");
    return;
  }
  // parse JSON command and add it to config_request_queue
  // parse all configs and add the new configs, one by one to queue
  const Json::Value &datastreams = command["payload"]["configuredDataStreams"];

  for (const Json::Value &JSconfigs : datastreams) {
    if (!JSconfigs.isMember("dataStreamName")) {
      error(__FILE__, "Config Event could not be parsed. payload doesn't contain "
                      "dataStreamName. Payload invalid. Event is ignored.");
      return;
    }

    std::string data_stream_name = JSconfigs["dataStreamName"].asString();
    // reorder configs by plugin name
    std::map<std::string, DataClassManager::ConfigDataRequestItem> new_config_requests;

    for (const Json::Value &JSconfig : JSconfigs["configurations"]) {
      if (!JSconfig.isMember("plugin")) {
        error(__FILE__, "Config Event could not be parsed. payload doesn't contain "
                        "plugin. Payload invalid. Event is ignored.");
        return;
      }
      std::string plugin_name = JSconfig["plugin"].asString();
      // lookup if there is already a ConfigDataRequestItem for this plugin
      DataClassManager::ConfigDataRequestItem &config_item = new_config_requests[plugin_name];
      // yes it is double work but it does not hurt
      config_item.item_type = DataClassManager::ConfigDataRequestItem::ITEM_TYPE::SET_CONFIG;
      config_item.plugin_name = plugin_name;
      config_item.source_name = data_stream_name;

      next::bridgesdk::plugin_config_t config;
      next::databridge::data_request_server::JSONToPluginConfig::fromJson(JSconfig, config);
      // add dataStreamName to all configs
      config.insert({"DATA_VIEW", {bridgesdk::ConfigType::FIXED, data_stream_name}});
      config_item.configs.push_back(config);
    }
    HandleParsedConfig(config_req_callback, data_stream_name, new_config_requests);
  }
}

void NextUiConfigRequestHandler::ParseJsonCommandAvailableDataClass(
    std::function<void(DataClassManager::ConfigDataRequestItem)> config_req_callback, Json::Value command) {
  if (!command["payload"].isMember("availableInterfaces")) {
    error(__FILE__, "Config request Event could not be parsed. payload.availableInterfaces is not "
                    "available. Payload invalid. Event is ignored.");
    return;
  }
  // parse JSON command and add it to config_request_queue
  const Json::Value &items = command["payload"]["availableInterfaces"];
  if (items.isArray()) {
    for (const Json::Value &item : items) {
      if ((!item.isMember("schemaId")) && (!item.isMember("schemaVersion"))) {
        error(__FILE__, "Config request Event could not be parsed. schemaId or schemaVersion for "
                        "availableInterfaces is not available.  Payload invalid. Event is ignored.");
        return;
      }
      DataClassManager::ConfigDataRequestItem config_item;
      config_item.name = item["schemaId"].asString();
      config_item.version = item["schemaVersion"].asString();
      config_item.source_name = "";
      config_item.item_type = DataClassManager::ConfigDataRequestItem::ITEM_TYPE::GET_DESCRIPTION;
      config_req_callback(config_item);
    }
  }
}

void NextUiConfigRequestHandler::HandleParsedConfig(
    std::function<void(DataClassManager::ConfigDataRequestItem)> config_req_callback, const std::string &source,
    std::map<std::string, DataClassManager::ConfigDataRequestItem> &new_config_requests) {
  // search for previously configured plugins and their configurations
  // if 'source' doesn't exist, use default constructor to create empty map
  const auto &current_requests = previous_config_requests_[source];

  auto forward_empty_config = [&config_req_callback, &source](const std::string &p_name) {
    DataClassManager::ConfigDataRequestItem empty_request{};
    empty_request.item_type = DataClassManager::ConfigDataRequestItem::ITEM_TYPE::SET_CONFIG;
    empty_request.plugin_name = p_name;
    empty_request.source_name = source;
    config_req_callback(empty_request);
  };
  if (new_config_requests.empty()) {
    // if there are no configs send, than we need to delete all known configs
    // we do this by sending an empty request
    for (const auto &[k, v] : current_requests) {
      (void)v;
      forward_empty_config(k);
    }
  } else {

    for (const auto &[k, v] : current_requests) {
      (void)v;
      auto it = new_config_requests.find(k);
      if (it == std::end(new_config_requests)) {
        forward_empty_config(k);
      }
    }
    for (auto request_item : new_config_requests) {
      config_req_callback(request_item.second);
    }
  }
  previous_config_requests_[source] = new_config_requests;
}

} // namespace ui_request
} // namespace databridge
} // namespace next

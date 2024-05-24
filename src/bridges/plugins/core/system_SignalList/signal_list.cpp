/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     signal_list.cpp
 * @brief    Signal List Plugin
 *
 * See signal_list.h.
 *
 **/

#include "signal_list.h"

#ifdef _DEBUG
#include <sstream>
#endif
#include <mutex>
#include <regex>
#include <thread>

#include <next/plugins_utils/helper_functions.h>

namespace next {
namespace plugins {
using namespace next::plugins_utils;

SignalList::SignalList(const char *path) : next::bridgesdk::plugin::Plugin(path) {
  debug(__FILE__, "Hello system_SignalList plugin");
  this->setVersion(std::string("0.0.1-alpha1"));
  this->setName(std::string("system_SignalList"));
}

SignalList::~SignalList() {
  // nothing to do here
}

Json::Value SignalList::createPayloadJsonBySearchKeyword(const std::string &keyword) const {

  // Initialize the payload_json
  Json::Value payload_json;
  payload_json["dataStreamSchema"] = Json::arrayValue;

  // Get the vector with the child strings from UDEX
  std::vector<std::string> vector_children_URL;
  try {
    vector_children_URL = plugin_signal_explorer_->searchSignalTree(keyword);
  } catch (const std::exception &udex_e) {
    error(__FILE__, " UDex 'createPayloadJsonBySearchKeyword' error: {0}", udex_e.what());
    // Populate the child_json with error node and append to main
    Json::Value child_json = Json::objectValue;
    child_json["id"] = "error_loading";
    child_json["text"] = "error_loading";
    child_json["isDeletable"] = false;
    child_json["isEditable"] = false;
    child_json["isLeaf"] = false;
    child_json["isExpanded"] = false;
    child_json["children"] = Json::arrayValue;
    // Bye bye, problem child!
    payload_json["dataStreamSchema"].append(child_json);
    return payload_json;
  }

  for (auto &node : vector_children_URL) {
    Json::Value node_json{Json::objectValue};
    node_json["id"] = node;
    auto pos = node.find_last_of(".");
    std::string text = node;
    if (pos != std::string::npos)
      text = node.substr(pos + 1);

    node_json["text"] = text;
    node_json["uriPath"] = node;
    node_json["isDeletable"] = false;
    node_json["isEditable"] = false;
    node_json["isExpanded"] = false;
    node_json["isLeaf"] = true;
    node_json["children"] = Json::arrayValue;
    payload_json["dataStreamSchema"].append(node_json);
  }

  return payload_json;
}

// Converts the current_URL into the json structure
Json::Value SignalList::createPayloadJSONByURL(
    const std::string &current_URL,
    const std::vector<bridgesdk::plugin_addons::signal_child_info> &vector_childs_URL) const {
  // Initialize the payload_json
  Json::Value payload_json;
  payload_json["dataStreamSchema"] = Json::arrayValue;

  // Split the string into a vector
  std::string prefix("");
  if (current_URL.size() != 0) {
    prefix = current_URL + ".";
  }

  // Root case
  payload_json["parentNodeId"] = current_URL;

  debug(__FILE__, " Getting children of URL: '{}'...", current_URL);

  // Get the vector with the child strings from UDEX
  try {
  } catch (const std::exception &udex_e) {
    error(__FILE__, " UDex 'getChildsbyUrl' error: {0}", udex_e.what());
    // Populate the child_json with error node and append to main
    Json::Value child_json = Json::objectValue;
    child_json["id"] = "error_loading";
    child_json["text"] = "error_loading";
    child_json["isDeletable"] = false;
    child_json["isEditable"] = false;
    child_json["isLeaf"] = false;
    child_json["isExpanded"] = false;
    child_json["children"] = Json::arrayValue;
    // Bye bye, problem child!
    payload_json["dataStreamSchema"].append(child_json);
    return payload_json;
  }
  debug(__FILE__, " Getting children successful");

  for (const auto &child : vector_childs_URL) {
    Json::Value child_json{Json::objectValue};
    child_json["id"] = prefix + child.name;
    child_json["text"] =
        (child.array_size > 1) ? child.name + "[" + std::to_string(child.array_size) + "]" : child.name;
    child_json["isExpanded"] = false;
    child_json["isLeaf"] = child.child_count == 0 ? true : false;
    child_json["children"] = Json::arrayValue;
    child_json["arraySize"] = child.array_size;
    // Append it to the array
    payload_json["dataStreamSchema"].append(child_json);
  }

  return payload_json;
}

bool SignalList::init() {

  debug(__FILE__, " Initialize parsing...");

  // Create the output json, using the defined structure
  Json::Value signal_list_json;
  signal_list_json["channel"] = "dataStream";
  signal_list_json["event"] = "DataStreamFormatUpdate";
  signal_list_json["source"] = "NextGUI";
  plugin_signal_explorer_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExplorer>(this);

  // Generate the initial tree
  auto vector_childs_URL = plugin_signal_explorer_->getChildsbyUrl("");
  signal_list_json["payload"] = this->createPayloadJSONByURL("", vector_childs_URL);

  // Output as a std::string
  Json::StreamWriterBuilder builder;
  builder["indentation"] = "";
  // Format the output_json
  std::string output_json = Json::writeString(builder, signal_list_json);

  // Send the formatted json
  plugin_publisher_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataPublisher>(this);

  return true;
}

bool SignalList::update(std::vector<std::string> user_args) {
  update_user_args_ = user_args;

  debug(__FILE__, " Received user_args=[");
  for (const auto &user_arg : user_args) {
    debug(__FILE__, "'{}'", user_arg);
  }
  debug(__FILE__, "]");
  trim(user_args);
  debug(__FILE__, " Trimmed user_args=[");
  for (const auto &user_arg : user_args) {
    debug(__FILE__, "'{}'", user_arg);
  }
  debug(__FILE__, "]");

  // Create the output json, using the defined structure
  Json::Value signal_list_json;
  signal_list_json["channel"] = "dataStream";
  signal_list_json["event"] = "DataStreamFormatUpdate";
  signal_list_json["source"] = "NextGUI";

  if (!user_args.empty()) {

    auto string = user_args[0];
    auto pos = string.find(":");
    if (pos != std::string::npos) {
      auto key = string.substr(2, pos - 3);
      auto value = string.substr(pos + 2, string.length() - pos - 4);
      if (key == "searchString") {
        signal_list_json["payload"] = this->createPayloadJsonBySearchKeyword(value);
      } else {
        debug(__FILE__, " not handling any other unknown key except searchString");
      }
    } else {
      // Calculate the tree
      auto vector_childs_URL = plugin_signal_explorer_->getChildsbyUrl(user_args[0]);
      signal_list_json["payload"] = this->createPayloadJSONByURL(user_args[0], vector_childs_URL);
    }
  } else {
    auto vector_childs_URL = plugin_signal_explorer_->getChildsbyUrl("");
    signal_list_json["payload"] = this->createPayloadJSONByURL("", vector_childs_URL);
  }

  // Output as a std::string
  Json::StreamWriterBuilder builder;
  builder["indentation"] = "";
  // Format the output_json
  std::string output_json = Json::writeString(builder, signal_list_json);

  // sending it
  plugin_publisher_->sendResultData((uint8_t *)output_json.data(), output_json.size(), {},
                                    std::chrono::milliseconds(100), true);

  return true;
}

bool SignalList::enterReset() {
  // so far we are doing nothing here
  return true;
}

bool SignalList::exitReset() { return update(update_user_args_); }

} // namespace plugins
} // namespace next

plugin_create_t create_plugin(const char *path) { return new next::plugins::SignalList(path); }

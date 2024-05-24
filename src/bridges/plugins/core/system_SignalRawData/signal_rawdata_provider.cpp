/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     signal_rawdata_provider.cpp
 * @brief    Signal Rawdata Provider Plugin
 *
 * See signal_rawdata_provider.h.
 *
 **/

#include <algorithm>
#include <cassert>

#include <next/plugins_utils/helper_functions.h>

#include "signal_rawdata_provider.hpp"

namespace next {
namespace plugins {
using namespace next::plugins_utils;

RawDataProvider::RawDataProvider(const char *path) : next::bridgesdk::plugin::Plugin(path) {
  debug(__FILE__, "Hello system_SignalRawDataPlugin");
  this->setVersion(std::string("0.0.1-alpha1"));
  this->setName(std::string("system_SignalRawDataPlugin"));

  writer_builder_["commentStyle"] = "None";
  writer_builder_["indentation"] = "";
}

RawDataProvider::~RawDataProvider() {}

bool RawDataProvider::init() {
  plugin_publisher_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataPublisher>(this);
  plugin_signal_explorer_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExplorer>(this);
  plugin_signal_extractor_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExtractor>(this);
  return true;
}

bool RawDataProvider::update(std::vector<std::string> user_args) {
  // make sure that this function is only called by one thread at a time
  // also block the data processing function until topic/url registration is finished
  update_user_args_ = user_args;

  // structure of this function:
  // 1) parse the user_args to get a list of the requested URLs
  // 2) collect the needed topics for this URLs
  // 3) update the topic subscription (subscribe to new topics, unsubscribe from no longer needed ones)

  for (const auto &arg : user_args) {
    debug(__FILE__, " update given arg: {0}", arg);
  }
  std::vector<std::string> requested_urls = parseJsonArgumentToRequesteUrls(user_args);
  auto topic_to_new_urls = createTopicToUrlMapping(requested_urls);

  // 3)
  // update the topic subscription

  for (const auto &it : topic_to_new_urls) {
    const auto &topic = it.first;
    for (const auto &new_url : it.second) {
      topic_to_urls_[topic].urls.emplace_back(new_url, nullptr);
      topic_to_urls_[topic].urls.unique();
    }
  }
  // unsubscribe from no longer needed topics
  for (auto it = topic_to_urls_.begin(); topic_to_urls_.end() != it;) {
    if (it->second.urls.empty()) {
      it = topic_to_urls_.erase(it);
    } else {
      debug(__FILE__, "keeping topic subscription for topic {0}", it->first);
      ++it;
    }
  }

  debug(__FILE__, "topic to url mapping:");
  for (const auto &topic_url : topic_to_urls_) {
    publishRawInformationPerPackage(topic_url.first);
    debug(__FILE__, " topic {0}", topic_url.first);
    for (const auto &url : topic_url.second.urls) {
      debug(__FILE__, "  url {0}", url.first);
    }
  }

  info(__FILE__, "Finished updating {0}.", this->getName());
  return true;
}
std::vector<std::string>
RawDataProvider::parseJsonArgumentToRequesteUrls(std::vector<std::string> &user_args) const { // 1)
  // get the urls we have to subscribe to
  std::vector<std::string> requested_urls;
  Json::Reader json_reader;
  for (const auto &arg : user_args) {
    Json::Value parsed;
    json_reader.parse(arg, parsed, false);
    if (parsed.isMember("usedUris")) {
      if (parsed["usedUris"].isArray()) {
        for (Json::ArrayIndex i = 0; i < parsed["usedUris"].size(); ++i) {
          requested_urls.push_back(parsed["usedUris"][i].asString());
        }
      } else {
        requested_urls.push_back(parsed["usedUris"].asString());
      }
    }
  }
  // we want to subscribe only once, this is why we make the requested urls unique
  std::sort(requested_urls.begin(), requested_urls.end());

  requested_urls.erase(std::unique(requested_urls.begin(), requested_urls.end()), requested_urls.end());
  for (const auto &url : requested_urls) {
    debug(__FILE__, " requested url: {0}", url);
  }
  return requested_urls;
}

// 2)
// collect the needed topics for the new urls
// CreateTopicToUrlMapping()
// maps the topic (key) to the url it is needed for (value)
std::map<std::string, std::list<std::string>>
RawDataProvider::createTopicToUrlMapping(const std::vector<std::string> &requested_urls) {
  std::map<std::string, std::list<std::string>> topic_to_new_urls;
  for (const auto &url : requested_urls) {
    auto topics = plugin_signal_explorer_->getTopicsByUrl(url);
    if (topics.empty()) {
      warn(__FILE__, "Got empty reply from getTopicsByUrl for url {0}", url);
      continue;
    } else if (topics.size() != 1) {
      debug(__FILE__, "Got {0} topics for url {1}, only the first will be used", topics.size(), url);
    }
    topic_to_new_urls[topics[0]].push_back(url);
  }

  return topic_to_new_urls;
}

std::string RawDataProvider::SignalTypeToString(next::bridgesdk::plugin_addons::SignalType type) const {
  switch (type) {
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_STRUCT:
    return "struct";
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_BOOL:
    return "bool";
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_CHAR:
    return "char";
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT8:
    return "uint_8";
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT8:
    return "int_8";
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT16:
    return "uint_16";
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT16:
    return "int_16";
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT32:
    return "uint_32";
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT32:
    return "int_32";
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT64:
    return "uint_64";
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT64:
    return "int_64";
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_FLOAT:
    return "float";
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_DOUBLE:
    return "double";
  default:
    return "unkown";
  }
}

void RawDataProvider::convertToJsonValue(Json::Value &json,
                                         const next::bridgesdk::plugin_addons::PluginSignalDescription &topic_info,
                                         const next::bridgesdk::plugin_addons::SignalInfo &info) {
  json.clear();
  json["signal"]["type"] = SignalTypeToString(info.signal_type);
  json["signal"]["size"] = info.signal_size;
  json["signal"]["array_size"] = info.array_size;
  json["signal"]["offset"] = info.offset;
  json["package"]["cycleId"] = topic_info.cycle_id;
  json["package"]["instanceId"] = topic_info.instance_id;
  json["package"]["sourceId"] = topic_info.source_id;
  json["package"]["vaddr"] = topic_info.vaddr;
}

void RawDataProvider::publishRawInformationPerPackage(const std::string topic) {
  std::map<std::string, Json::Value> received_data;

  // make sure that no other data processing is started while this one isn't finished
  // also the update function has to be blocked
  debug(__FILE__, "received some data for topic {0}", topic);
  auto topic_url_it = topic_to_urls_.find(topic);
  if (topic_to_urls_.end() != topic_url_it) {
    for (auto &url_info : topic_url_it->second.urls) {
      const auto &url = url_info.first;
      if (url_info.second) {
        // info is already available from a previous run
        debug(__FILE__, "reusing interpretation info of {0}", url);
      } else {
        // info is not available, maybe it is also CAN data
        debug(__FILE__, "requesting interpretation info of {0}", url);
        const auto signal_info = plugin_signal_extractor_->GetInfo(url);
        if (signal_info.array_size == 0) {
          // the map might be empty or our url is unknown
          debug(__FILE__, " failed to get data interpretation info for {0}", url);
        } else {
          url_info.second = std::make_shared<next::bridgesdk::plugin_addons::SignalInfo>();
          *(url_info.second) = signal_info;
        }
      }
      auto info = plugin_signal_explorer_->getPackageDetailsByUrl(topic);
      // check if SignalInfo is available
      if (auto signal_Info = url_info.second; signal_Info != nullptr) {
        convertToJsonValue(received_data[url], info, *signal_Info);
      }
    }
    using namespace std::chrono;
    milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    prepareAndSend(static_cast<uint64_t>(ms.count()), received_data);
  }
}

void RawDataProvider::prepareAndSend(bridgesdk::time_t timestamp, std::map<std::string, Json::Value> &received_data) {

  std::string message_json;
  combineJsonStringFromReceivedData(timestamp, message_json, received_data);

  // TODO give here the "url" (we are sending for several urls in one message...)
  plugin_publisher_->sendResultData((uint8_t *)message_json.c_str(), message_json.size());
}

bool RawDataProvider::combineJsonStringFromReceivedData(
    unsigned long long int timestamp, std::string &result_msg,
    std::map<std::string, Json::Value> &json_list) { // preparing the JSON message
  Json::Value message;
  for (auto &entry : json_list) {
    auto split = split_string(entry.first, ".");
    Json::Value *v = &message["dataStream"]["signalRawDetails"];
    for (auto &s : split) {
      v = &(*v)[s];
    }
    *v = entry.second;
  }

  // send
  message["dataStream"]["timestamp"] = static_cast<Json::Value::UInt64>(timestamp);
  result_msg = Json::writeString(writer_builder_, message);
  debug(__FILE__, "sending {0}", result_msg);
  return true;
}

bool RawDataProvider::enterReset() { return true; }

bool RawDataProvider::exitReset() { return update(update_user_args_); }

} // namespace plugins
} // namespace next

plugin_create_t create_plugin(const char *path) { return new next::plugins::RawDataProvider(path); }

/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     signal_provider.cpp
 * @brief    Signal Provider Plugin
 *
 * See signal_provider.h.
 *
 **/

#include <algorithm>
#include <cassert>
#include <functional>

#include <next/plugins_utils/helper_functions.h>

#include "signal_provider.h"

namespace next {
namespace plugins {
using namespace next::plugins_utils;

SignalProvider::SignalProvider(const char *path) : next::bridgesdk::plugin::Plugin(path) {
  debug(__FILE__, "Hello system_SignalProviderPlugin");
  this->setVersion(std::string("0.0.1-alpha1"));
  this->setName(std::string("system_SignalProviderPlugin"));

  writer_builder_["commentStyle"] = "None";
  writer_builder_["indentation"] = "";
}

SignalProvider::~SignalProvider() {}

bool SignalProvider::init() {
  plugin_publisher_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataPublisher>(this);
  plugin_data_subscriber_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataSubscriber>(this);
  plugin_signal_explorer_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExplorer>(this);
  plugin_signal_extractor_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExtractor>(this);
  return true;
}

std::string SignalProvider::GetLowerLevelUrl(const std::string &topic, const std::string &url) const {
  if (topic.find(url) != std::string::npos) {
    return topic;
  }
  return url;
}

std::string FindCommonUrlPath(const std::string &topic, const std::string &url) {
  std::string cut_url = topic;
  if (url.find(topic) != std::string::npos) {
    return topic;
  }

  std::string cut = ".";
  size_t end;

  while (end = cut_url.rfind(cut), end != std::string::npos) {
    cut_url.erase(cut_url.begin() + (int)end, cut_url.end());
    if (url.find(cut_url) != std::string::npos) {
      return cut_url;
    }
  }
  return cut_url;
}

bool SignalProvider::update(std::vector<std::string> user_args) {
  // make sure that this function is only called by one thread at a time
  // also block the data processing function until topic/url registration is finished
  std::lock_guard<std::mutex> lck(processing_mtx_);

  update_user_args_ = user_args;

  // structure of this function:
  // 1) parse the user_args to get a list of the requested URLs
  // 2) collect the needed topics for this URLs
  // 3) update the topic subscription (subscribe to new topics, unsubscribe from no longer needed ones)

  for (const auto &arg : user_args) {
    debug(__FILE__, " update given arg: {0}", arg);
  }

  std::vector<std::string> requested_urls = ParseJsonRequest(user_args);
  std::vector<std::string> requested_tree_urls = GetDetailedUrlFromRequests(requested_urls);
  RemoveDuplicatedTopics(requested_tree_urls);
  for (const auto &url : requested_tree_urls) {
    debug(__FILE__, " requested package_urls: {0}", url);
  }

  std::vector<std::string> remove_trees;
  FilterTopicAndRequests(requested_tree_urls, remove_trees);

  if (requested_tree_urls.size() != 0) {
    for (auto missed_url : requested_tree_urls) {
      warn(__FILE__, " missed to connect a requested url {0} to a TreeExtractor.", missed_url);
    }
  }

  for (auto &remove_url : remove_trees) {
    auto topic_it = topic_to_details_.find(remove_url);
    if (topic_it != topic_to_details_.end()) {
      debug(__FILE__, " unsubscribing {0}", topic_it->first);
      plugin_data_subscriber_->unsubscribe(topic_it->second.topic_subscription_id);
      topic_to_details_.erase(topic_it);
    }
  }

  AddNewTreeExtractors();

  debug(__FILE__, "topic to url mapping:");
  for (const auto &topic_url : topic_to_details_) {
    debug(__FILE__, " topic {0}", topic_url.first);
    if (!topic_url.second.signalTree) {
      warn(__FILE__, "Empty TopicDetails for topic {0}", topic_url.first);
      continue;
    }
    for (const auto &url : topic_url.second.signalTree->signals_) {
      debug(__FILE__, "  url {0}", url.name);
    }
  }
  debug(__FILE__, "SignalProvider Update finished");
  return true;
}

std::vector<std::string> SignalProvider::GetDetailedUrlFromRequests(std::vector<std::string> &requested_urls) const {
  std::vector<std::string> requested_tree_urls;
  for (auto &url : requested_urls) {
    auto topics = plugin_signal_explorer_->getTopicsByUrl(url);
    for (const auto &topic : topics) {
      auto detailed_url = GetLowerLevelUrl(topic, url);
      requested_tree_urls.push_back(detailed_url);
    }
  }
  return requested_tree_urls;
}

void SignalProvider::AddNewTreeExtractors() {
  for (const auto &detail_temp : topic_to_details_) {
    const auto &root_tree_url = detail_temp.second.tree_url;
    auto topics = plugin_signal_explorer_->getTopicsByUrl(root_tree_url);
    if (topics.size() != 1) {
      error(__FILE__, " single url has multiple topics or none! Data corrupt");
      continue;
    }
    std::string topic = topics[0];

    if (topic_to_details_.end() != topic_to_details_.find(topic)) {
      auto id = topic_to_details_[topic].topic_subscription_id;
      if (id != 0) {
        plugin_data_subscriber_->unsubscribe(topic_to_details_[topic].topic_subscription_id);
      }
    } else {
      topic_to_details_[topic] = {};
      warn(__FILE__, " No TopicDetails found for topic. Should not happen!");
    }
    // we have to add one subscription
    topic_to_details_[topic].signalTree = std::make_shared<udex::struct_extractor::PackageTreeExtractor>(root_tree_url);
    debug(__FILE__, " new topic {0} for url {1}", topic, root_tree_url);
    const size_t id =
        plugin_data_subscriber_->subscribeUrl(topic, bridgesdk::DIRECT,
                                              std::bind(&SignalProvider::processData, this, std::placeholders::_1,
                                                        std::placeholders::_2, topic_to_details_[topic]));
    if (0 != id) {
      debug(__FILE__, "subscribed with subscription id {0} for topic {1}", id, topic);
      topic_to_details_[topic].topic_subscription_id = id;
    } else {
      warn(__FILE__, " failed to subscribe to topic {0} for url {1}", topic, root_tree_url);
      continue;
    }
  }
}

void SignalProvider::FilterTopicAndRequests(std::vector<std::string> &requested_tree_urls,
                                            std::vector<std::string> &remove_topics) {
  for (auto &topic_details : topic_to_details_) {
    bool signal_found_for_tree = false;
    signal_found_for_tree = FindAndRemoveAlreadyProvidedRequests(requested_tree_urls, topic_details.second);
    if (!signal_found_for_tree) {
      remove_topics.push_back(topic_details.first);
    }
  }
}

std::vector<std::string> SignalProvider::ParseJsonRequest(std::vector<std::string> &user_args) const {
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
  return requested_urls;
}

void SignalProvider::RemoveDuplicatedTopics(
    std::vector<std::string>
        &url_list) { // we want to subscribe only once, this is why we make the requested urls unique

  std::sort(url_list.begin(), url_list.end());
  url_list.erase(std::unique(url_list.begin(), url_list.end()), url_list.end());

  std::map<std::string, std::string> topics_to_urls_temp;
  for (auto &single_url : url_list) {
    auto topics = plugin_signal_explorer_->getTopicsByUrl(single_url);
    if (topics.size() != 1) {
      warn(__FILE__, " single url has multiple topics or none! Data corrupt");
      continue;
    }
    std::string current_topic = topics[0];
    auto current_url = topics_to_urls_temp.find(current_topic);
    std::string common_path;
    if (current_url == topics_to_urls_temp.end()) {
      topics_to_urls_temp.insert({topics[0], single_url});
      common_path = single_url;
    } else {
      common_path = FindCommonUrlPath(topics_to_urls_temp[current_topic], single_url); //
      // ignore paths were we only have a common path larger than a package url.
      // in detail we should check if the common path has a single package!
      if (plugin_signal_explorer_->getTopicsByUrl(common_path).size() != 1) {
        topics_to_urls_temp.insert({current_topic, single_url});
        common_path = single_url;
      } else {
        topics_to_urls_temp[current_topic] = common_path;
      }
    }
    // now add the url to the requeted trees
    auto found_details = topic_to_details_.find(current_topic);
    if (found_details == topic_to_details_.end()) {
      topic_to_details_[current_topic] = {};
    }
    topic_to_details_[current_topic].tree_url = common_path;
    topic_to_details_[current_topic].required_signals.push_back(single_url);
  }
  url_list.clear();
  for (auto &topic_to_url : topics_to_urls_temp) {
    url_list.push_back(topic_to_url.second);
  }
}

bool SignalProvider::FindAndRemoveAlreadyProvidedRequests(std::vector<std::string> &requested_urls,
                                                          TopicDetails &topic_details) const {
  bool signal_found_for_tree = false;
  for (auto request_url_it = requested_urls.begin(); requested_urls.end() != request_url_it;) {

    if (*request_url_it == topic_details.tree_url) {
      signal_found_for_tree = true;
      // we already handle this url
      debug(__FILE__, " not new url: {0}", *request_url_it);
      requested_urls.erase(request_url_it);
      break;
    }
    request_url_it++;
  }
  return signal_found_for_tree;
}

std::pair<std::string, Json::Value>
SignalProvider::FillJsonFromAnyValue(const next::udex::ValueUnion &value_union_type, const udex::SignalType &type,
                                     const std::string &url) const { // preparing the JSON message
  switch (type) {
  case udex::SignalType::SIGNAL_TYPE_CHAR:
    return {url, value_union_type.asChar};
  case udex::SignalType::SIGNAL_TYPE_UINT8:
    return {url, value_union_type.asUint8};
  case udex::SignalType::SIGNAL_TYPE_INT8:
    return {url, value_union_type.asInt8};
  case udex::SignalType::SIGNAL_TYPE_UINT16:
    return {url, value_union_type.asUint16};
  case udex::SignalType::SIGNAL_TYPE_INT16:
    return {url, value_union_type.asInt16};
  case udex::SignalType::SIGNAL_TYPE_UINT32:
    return {url, value_union_type.asUint32};
  case udex::SignalType::SIGNAL_TYPE_INT32:
    return {url, value_union_type.asInt32};
  case udex::SignalType::SIGNAL_TYPE_UINT64:
    return {url, value_union_type.asUint64};
  case udex::SignalType::SIGNAL_TYPE_INT64:
    return {url, value_union_type.asInt64};
  case udex::SignalType::SIGNAL_TYPE_FLOAT:
    return {url, value_union_type.asFloat};
  case udex::SignalType::SIGNAL_TYPE_DOUBLE:
    return {url, value_union_type.asDouble};
  case udex::SignalType::SIGNAL_TYPE_BOOL:
    return {url, value_union_type.asBool};
  case udex::SignalType::SIGNAL_TYPE_STRUCT:
    warn(__FILE__, " Signal type of URL {} is STRUCT, which is unsupported.", url);
    return {url, {}};
    // default:
    // nothing
  }
  return {url, {}};
}

void SignalProvider::processData([[maybe_unused]] const std::string topic,
                                 const next::bridgesdk::ChannelMessagePointers *data,
                                 const TopicDetails &topic_details) {
  // make sure we get valid pointers as input
  // afterwards extract the data for all of the needed urls of the provided topic
  if (nullptr != data && nullptr != data->data && 0 != data->size) {
    std::list<std::pair<std::string, Json::Value>> received_data;
    if (topic_details.required_signals.size() != 0) {
      topic_details.signalTree->SetMemory((const char *)data->data, data->size);
      std::string url;
      size_t leaf_array_index = 0;
      for (const auto &signal_url : topic_details.required_signals) {
        // check if signal is array on leaf level
        size_t dot_pos = signal_url.rfind(".");
        size_t open_bracket_pos = signal_url.rfind("[");
        size_t closing_bracket_pos = signal_url.rfind("]");
        if ((open_bracket_pos > dot_pos) && (open_bracket_pos != std::string::npos)) {
          url = signal_url.substr(0, open_bracket_pos);
          leaf_array_index = static_cast<size_t>(
              std::stoi(signal_url.substr(open_bracket_pos + 1, closing_bracket_pos - open_bracket_pos - 1)));
        } else {
          url = signal_url;
        }

        for (auto &signal : topic_details.signalTree->signals_) {
          if (signal.name != url) {
            continue;
          } else {
            size_t array_size = signal.value->ArraySize();
            next::udex::ValueUnion value_union_type;
            if (array_size > 1) {
              value_union_type = signal.value->Get(leaf_array_index);
            } else {
              value_union_type = signal.value->Get();
            }

            auto json_pair = FillJsonFromAnyValue(value_union_type, signal.value->GetTypeOfAnyValue(), signal_url);
            received_data.push_back(json_pair);
          }
        }
      }
    }
    prepareAndSend(data->time, received_data);
  } else {
    debug(__FILE__, "processData was called, but data pointers are invalid");
  }
}

void SignalProvider::prepareAndSend(bridgesdk::time_t timestamp,
                                    std::list<std::pair<std::string, Json::Value>> received_data_) {
  // preparing the JSON message
  Json::Value message;
  message["dataStream"]["timestamp"] = static_cast<Json::Value::UInt64>(timestamp);
  for (auto &entry : received_data_) {
    auto split = split_string(entry.first, ".");
    Json::Value *v = &message["dataStream"]["signalList"];
    for (auto &s : split) {
      v = &(*v)[s];
      // info(Json::writeString(writer_builder_, message));
    }
    *v = entry.second;
    // info(Json::writeString(writer_builder_, message));
  }
  // send
  std::string message_json = Json::writeString(writer_builder_, message);
  debug(__FILE__, "sending {0}", message_json);
  // TODO give here the "url" (we are sending for several urls in one message...)
  plugin_publisher_->sendResultData((uint8_t *)message_json.c_str(), message_json.size());
  // clear buffer
  received_data_.clear();
}

bool SignalProvider::enterReset() { return true; }

bool SignalProvider::exitReset() { return update(update_user_args_); }

} // namespace plugins
} // namespace next

plugin_create_t create_plugin(const char *path) { return new next::plugins::SignalProvider(path); }

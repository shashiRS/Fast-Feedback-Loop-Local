/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     signal_provider.h
 * @brief    Signal Provider Plugin
 *
 * This plugin gets the URLs from the signal provider, collects the data from the
 * inputs and provides it to the signal provider.
 *
 **/

#ifndef NEXT_PLUGINS_SIGNAL_PROVIDER_PLUGIN_H_
#define NEXT_PLUGINS_SIGNAL_PROVIDER_PLUGIN_H_

#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <utility>

#include <json/json.h>

#include <next/bridgesdk/plugin.h>
#include <next/bridgesdk/plugin_addons/plugin_datapublisher.hpp>
#include <next/bridgesdk/plugin_addons/plugin_datasubscriber.hpp>
#include <next/bridgesdk/plugin_addons/plugin_signal_explorer.hpp>
#include <next/bridgesdk/plugin_addons/plugin_signal_extractor.hpp>

#include <next/udex/struct_extractor/package_tree_extractor.hpp>

namespace next {
namespace plugins {

class SignalProvider : public next::bridgesdk::plugin::Plugin {
private:
  struct TopicDetails {
    size_t topic_subscription_id{}; // topic subscription id
    std::shared_ptr<next::udex::struct_extractor::PackageTreeExtractor> signalTree;
    std::string tree_url;
    std::list<std::string> required_signals;
  };

public:
  SignalProvider(const char *path);
  virtual ~SignalProvider();

  bool init() override;
  bool update(std::vector<std::string> user_args) override;

  void processData(const std::string topic, const next::bridgesdk::ChannelMessagePointers *,
                   const TopicDetails &topic_details);

  virtual bool enterReset() override;
  virtual bool exitReset() override;

private:
  // prepares the json message and sends it for given url
  void prepareAndSend(bridgesdk::time_t timestamp, std::list<std::pair<std::string, Json::Value>> received_data_);

  // this map holds the topics as keys and the details of the needed urls as values
  std::map<std::string, TopicDetails> topic_to_details_;

  // mutex for locking data processing and topic/url registration
  std::mutex processing_mtx_;

  Json::StreamWriterBuilder writer_builder_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataPublisher> plugin_publisher_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataSubscriber> plugin_data_subscriber_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExplorer> plugin_signal_explorer_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExtractor> plugin_signal_extractor_;

  std::vector<std::string> update_user_args_;

  std::string GetLowerLevelUrl(const std::string &topic, const std::string &url) const;
  std::string GetUpperLevelUrl(const std::string &topic, const std::string &url) const;

  bool FindAndRemoveAlreadyProvidedRequests(std::vector<std::string> &requested_urls,
                                            TopicDetails &topic_details) const;
  std::vector<std::string> ParseJsonRequest(std::vector<std::string> &user_args) const;
  void RemoveDuplicatedTopics(std::vector<std::string> &url_list);
  void FilterTopicAndRequests(std::vector<std::string> &requested_tree_urls, std::vector<std::string> &remove_topics);
  void AddNewTreeExtractors();
  std::vector<std::string> GetDetailedUrlFromRequests(std::vector<std::string> &requested_urls) const;

  std::pair<std::string, Json::Value> FillJsonFromAnyValue(const next::udex::ValueUnion &value_union_type,
                                                           const udex::SignalType &type, const std::string &url) const;
};

} // namespace plugins
} // namespace next

extern "C" NEXTPLUGINSHARED_EXPORT plugin_create_t create_plugin(const char *path);

#endif // NEXT_PLUGINS_SIGNAL_PROVIDER_PLUGIN_H_

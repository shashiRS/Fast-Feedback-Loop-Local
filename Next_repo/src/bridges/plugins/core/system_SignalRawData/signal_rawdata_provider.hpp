/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     signal_rawdata_provider.h
 * @brief    Signal Rawdata Provider Plugin
 *
 * This plugin gets the URLs from the GUI and provides details on the raw data
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

namespace next {
namespace plugins {

class RawDataProvider : public next::bridgesdk::plugin::Plugin {
public:
  RawDataProvider(const char *path);
  ~RawDataProvider();

  bool init() override;
  bool update(std::vector<std::string> user_args) override;

  virtual bool enterReset() override;
  virtual bool exitReset() override;

private:
  void publishRawInformationPerPackage(const std::string topic);

  struct TopicDetails {
    size_t topic_subscription_id{}; // topic subscription id
    std::list<std::pair<std::string, std::shared_ptr<next::bridgesdk::plugin_addons::SignalInfo>>> urls;
  };

  void convertToJsonValue(Json::Value &json, const next::bridgesdk::plugin_addons::PluginSignalDescription &topic_info,
                          const next::bridgesdk::plugin_addons::SignalInfo &info);

  // prepares the json message and sends it for given url
  void prepareAndSend(bridgesdk::time_t timestamp, std::map<std::string, Json::Value> &received_data);

  bool combineJsonStringFromReceivedData(unsigned long long int timestamp, std::string &result_msg,
                                         std::map<std::string, Json::Value> &json_list);

  std::string SignalTypeToString(next::bridgesdk::plugin_addons::SignalType type) const;
  // this map holds the topics as keys and the details of the needed urls as values
  std::map<std::string, TopicDetails> topic_to_urls_;

  Json::StreamWriterBuilder writer_builder_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataPublisher> plugin_publisher_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExplorer> plugin_signal_explorer_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExtractor> plugin_signal_extractor_;

  bool init_succesfull_{false};
  std::vector<std::string> update_user_args_;

public:
  friend class RawDataProviderTester;
  std::vector<std::string> parseJsonArgumentToRequesteUrls(std::vector<std::string> &user_args) const;
  std::map<std::string, std::list<std::string>> createTopicToUrlMapping(const std::vector<std::string> &requested_urls);
};

} // namespace plugins
} // namespace next

extern "C" NEXTPLUGINSHARED_EXPORT plugin_create_t create_plugin(const char *path);

#endif // NEXT_PLUGINS_SIGNAL_PROVIDER_PLUGIN_H_

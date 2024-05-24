/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     signal_exporter.h
 * @brief    Signal Exporter Plugin
 *
 * This plugin exports selected signals to a .bsig or .csv file
 *
 **/

#ifndef NEXT_PLUGINS_SIGNAL_EXPORTER_H_
#define NEXT_PLUGINS_SIGNAL_EXPORTER_H_

#include <ctype.h>
#include <chrono>
#include <cmath>
#include <ctime>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <string>

#include <json/json.h>

#include <ctk_gex/ctk_gex.h>

#include <next/bridgesdk/plugin.h>
#include <next/bridgesdk/plugin_addons/plugin_datasubscriber.hpp>
#include <next/bridgesdk/plugin_addons/plugin_signal_explorer.hpp>
#include <next/udex/struct_extractor/package_tree_extractor.hpp>

namespace next {
namespace plugins {

constexpr const char *kPackage_Timestamp = "MTS.Package.TimeStamp";

enum eTRIGGER_TYPE {
  signal_based = 0,
  // cycle_based = 1,
  cycle_independent = 2,
};

struct SignalInfo {
  udex::SignalType signal_type;
  size_t signal_size;
  size_t array_size;
};

enum class eFILE_TYPE { bsig = 0, csv = 1 };

class SignalExporter : public next::bridgesdk::plugin::Plugin {
public:
  SignalExporter(const char *path);
  virtual ~SignalExporter();

  bool init() override;
  bool enterReset() override;
  bool exitReset() override;
  bool update(std::vector<std::string> user_input) override;
  bool addConfig(const next::bridgesdk::plugin_config_t &config) override;

  struct TopicPair {
    std::string url;
    std::string topic;
  };

private:
  struct TopicDetails {
    std::shared_ptr<next::udex::struct_extractor::PackageTreeExtractor> signalTree;
    std::string tree_url;
    std::list<std::string> required_signals;
  };

  void subscribeToTopic(const std::string &topic);
  void addSignalToSignalsList(const TopicPair &url_topic);
  void purgeTreeExtractorFromNotNeededSignals();
  void addValidSubscriptionsToTreeAndSubscribers(const std::vector<TopicPair> &valid_signal_urls);

  void parseFolderAndFile(const std::string &file_path, std::string &folder_name, std::string &file_name);
  void createFolder(const std::string &folder_name);
  void replaceVariables(std::string &file_name);
  void initFileWriter(const std::string &file_formats);
  void replaceFileIndex(std::string &file_name, const std::string &file_format);
  void createInitFile(const std::string &file_path);
  void prepareOutputFile(const std::string &file_path, const std::string &file_format);

  bool handleStartExport(const Json::Value &root);
  void handleStopExport();
  void configure();

  void save_config(const std::vector<std::string> &user_input);

  void callBackInputPort(std::string const topic, const bridgesdk::ChannelMessagePointers *data);

  std::shared_ptr<next::bridgesdk::plugin_addons::PluginDataSubscriber> plugin_data_subscriber_;
  std::shared_ptr<next::bridgesdk::plugin_addons::PluginSignalExplorer> plugin_signal_explorer_;

  std::vector<std::string> configuration_;
  bool partialy_configured_ = false;
  bool configured_ = false;

  std::unique_ptr<ctkGex> generic_exporter_ptr_;
  std::mutex file_writer_lock_;

  // vector is used for writer InitSignalData and ProcessSignals
  std::vector<ctkGexSignalData> signals_list_;
  std::unordered_map<std::string, ctkGexSignalData> map_gex_signal_to_url_;

  std::vector<std::string> subscribed_topics_;
  std::map<std::string, TopicDetails> topic_tree_extractor_map_;

  std::vector<size_t> subscription_ids_;

  eTRIGGER_TYPE triggering_type_;
  std::string trigger_topic_;

  std::atomic<bool> file_open_{false};
  std::atomic<bool> exit_{false};

  std::string exporter_name_;

public:
  friend class SignalExporterTester;
  std::string removeArrayIndexAtLeafLevel(const std::string &signal_url) const;
};

} // namespace plugins
} // namespace next

extern "C" NEXTPLUGINSHARED_EXPORT plugin_create_t create_plugin(const char *path);

// const std::string GUI_input = R"({
//  "event": "UserPressedStartSignalExportingButton",
//  "listofURLs": [
//    "ARS5xx.AlgoVehCycle.VehDyn.uiVersionNumber",
//    "ARS5xx.AlgoVehCycle.VehDyn.sSigHeader.uiTimeStamp",
//    "ARS5xx.AlgoVehCycle.VehDyn.sSigHeader.uiMeasurementCounter",
//    "ARS5xx.AlgoVehCycle.VehDyn.sSigHeader.uiCycleCounter",
//    "ARS5xx.AlgoVehCycle.VehDyn.sSigHeader.eSigStatus",
//    "ARS5xx.AlgoVehCycle.VehDyn.sSigHeader.a_reserve",
//    "ADC4xx.IPVDY.VehDyn_t.uiVersionNumber",
//    "ADC4xx.IPVDY.VehDyn_t.padding",
//    "ADC4xx.IPVDY.VehDyn_t.sSigHeader.uiTimeStamp",
//    "ADC4xx.IPVDY.VehDyn_t.sSigHeader.uiMeasurementCounter",
//    "ADC4xx.IPVDY.VehDyn_t.sSigHeader.uiCycleCounter",
//    "ADC4xx.IPVDY.VehDyn_t.sSigHeader.eSigStatus",
//    "ADC4xx.IPVDY.VehDyn_t.sSigHeader.a_reserve"
//  ],
//  "triggerURL": "ADC4xx.IPVDY.VehDyn_t.sSigHeader.uiMeasurementCounter",
//  "fileFormat": ".bsig",
//  "outputFolder": "D:\\whatever_folder"
//})";

#endif // NEXT_PLUGINS_SIGNAL_EXPORTER_H_

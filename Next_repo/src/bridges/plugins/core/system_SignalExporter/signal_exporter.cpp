/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     next_exporter.cpp
 * @brief    Next Exporter Plugin
 *
 * See next_exporter.h.
 *
 **/

#include "signal_exporter.h"

#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include <next/appsupport/next_status_code/next_status_code.hpp>

#include <next/plugins_utils/helper_functions.h>

#include <boost/filesystem.hpp>
#include <ctime>

namespace next {
namespace plugins {

std::map<std::string, eFILE_TYPE> file_type_map{{".bsig", eFILE_TYPE::bsig}, {".csv", eFILE_TYPE::csv}};

std::map<udex::SignalType, ctkGexSignalData::eDATA_TYPE> type_map{
    {udex::SignalType::SIGNAL_TYPE_CHAR, ctkGexSignalData::dtChar},
    {udex::SignalType::SIGNAL_TYPE_UINT8, ctkGexSignalData::dtByte},
    {udex::SignalType::SIGNAL_TYPE_INT8, ctkGexSignalData::dtChar},
    {udex::SignalType::SIGNAL_TYPE_UINT16, ctkGexSignalData::dtWord},
    {udex::SignalType::SIGNAL_TYPE_INT16, ctkGexSignalData::dtShort},
    {udex::SignalType::SIGNAL_TYPE_UINT32, ctkGexSignalData::dtDWord},
    {udex::SignalType::SIGNAL_TYPE_INT32, ctkGexSignalData::dtLong},
    {udex::SignalType::SIGNAL_TYPE_UINT64, ctkGexSignalData::dtQWord},
    {udex::SignalType::SIGNAL_TYPE_INT64, ctkGexSignalData::dtInt64},
    {udex::SignalType::SIGNAL_TYPE_FLOAT, ctkGexSignalData::dtFloat},
    {udex::SignalType::SIGNAL_TYPE_DOUBLE, ctkGexSignalData::dtDouble}};

std::wstring inline WidenString(const std::string &str) {
  std::wstring ret;

  if (!str.empty()) {
    ret.resize(str.length());

    typedef std::ctype<wchar_t> CT;
    CT const &ct = std::use_facet<CT>(std::locale());

    ct.widen(&str[0], &*str.begin() + str.size(), &ret[0]);
  }

  return ret;
}

SignalExporter::SignalExporter(const char *path)
    : next::bridgesdk::plugin::Plugin(path), generic_exporter_ptr_(nullptr),
      triggering_type_(eTRIGGER_TYPE::signal_based) {
  debug(__FILE__, "Hello system_Signal-Exporter plugin");

  this->setVersion(std::string("0.0.1"));
  this->setName(std::string("system_Signal-Exporter"));
}

SignalExporter::~SignalExporter() {
  handleStopExport();
  debug(__FILE__, "Goodbye Signal Exporter");
}

bool SignalExporter::init() {
  plugin_data_subscriber_ = std::make_shared<next::bridgesdk::plugin_addons::PluginDataSubscriber>(this);
  plugin_signal_explorer_ = std::make_shared<next::bridgesdk::plugin_addons::PluginSignalExplorer>(this);
  return true;
}

bool SignalExporter::enterReset() {
  if ((!configured_ || partialy_configured_) && !configuration_.empty()) {
    configure();
  }
  return configured_;
}

bool SignalExporter::exitReset() {
  // so far we are doing nothing here
  return true;
}

void SignalExporter::subscribeToTopic(const std::string &topic) {
  // if it's not already susbscribed to from other SignalURL, subscribe to topic
  if (std::find(subscribed_topics_.begin(), subscribed_topics_.end(), topic) == subscribed_topics_.end()) {
    auto bf = std::bind(&SignalExporter::callBackInputPort, this, std::placeholders::_1, std::placeholders::_2);
    size_t subscribtion_id = plugin_data_subscriber_->subscribeUrl(topic, bridgesdk::SubscriptionType::DIRECT, bf);
    subscribed_topics_.push_back(topic);
    subscription_ids_.push_back(subscribtion_id);
  }
}

void SignalExporter::addSignalToSignalsList(const TopicPair &url_topic) {
  std::string url = removeArrayIndexAtLeafLevel(url_topic.url);
  if (map_gex_signal_to_url_.find(url) != map_gex_signal_to_url_.end()) {
    return;
  }

  SignalInfo sig_info{udex::SignalType::SIGNAL_TYPE_UINT8, 0, 0};

  if (url == kPackage_Timestamp) {
    sig_info.signal_type = udex::SignalType::SIGNAL_TYPE_UINT64;
    sig_info.array_size = 1;
    sig_info.signal_size = 8;
  } else {

    for (auto &signal : topic_tree_extractor_map_[url_topic.topic].signalTree->signals_) {
      if (signal.name == url) {
        sig_info.array_size = signal.value->ArraySize();
        sig_info.signal_type = signal.value->GetTypeOfAnyValue();
        sig_info.signal_size = next::udex::any_value_size_map.at(signal.value->GetTypeOfAnyValue());
        break;
      }
    }
  }

  if (sig_info.signal_size != 0) {
    if (url == kPackage_Timestamp) {
      // ignore the MTS timestamps for tree extractors
    } else {
      topic_tree_extractor_map_[url_topic.topic].required_signals.push_back(url);
    }
    auto DataType = sig_info.signal_type;
    auto ArrayLength = sig_info.array_size;
    auto DataSize = sig_info.signal_size;

    ctkGexSignalData signal;

    signal.m_ssSignalURL = WidenString(url);
    signal.m_DataType = type_map[DataType];
    signal.m_nArrayLength = static_cast<uint32_t>(ArrayLength);
    signal.m_nDataSize = static_cast<uint32_t>(DataSize * ArrayLength);
    if (!signal.m_pData) {
      signal.m_pData = new (std::nothrow) uint8_t[signal.m_nDataSize]();
    }

    signals_list_.push_back(signal);
    map_gex_signal_to_url_.insert(std::make_pair(url, signal));
  }
}

std::string SignalExporter::removeArrayIndexAtLeafLevel(const std::string &signal_url) const {
  std::string url = signal_url;
  // check for arrays on leaf level
  size_t dot_pos = signal_url.rfind(".");
  size_t open_bracket_pos = signal_url.rfind("[");
  if ((open_bracket_pos > dot_pos) && (open_bracket_pos != std::string::npos)) {
    url = signal_url.substr(0, open_bracket_pos);
  }
  return url;
}

void SignalExporter::parseFolderAndFile(const std::string &file_path, std::string &folder_name,
                                        std::string &file_name) {
  if (file_path.empty()) {
    warn(__FILE__, "Output folder/file not set, current folder will be used, default file name");
    return;
  }

  auto temp_path = file_path;
  std::replace(temp_path.begin(), temp_path.end(), '\\', '/');
  boost::filesystem::path dir_path = temp_path;

  if (dir_path.has_extension()) {
    folder_name = dir_path.parent_path().string();
    file_name = dir_path.stem().string();
  } else {
    folder_name = dir_path.string();
  }
}

void SignalExporter::createFolder(const std::string &folder_name) {
  if (!boost::filesystem::exists(folder_name)) {
    boost::filesystem::create_directories(folder_name);
  }
}

void SignalExporter::replaceVariables(std::string &file_name) {
  auto now = std::chrono::system_clock::now();
  auto current_time = std::chrono::system_clock::to_time_t(now);
  NEXT_DISABLE_DEPRECATED_WARNING
  auto gmt_time = std::gmtime(&current_time);
  NEXT_RESTORE_WARNINGS

  if (size_t datePos = file_name.find("$DATE$"); datePos != std::string::npos) {
    char dateBuffer[12]; // DYYYY_MM_DD
    std::strftime(dateBuffer, sizeof(dateBuffer), "D%Y_%m_%d", gmt_time);
    file_name.replace(datePos, 6, dateBuffer);
  }

  if (size_t timePos = file_name.find("$TIME$"); timePos != std::string::npos) {
    char timeBuffer[10]; // THH_MM_SS
    std::strftime(timeBuffer, sizeof(timeBuffer), "T%H_%M_%S", gmt_time);
    file_name.replace(timePos, 6, timeBuffer);
  }
  cout << file_name.find("$INPUT$");
  cout <<"^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^";
  if (size_t inputPos = file_name.find("$INPUT$"); inputPos != std::string::npos) {
    auto config_client = next::appsupport::ConfigClient::getConfig();

    std::string recording_path =
        config_client->get_string(next::appsupport::configkey::player::getRecordingPathKey(), "");
    std::string rec_folder_name = "";
    std::string rec_file_name = "";
    parseFolderAndFile(recording_path, rec_folder_name, rec_file_name);

    file_name.replace(inputPos, 7, rec_file_name);
  }
}

void SignalExporter::initFileWriter(const std::string &file_format) {
  std::lock_guard<std::mutex> lock_guard(file_writer_lock_);

  if (generic_exporter_ptr_) {
    generic_exporter_ptr_->CloseFile();
  }

  switch (file_type_map[file_format]) {
  case eFILE_TYPE::bsig:
    generic_exporter_ptr_ = std::make_unique<ctkGex>(ctkGexType_t::ctkGexType_BSIG3);
    break;
  case eFILE_TYPE::csv:
    generic_exporter_ptr_ = std::make_unique<ctkGex>(ctkGexType_t::ctkGexType_CSV);
    break;
  default:
    next::appsupport::NextStatusCode::PublishStatusCode(
        exporter_name_, next::appsupport::StatusCode::ERR_EXPORTER_FILE_FORMAT_NOT_SUPPORTED,
        "Specified file format invalid.");
    break;
  }
}

void SignalExporter::replaceFileIndex(std::string &file_name, const std::string &file_format) {
  if (size_t fileIndexPos = file_name.find("$FILE_INDEX$"); fileIndexPos != std::string::npos) {
    int index = 0;
    do {
      std::string temp_str = file_name;
      temp_str.replace(fileIndexPos, 12, std::to_string(index));

      // add extension
      std::string fileExtended = temp_str + file_format;

      if (!boost::filesystem::exists(fileExtended)) {
        // found first file + index that does not exist
        file_name = temp_str;
        break;
      }

      index++;
    } while (true);
  }
}

void SignalExporter::createInitFile(const std::string &file_path) {
  std::lock_guard<std::mutex> lock_guard(file_writer_lock_);

  std::wstring w_file_path = WidenString(file_path);

  if (generic_exporter_ptr_) {
    // creates file in memory
    if (generic_exporter_ptr_->CreateFile(w_file_path.c_str()) != CTK_ERR_OK) {
      error(__FILE__, "Create File failed");
    }

    // init signals and write file on disk
    if (generic_exporter_ptr_->InitSignalData(signals_list_.data(), signals_list_.size()) != CTK_ERR_OK) {
      error(__FILE__, "Signal list initialization filed");
    }
  }
}

void SignalExporter::prepareOutputFile(const std::string &file_path, const std::string &file_format) {
  std::string folder_name = ".";
  std::string file_name = "exported_file__$DATE$_$TIME$";

  parseFolderAndFile(file_path, folder_name, file_name);

  createFolder(folder_name);

  replaceVariables(file_name);

  initFileWriter(file_format);

  std::string formated_file_path = folder_name + "/" + file_name;
  replaceFileIndex(formated_file_path, file_format);
  createInitFile(formated_file_path);
}

bool SignalExporter::handleStartExport(const Json::Value &root) {
  auto file_format = root["fileFormat"].asCString();
  if (auto file_type = file_type_map.find(file_format); file_type == file_type_map.end()) {
    return false;
  }

  info(__FILE__, "== SignalExporter starting ==");

  setSynchronizationMode(true);
  setPackageDropMode(false);

  // count valid signals and add them in map
  std::vector<TopicPair> valid_signal_urls;
  for (auto &url : root["listofURLs"]) {
    auto signalURL = url.asString();
    auto topic = plugin_signal_explorer_->getTopicsByUrl(signalURL);
    if (topic.empty()) {
      continue;
    }
    valid_signal_urls.push_back({signalURL, topic[0]});
  }

  // add the MTS.Package.TimeStamp as a signal, always first
  addSignalToSignalsList(TopicPair{kPackage_Timestamp, kPackage_Timestamp});

  // go though the URL list and subscribe to the according topic
  addValidSubscriptionsToTreeAndSubscribers(valid_signal_urls);

  purgeTreeExtractorFromNotNeededSignals();
  // set trigger url
  std::string triggerURL = root["triggerURL"].asCString();
  auto trigger_topic = plugin_signal_explorer_->getTopicsByUrl(triggerURL);
  if (trigger_topic.empty()) {
    next::appsupport::NextStatusCode::PublishStatusCode(exporter_name_,
                                                        next::appsupport::StatusCode::ERR_EXPORTER_TRIGGER_NOT_FOUND,
                                                        "Trigger not set. Export not possible.");
    // trigger is missing, cleanup everything to avoid memory leaks
    handleStopExport();
    return false;
  }

  trigger_topic_ = trigger_topic[0];
  debug(__FILE__, "== Trigger set to {0} ==", trigger_topic_);

  std::string output_folder = root["outputFolder"].asCString();
  prepareOutputFile(output_folder, file_format);

  if (subscription_ids_.empty()) {
    next::appsupport::NextStatusCode::PublishStatusCode(exporter_name_,
                                                        next::appsupport::StatusCode::ERR_EXPORTER_NO_SUBSCRIPTION,
                                                        "No subscription found. Exporting stopped");

    // nothing was subscribed, cleanup everything to avoid memory leaks
    handleStopExport();
    return false;
  }

  partialy_configured_ = valid_signal_urls.size() != root["listofURLs"].size();
  if (partialy_configured_) {
    std::string warningMessage =
        fmt::format("== Partial subscribe (some signals not available), {0}/{1} signals ==", valid_signal_urls.size(),
                    root["listofURLs"].size());

    next::appsupport::NextStatusCode::PublishStatusCode(
        exporter_name_, next::appsupport::StatusCode::WARN_EXPORTER_PARTIAL_SUBSCRIPTION, warningMessage);
  } else {
    info(__FILE__, "== Full subscribe, {0} signals ==", valid_signal_urls.size());
    next::appsupport::NextStatusCode::PublishStatusCode(
        exporter_name_, next::appsupport::StatusCode::SUCC_FULL_SUBSCRIPTION, "Exporting started");
  }

  return true;
}

void SignalExporter::addValidSubscriptionsToTreeAndSubscribers(const std::vector<TopicPair> &valid_signal_urls) {
  for (auto &signal : valid_signal_urls) {
    // create tree extractor for topic if not already created
    if (topic_tree_extractor_map_.find(signal.topic) == topic_tree_extractor_map_.end()) {
      debug(__FILE__, "== Creating PackageTreeExtractor for topic {0} ==", signal.topic);
      TopicDetails topicDetails{std::make_shared<udex::struct_extractor::PackageTreeExtractor>(signal.topic),
                                signal.topic, std::list<std::string>{}};
      topic_tree_extractor_map_.insert(std::make_pair(signal.topic, topicDetails));
    }

    addSignalToSignalsList(signal);

    subscribeToTopic(signal.topic);
  }
}

void SignalExporter::handleStopExport() {
  for (auto subscribtion_id : subscription_ids_) {
    plugin_data_subscriber_->unsubscribe(subscribtion_id);
  }
  subscription_ids_.clear();

  std::lock_guard<std::mutex> lock_guard(file_writer_lock_);

  if (generic_exporter_ptr_) {
    generic_exporter_ptr_->CloseFile();
  }

  for (auto &signal : signals_list_) {
    delete[](uint8_t *) signal.m_pData;
  }
  signals_list_.clear();

  map_gex_signal_to_url_.clear();

  subscribed_topics_.clear();
  topic_tree_extractor_map_.clear();

  info(__FILE__, "== SignalExporter stopped ==");
  next::appsupport::NextStatusCode::PublishStatusCode(
      exporter_name_, next::appsupport::StatusCode::SUCC_EXPORTER_STOPPED, "Exporting stopped");
}

void SignalExporter::configure() {
  debug(__FILE__, "configure() ...");

  NEXT_DISABLE_SPECIFIC_WARNING_WINDOWS(4996)
  Json::Value root;
  Json::Reader reader;
  if (configuration_.size() == 1) {
    reader.parse(configuration_[0], root);
  } else {
    for (auto const &us_in : configuration_) {
      Json::Value v_tmp;
      reader.parse(us_in, v_tmp);
      auto mem_name{v_tmp.getMemberNames()[0]};
      root[mem_name] = v_tmp[mem_name];
    }
  }
  NEXT_RESTORE_WARNINGS_WINDOWS

  if (root["event"] == "UserPressedStopSignalExportingButton") {
    handleStopExport();
    configured_ = false;
  } else if (root["event"] == "UserPressedStartSignalExportingButton") {
    configured_ = handleStartExport(root);
  } else {
    next::appsupport::NextStatusCode::PublishStatusCode(
        exporter_name_, next::appsupport::StatusCode::ERR_EXPORTER_CONFIGURATION_NOT_SUPPORTED,
        "Configuration event not supported");
  }
}

void SignalExporter::save_config(const std::vector<std::string> &user_input) {
  // TODO instead of saving all the config, only adapt relevant info or properties that are different
  configuration_ = user_input;
}

bool SignalExporter::update(std::vector<std::string> user_input) {
  if (user_input.empty())
    return true;

  save_config(user_input);

  // try to configure with lates received config
  configure();

  return configured_;
}

bool SignalExporter::addConfig(const next::bridgesdk::plugin_config_t &config) {
  if (!next::plugins_utils::GetValueFromPluginConfig(config, "DATA_VIEW", exporter_name_)) {
    warn(__FILE__, "SignalExporter wrong data view mapping name: DATA_VIEW");
    return false;
  }

  return true;
}

void SignalExporter::purgeTreeExtractorFromNotNeededSignals() {
  for (auto topic_detail : topic_tree_extractor_map_) {
    auto &signal_list = topic_detail.second.signalTree->signals_;
    auto &required_signal_list = topic_detail.second.required_signals;
    auto signal_it = signal_list.begin();
    while (signal_it != signal_list.end()) {
      bool signal_needed = false;

      for (auto &required_signal : required_signal_list) {
        if (required_signal.find(signal_it->name) != std::string::npos) {
          signal_needed = true;
          break;
        }
      }

      if (!signal_needed) {
        signal_it = signal_list.erase(signal_it);
      } else {
        ++signal_it;
      }
    }
  }
}

void SignalExporter::callBackInputPort(std::string const topic, const bridgesdk::ChannelMessagePointers *data) {
  if (data != nullptr && data->data != nullptr && data->size != 0) {
    if (topic_tree_extractor_map_.count(topic) != 0) {

      topic_tree_extractor_map_[topic].signalTree->SetMemory((const char *)data->data, data->size);
      for (const auto &signal : topic_tree_extractor_map_[topic].signalTree->signals_) {
        size_t array_size = signal.value->ArraySize();
        size_t data_size = next::udex::any_value_size_map.at(signal.value->GetTypeOfAnyValue()) * array_size;
        size_t max_size;
        void *tmp = signal.value->GetAddress(max_size);

        if (data_size > max_size) {
          error(__FILE__, "Could not process signals");
        } else {
          memcpy(map_gex_signal_to_url_[signal.name].m_pData, tmp, data_size);
        }
      }
    }
  }

  switch (triggering_type_) {
  case eTRIGGER_TYPE::signal_based: {
    if (topic == trigger_topic_) {
      // MTS.Package.TimeStamp from package containing trigger signal
      memcpy(map_gex_signal_to_url_[kPackage_Timestamp].m_pData, &data->time,
             map_gex_signal_to_url_[kPackage_Timestamp].m_nDataSize);
      if (generic_exporter_ptr_) {
        std::lock_guard<std::mutex> lock_guard(file_writer_lock_);
        if (generic_exporter_ptr_->ProcessSignals(signals_list_.data(), signals_list_.size()) != CTK_ERR_OK) {
          error(__FILE__, "Could not process signals");
        }
      }
    }
    break;
  }

  // TODO
  // this is not cycle based, when that specific device sends data, cycle triggering needs to be discussed
  // case eTRIGGER_TYPE::cycle_based : {
  //  if (topic == "ADC4xx.IPVDY.VehDyn_t")
  //  {
  //    generic_exporter.ProcessSignals(signals_list, generic_exporter_size);
  //  }
  //  break;
  //}

  // this is not really cycle independent, as it's not based on cycles
  // more like for every group update, needs to be discussed after cycle triggering
  case eTRIGGER_TYPE::cycle_independent: {
    if (generic_exporter_ptr_) {
      // MTS.Package.TimeStamp from current package
      memcpy(map_gex_signal_to_url_[kPackage_Timestamp].m_pData, &data->time,
             map_gex_signal_to_url_[kPackage_Timestamp].m_nDataSize);
      std::lock_guard<std::mutex> lock_guard(file_writer_lock_);
      if (generic_exporter_ptr_->ProcessSignals(signals_list_.data(), signals_list_.size()) != CTK_ERR_OK) {
        error(__FILE__, "Could not process signal");
      }
    }
    break;
  }

  default:
    if (generic_exporter_ptr_) {
      // MTS.Package.TimeStamp from current package
      memcpy(map_gex_signal_to_url_[kPackage_Timestamp].m_pData, &data->time,
             map_gex_signal_to_url_[kPackage_Timestamp].m_nDataSize);
      std::lock_guard<std::mutex> lock_guard(file_writer_lock_);
      if (generic_exporter_ptr_->ProcessSignals(signals_list_.data(), signals_list_.size()) != CTK_ERR_OK) {
        error(__FILE__, "Could not process signal");
      }
    }
    break;
  }
}

} // namespace plugins
} // namespace next

plugin_create_t create_plugin(const char *path) { return new next::plugins::SignalExporter(path); }

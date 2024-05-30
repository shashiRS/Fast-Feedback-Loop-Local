/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     next_inport.cpp
 * @brief    provides inport functions and holds respective data
 *
 */
#include <next/data_types.hpp>
#include <next/next.hpp>
#include <next_inport.hpp>

namespace next {
namespace inport {

void InportConfig::SetValuesToConfig(const ComponentInputPortConfig &it_input_port, const std::string component_name) {
  config::getConfig()->put(appsupport::configkey::getInportVersionKey(component_name, it_input_port.port_name),
                           (int)it_input_port.port_version_number);

  config::getConfig()->put(appsupport::configkey::getInportSizeKey(component_name, it_input_port.port_name),
                           (int)it_input_port.port_size);
  config::getConfig()->put(appsupport::configkey::getInportDataTypeKey(component_name, it_input_port.port_name),
                           it_input_port.data_type_name);

  addCompositionUrlsFromGlueCode(component_name, it_input_port);
}

void InportConfig::addCompositionUrlsFromGlueCode(const std::string &component_name,
                                                  const next::ComponentInputPortConfig &it_input_port) {
  std::string composition_url_key =
      appsupport::configkey::getCompositionPortUrlKey(component_name, it_input_port.port_name);
  // set default composition values if not set
  auto port_connections = config::getConfig()->getStringList(composition_url_key, {}, true);
  size_t index_last_element = port_connections.size();
  if (it_input_port.composition_connections.size() > 0) {
    for (auto &composition_inport : it_input_port.composition_connections) {
      // TODO if prefix is settable get this from config
      std::string prefix = (composition_inport.find("SIM VFB.") != 0 && composition_inport != "") ? "SIM VFB." : "";
      std::string port_composition_connection = (prefix + composition_inport);
      if (std::find(port_connections.begin(), port_connections.end(), port_composition_connection) ==
          port_connections.end()) {
        config::getConfig()->put((composition_url_key + "[" + std::to_string(index_last_element) + "]"),
                                 port_composition_connection);
        index_last_element++;
        next::sdk::logger::info(__FILE__, "Put URL {} into config from composition", composition_inport);
      }
    }
  }
}

void InportConfig::setActivePortInCompositionConfiguration(const std::string & /*component_name*/,
                                                           const std::string & /*port_name*/,
                                                           const std::string & /*activeURL*/) {
  // TODO comment in once tag system is implemented in config system
  /*
  auto configKey = appsupport::configkey::getCompositionPortUrlKey(component_name, port_name);

  auto subscribe_url_list = config::getConfig()->getStringList(configKey);
  if (subscribe_url_list.size() == 1 && !config::getConfig()->isList(configKey)) {
    config::getConfig()->removeTag(configKey, next::appsupport::configtags::TAG::ACTIVE);
    if (subscribe_url_list[0].compare(activeURL) == 0) {
      config::getConfig()->setTag(configKey, next::appsupport::configtags::TAG::ACTIVE);
    }
  } else {
    for (size_t i = 0; i < subscribe_url_list.size(); i++) {
      std::string keyUrl = configKey + "[" + std::to_string(i) + "]";
      config::getConfig()->removeTag(keyUrl, next::appsupport::configtags::TAG::ACTIVE);
      if (subscribe_url_list[i].compare(activeURL) == 0) {
        config::getConfig()->setTag(keyUrl, next::appsupport::configtags::TAG::ACTIVE);
      }
    }
  }*/
}

std::vector<std::string> InportConfig::getSubscriptionUrls(const std::string &component_name,
                                                           const std::string &port_name) {
  return config::getConfig()->getStringList(appsupport::configkey::getCompositionPortUrlKey(component_name, port_name),
                                            {});
}

std::string InportConfig::getDataAvilabilityMode(const std::string &component_name) {
  bool value_available{false};
  std::string data_availability_mode = config::getConfig()->get_string(
      appsupport::configkey::getDataAvilabilityMode(component_name), "", false, value_available);
  if (!value_available) {
    config::getConfig()->put(appsupport::configkey::getDataAvilabilityMode(component_name), "DATA_BASED");
  }
  return data_availability_mode;
}

int InportConfig::getUdexSizeDiffThreshold(const std::string &component_name) {
  return config::getConfig()->get_int(next::appsupport::configkey::getUdexSizeDiffThreshold(component_name), 0);
}

int InportConfig::getUdexMaximumSize(const std::string &component_name, const std::string &port_name) {
  return config::getConfig()->get_int(next::appsupport::configkey::getUdexMaximumSize(component_name, port_name), 100);
}

float InportConfig::getUdexMaximumSizeMb(const std::string &component_name, const std::string &port_name) {
  return config::getConfig()->get_float(next::appsupport::configkey::getUdexMaximumSizeMb(component_name, port_name),
                                        0);
}

unsigned int InportConfig::computeUdexMaximumSize(const std::string &component_name, const std::string &port_name,
                                                  const std::size_t &port_size) {
  // get the max number of elements the udex queue should contain
  unsigned int queue_size = (unsigned int)getUdexMaximumSize(component_name, port_name);
  // also compute that value based on a limit size in MB and the input port size
  // conversion 1MB = 1024*1024 B = 2^20 B
  float queue_size_based_on_mb = (getUdexMaximumSizeMb(component_name, port_name) / (float)port_size) * powf(2, 20);

  // get the more restrictive value bitween them
  if (queue_size_based_on_mb > 1.0f && (float)queue_size > queue_size_based_on_mb) {
    // threshold for precision inacurracy
    if (fabs(round(queue_size_based_on_mb) - queue_size_based_on_mb) < 1e-4)
      queue_size = (unsigned int)round(queue_size_based_on_mb);
    else
      queue_size = (unsigned int)queue_size_based_on_mb;
  }
  return queue_size;
}

const std::vector<ComponentInputPortConfig> NextInport::GetInputPorts() const { return configured_input_ports_; }

bool NextInport::isDataAvailable(const std::string &input_port_name) {

  switch (data_availability_mode_) {
  case next::DataAvailabilityMode::ALWAYS_TRUE:
    return true;
  case next::DataAvailabilityMode::DATA_BASED:
  default:
    // Checks if available data is at the port
    auto data_extractor = map_of_extractors_.find(input_port_name);
    if (data_extractor == map_of_extractors_.end()) {
      next::sdk::logger::error(__FILE__, "No extractor available for {0}", input_port_name);
      return false;
    }
    std::string subscribe_url = map_of_subscribe_urls_[input_port_name];
    return data_extractor->second->isDataAvailable(subscribe_url);
  }
}

int NextInport::FillReferenceValueAndData(next::control::events::OrchestratorTrigger &trigger_info) {
  return sync_manager_.FillReferenceValueAndData(trigger_info);
}

bool NextInport::isUpdated(const std::string &port_name) { return sync_manager_.isUpdated(port_name); }

next::udex::TimestampedDataPacket NextInport::getSyncedPacket(const std::string &port_name) {
  return sync_manager_.getSyncedPacket(port_name);
}

void NextInport::setDataAvailabilityMode(const DataAvailabilityMode &data_availability_mode) {
  data_availability_mode_ = data_availability_mode;
}

int NextInport::SetupInput(const std::string &adapter_dll_path,
                           const std::vector<ComponentInputPortConfig> &input_ports) {
  if (!data_subscriber_) {
    data_subscriber_ = std::make_shared<next::udex::subscriber::DataSubscriber>(component_name_);
  }
  // Write all given values into Config active section
  for (auto &it_input_port : input_ports) {
    InportConfig::SetValuesToConfig(it_input_port, component_name_);
  }

  SaveComponentInputPortConfig saved_input_pair;
  saved_input_pair.input_ports = input_ports;
  saved_input_pair.adapter_dll_path = adapter_dll_path;
  saved_input_.push_back(saved_input_pair);

  return 0;
}

void NextInport::SetupSavedInputConfig() {
  auto data_availability_mode = InportConfig::getDataAvilabilityMode(component_name_);
  setDataAvailabilityMode(getDataAvilabilityEnum(data_availability_mode));
  auto udex_size_diff_threshold = InportConfig::getUdexSizeDiffThreshold(component_name_);

  for (auto &elem : saved_input_) {
    if (!elem.initialized_) {
      for (auto &it_input_port : elem.input_ports) {
        auto queue_size = (unsigned short)InportConfig::computeUdexMaximumSize(component_name_, it_input_port.port_name,
                                                                               it_input_port.port_size);
        SetupInportConnectionToNext(elem.adapter_dll_path, udex_size_diff_threshold, queue_size, it_input_port);
      }
      elem.initialized_ = true;
    }
  }
}

std::string NextInport::SearchValidURLinList(const std::vector<std::string> &subscribe_url_list,
                                             std::string &signal_url) {
  next::udex::explorer::SignalExplorer signal_explorer;
  for (auto &subscribe_url : subscribe_url_list) {
    auto available_signals = signal_explorer.GetPackageUrl(subscribe_url);
    if (available_signals.size() != 0) {
      if (available_signals[0] != "") {
        if (available_signals.size() != 1) {
          next::sdk::logger::warn(__FILE__, "Got unclear topic number for {}", subscribe_url);
        }
        // set full selected url (e.g. ADC4xx.EMF.EmfGlobalTimestamp.SigHeader.uiTimeStamp)
        signal_url = subscribe_url;
        // return package url (e.g. ADC4xx.EMF.EmfGlobalTimestamp)
        return available_signals[0];
      }
    }
    next::sdk::logger::warn(__FILE__, "Got empty reply for {}", subscribe_url);
  }
  // if all given urls are not valid return first url
  signal_url = subscribe_url_list[0];
  return subscribe_url_list[0];
}

std::string NextInport::getValidSubscribeURL(std::vector<std::string> &subscribe_url_list, std::string &signal_url,
                                             const std::string &default_url) {
  // catch empty config
  if (subscribe_url_list.empty()) {
    subscribe_url_list.push_back(default_url);
  }
  // TODO remove the space from the compare value after SIMEN-8323 is rezolved
  if (subscribe_url_list.size() == 1 &&
      (subscribe_url_list[0].compare("") == 0 || subscribe_url_list[0].compare(" ") == 0)) {
    subscribe_url_list[0] = default_url;
  }
  auto subscribe_url = SearchValidURLinList(subscribe_url_list, signal_url);
  // catch emtpy subscriptions at all cost
  if (subscribe_url == "" || subscribe_url == " ") {
    subscribe_url = signal_url = "undefined";
  }
  return subscribe_url;
}

void NextInport::SetupInportConnectionToNext(const std::string &adapter_dll_path, const int &udex_size_diff_threshold,
                                             const unsigned short &queue_size,
                                             const ComponentInputPortConfig &it_input_port) {

  auto extractor_temp = std::make_shared<udex::extractor::DataExtractor>(adapter_dll_path, queue_size);

  std::string signal_url{};

  auto subscribe_url_list = InportConfig::getSubscriptionUrls(component_name_, it_input_port.port_name);
  auto subscribe_url = getValidSubscribeURL(subscribe_url_list, signal_url, it_input_port.port_name);

  InportConfig::setActivePortInCompositionConfiguration(component_name_, it_input_port.port_name, subscribe_url);

  udex::SyncAlgoCfg sync_cfg =
      sync_manager_.SetSyncConfig(it_input_port, subscribe_url, signal_url, extractor_temp, data_subscriber_);
  extractor_temp->SetSyncAlgo(subscribe_url, sync_cfg);
  // TODO Need to handle alternatives of subscribers if initial url fails
  data_subscriber_->Subscribe(subscribe_url, extractor_temp);
  extractor_temp->SetExtractionVersion(it_input_port.port_name, it_input_port.port_size,
                                       it_input_port.port_version_number, it_input_port.data_type_name);
  extractor_temp->setSizeDiffThreshold((size_t)udex_size_diff_threshold);

  map_of_extractors_[it_input_port.port_name] = extractor_temp;

  configured_input_ports_.push_back(it_input_port);
  map_of_configured_inports_[it_input_port.port_name] = it_input_port;
  map_of_subscribe_urls_[it_input_port.port_name] = subscribe_url;
}

void NextInport::Reset() {
  if (data_subscriber_) {
    data_subscriber_->Reset();
  }
  sync_manager_.Reset();
  map_of_subscribe_urls_.clear();
  map_of_configured_inports_.clear();
  configured_input_ports_.clear();
  map_of_extractors_.clear();
  data_availability_mode_ = DataAvailabilityMode::NOT_SPECIFIED;
  saved_input_.clear();
}

} // namespace inport
} // namespace next

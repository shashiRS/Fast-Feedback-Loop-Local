/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     next_inport.hpp
 * @brief    provides inport functions and holds respective data
 *
 */
#ifndef NEXT_NEXT_INPORT_HPP
#define NEXT_NEXT_INPORT_HPP

#include <next/data_types.hpp>
#include <next/next.hpp>

#include <next/sdk/sdk.hpp>

#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>

#include <next/udex/explorer/signal_explorer.hpp>
#include <next/udex/extractor/data_extractor.hpp>
#include <next/udex/subscriber/data_subscriber.hpp>

#include "sync_manager/sync_manager.hpp"

namespace next {
namespace inport {
class InportConfig {
  using config = next::appsupport::ConfigClient;

public:
  static void SetValuesToConfig(const ComponentInputPortConfig &it_input_port, const std::string component_name);

  static void addCompositionUrlsFromGlueCode(const std::string &component_name,
                                             const next::ComponentInputPortConfig &it_input_port);

  static void setActivePortInCompositionConfiguration(const std::string & /*component_name*/,
                                                      const std::string & /*port_name*/,
                                                      const std::string & /*activeURL*/);

  static std::vector<std::string> getSubscriptionUrls(const std::string &component_name, const std::string &port_name);

  static std::string getDataAvilabilityMode(const std::string &component_name);

  static int getUdexSizeDiffThreshold(const std::string &component_name);

  static int getUdexMaximumSize(const std::string &component_name, const std::string &port_name);
  static float getUdexMaximumSizeMb(const std::string &component_name, const std::string &port_name);
  static unsigned int computeUdexMaximumSize(const std::string &component_name, const std::string &port_name,
                                             const std::size_t &port_size);
};

class NextInport {
public:
  NextInport(const std::string &component_name) : component_name_(component_name), sync_manager_(component_name) {
    data_subscriber_ = std::make_shared<next::udex::subscriber::DataSubscriber>(component_name_);
  }
  virtual ~NextInport() {}

  const std::vector<ComponentInputPortConfig> GetInputPorts() const;

  bool isDataAvailable(const std::string &input_port_name);

  int FillReferenceValueAndData(next::control::events::OrchestratorTrigger &trigger_info);

  bool isUpdated(const std::string &port_name);

  next::udex::TimestampedDataPacket getSyncedPacket(const std::string &port_name);

  /*!
   * @brief Sets the mode for the "isDataAvailable" call
   *
   * @param data_availability_mode defines how data is marked as ready, either ALWAYS_TRUE for always delivering data
   * (may be 0 initialized) or DATA_BASED to mark validity on new data packages
   */
  void setDataAvailabilityMode(const DataAvailabilityMode &data_availability_mode);

  int SetupInput(const std::string &adapter_dll_path, const std::vector<ComponentInputPortConfig> &input_ports);

  void SetupSavedInputConfig();

  /*!
   * @brief Searches list for valid urls
   *
   * Checks a list of urls to see if a url is available as a Publisher
   *
   * @param subscribe_url_list lsit to be checked for valid subscriber
   * @param signal_url Will be set. Full url of the package that is available
   * @return std::string Available package url
   */
  static std::string SearchValidURLinList(const std::vector<std::string> &subscribe_url_list, std::string &signal_url);

  /*!
   * @brief Get the Valid Subscribe URL object
   *
   * Returns package Url of the url given. Set´s signal URL to selected signal url (If list is given)
   *
   * @param subscriber_url_list list of the subscribe signals
   * @param signal_url Full url that is available
   * @param default_url if no url is given or none is available this url will be used
   * @return std::string Package url that is available (default url if no URL is available)
   */
  static std::string getValidSubscribeURL(std::vector<std::string> &subscribe_url_list, std::string &signal_url,
                                          const std::string &default_url);

  void SetupInportConnectionToNext(const std::string &adapter_dll_path, const int &udex_size_diff_threshold,
                                   const unsigned short &queue_size, const ComponentInputPortConfig &it_input_port);

  void Reset();

private:
  DataAvailabilityMode data_availability_mode_ = DataAvailabilityMode::NOT_SPECIFIED;

  std::string component_name_;
  SyncManager sync_manager_;

  std::shared_ptr<next::udex::subscriber::DataSubscriber> data_subscriber_;
  std::unordered_map<std::string, std::shared_ptr<next::udex::extractor::DataExtractor>> map_of_extractors_;

  std::vector<ComponentInputPortConfig> configured_input_ports_;
  std::unordered_map<std::string, ComponentInputPortConfig> map_of_configured_inports_;
  std::unordered_map<std::string, std::string> map_of_subscribe_urls_;

  struct SaveComponentInputPortConfig {
    std::vector<ComponentInputPortConfig> input_ports;
    std::string adapter_dll_path;
    bool initialized_ = false;
  };
  std::vector<SaveComponentInputPortConfig> saved_input_;

public:
  friend class NextInportTester;
};

} // namespace inport
} // namespace next

#endif // NEXT_NEXT_INPORT_HPP

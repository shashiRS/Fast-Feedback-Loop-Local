/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     sync_manager.hpp
 * @brief    Provide all necessary functions to sync the data for the components
 *
 */

#ifndef SYNCREF_HANDLER_H_
#define SYNCREF_HANDLER_H_

#include <string>
#include <unordered_map>

#include <next/data_types.hpp>

#include <next/udex/data_types.hpp>
#include <next/udex/explorer/signal_explorer.hpp>
#include <next/udex/extractor/data_extractor.hpp>
#include <next/udex/subscriber/data_subscriber.hpp>

namespace next {

struct SyncRefExtractor {
  std::shared_ptr<next::udex::extractor::DataExtractor> syncref_data_extractor;
  bool data_extracted = false;
};

struct SignalUrlPackageUrl {
  std::string signal_url;
  std::string package_url;
};

struct SyncRefPortInfo {
  std::shared_ptr<SyncRefExtractor> syncref_extractor;
  SignalUrlPackageUrl syncref_info;
  std::string syncref_selection_url;
  next::udex::SignalInfo port_selector_info;
};

class SyncRefHandler {
public:
  /*!
   * @brief c-tor
   *
   *
   * @param flow_name the current orchestrator flow for which this instance was create
   * @param selection_url the signal url which is used as a reference for selecting sync ref data
   * @param data_subscriber data subscriber used by the component
   */
  SyncRefHandler(std::string flow_name, std::string selection_url,
                 std::shared_ptr<next::udex::subscriber::DataSubscriber> data_subscriber);

  /*!
   * @brief Sets port specific sync ref handling
   *
   *
   * @param port_name port name for which the sync ref will be used
   * @param sync_ref_url the sync ref signal url which is used to determine which package of the sync ref to be used
   * @param port_sync_info the signal url taht we extract from the sync ref to use as a ref for selecting the correct
   * port package
   */
  void setupPortSyncData(std::string port_name, std::string sync_ref_url, std::string port_sync_info);

  /*!
   * @brief get the sync value for a specific port
   *
   *
   * @param port_name the port for which we extract the sync value
   * @param ts the current trigger ts
   * @return next::udex::SyncValue the sync value that will be used to extract the port package
   */
  next::udex::SyncValue getSyncValue(std::string port_name, uint64_t ts);

private:
  std::shared_ptr<next::udex::extractor::DataExtractor> syncref_selector_extractor_;
  SignalUrlPackageUrl syncref_selection_url_;
  // map of syncref info for each port
  std::unordered_map<std::string, SyncRefPortInfo> sync_info_map_;
  std::string flow_name_;
  std::shared_ptr<next::udex::subscriber::DataSubscriber> data_subscriber_;
  next::udex::dataType syncref_selection_ts_ = (uint64_t)0;
  std::unique_ptr<next::udex::explorer::SignalExplorer> signal_explorer_ =
      std::make_unique<next::udex::explorer::SignalExplorer>();
  uint64_t old_slection_ts_ = 0;
};

} // namespace next

#endif // SYNCREF_HANDLER_H_

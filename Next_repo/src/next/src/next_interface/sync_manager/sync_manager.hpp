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

#ifndef SYNC_MANAGER_H_
#define SYNC_MANAGER_H_

#include <string>
#include <unordered_map>

#include <next/control/orchestrator/orchestrator_client.h>

#include <next/data_types.hpp>
#include <next/next.hpp>

#include <next/udex/data_types.hpp>
#include <next/udex/extractor/data_extractor.hpp>
#include <next/udex/subscriber/data_subscriber.hpp>

#include "sync_config_reader.hpp"
#include "sync_value_handler.hpp"
#include "syncref_handler.hpp"

namespace next {

struct PortInfo {
  std::string subscribed_url_;
  std::size_t port_size_;
  bool port_update_state_;
  std::shared_ptr<next::udex::extractor::DataExtractor> extractor_;
  // unordered_map that holds the SignalInfo for the sync_ref signal for each flow
  std::unordered_map<std::string, next::udex::SignalInfo> sync_ref_signal_info_;
  next::udex::SignalInfo signal_info_;
};

class SyncManager {
  friend class SyncManagerTest;

  struct SyncPortConfig {
    next::SyncMode sync_mode;
    std::string syncref_url;
  };

public:
  SyncManager() = delete;
  SyncManager(const std::string &component_name);
  virtual ~SyncManager();

  /*!
   * @brief Set the Sync Config for the given port
   *
   * Checks the configured ports and sets the mode for all set up data extractors
   * Gets called in setup function
   *
   * @param it_input_port port name to get the SyncAlgo Config for
   * @param subscribe_url the url to subscribe
   * @param signal_url the signal_url that was used to get the url to subscribe, maybe be identical to subscribe_url
   * @param extractor a pointer of the data extractor
   * @return next::udex::SyncAlgoCfg Config for the given port (Algorythm Type, path, signal_url)
   */
  next::udex::SyncAlgoCfg SetSyncConfig(const ComponentInputPortConfig &it_input_port, const std::string &subscribe_url,
                                        std::string &signal_url,
                                        std::shared_ptr<next::udex::extractor::DataExtractor> extractor,
                                        std::shared_ptr<next::udex::subscriber::DataSubscriber> data_subscriber);

  /*!
   * @brief Returns the SyncValue for the given Port
   *
   * Returns the SyncValue for the given subscribe URL which will be used in udex::GetSyncExtractedData
   * gets called in getTriggerAsyncPush
   *
   * @param port_name port name to get sync Value for
   * @return next::udex::SyncValue Value to sync the UDex queue for (Signal Type, Signal Value)
   */
  next::udex::SyncValue GetSyncValue(const std::string &port_name);

  /*!
   * @brief Extracts Sync Values from the trigger information
   *
   * Based on the sync mode decides which values from the trigger should be used to sync.
   * sets them in m_sync_config_
   * gets called in each Callback
   *
   * @param trigger_info Trigger Information to get sync information from
   * @return int 0 = success
   * @return int -1 = failed
   */
  int FillReferenceValueAndData(next::control::events::OrchestratorTrigger &trigger_info);

  /*!
   * @brief Resets all internal variables
   */
  void Reset();

  next::udex::TimestampedDataPacket getSyncedPacket(const std::string &port_name);

  /*!
   * @brief checking whether the data was updated with data from the queue
   *
   * @param port_name the name of the port which is checked
   * @return bool flag indicating if the port has been updated with new data from the queue
   */
  bool isUpdated(const std::string &port_name);

  /*!
   * @brief Sets up the subscribers for the sync ref ports based on the orchestrator configuration defined
   * flows.Reads the flow configuration from the orchestrator and creates the necessary subscribers
   */
  void SetupSyncRef(std::shared_ptr<next::udex::subscriber::DataSubscriber> data_subscriber);

  void SetupSyncValue(std::shared_ptr<next::udex::subscriber::DataSubscriber> data_subscriber);

private:
  /*!
   * @brief
   *
   * @param trigger_info
   * @param cycle_id
   * @param timestamp
   * @return true
   * @return false
   */
  bool UpdateSyncValues(next::control::events::OrchestratorTrigger &trigger_info, next::udex::SyncValue &cycle_id,
                        next::udex::SyncValue &timestamp);
  /*!
   * @brief Get the Sync Value From Sync Ref object
   *
   * @param port_name
   * @return next::udex::SyncValue
   */
  next::udex::SyncValue GetSyncValueFromSyncRef(const std::string &port_name);

  void SetupSyncRefPortInfo(const std::string &port_name, const SyncConfig &sync_config);

  void FillPortSyncedData(const std::string &port_name, next::udex::SyncValue sync);

  void fillBufferUsingOffset(const PortInfo &port_info, next::udex::TimestampedDataPacket &buffer_element);

  void AddPortInfo(const ComponentInputPortConfig &it_input_port, const std::string &subscribe_url,
                   std::string &signal_url, std::shared_ptr<next::udex::extractor::DataExtractor> extractor);

  /*!
   * @brief Maps each input port to the SyncPort Config (Sync Mode and Subscribe URL)
   *
   */
  std::unordered_map<std::string, SyncPortConfig> m_sync_port_config_;
  /*!
   * @brief Maps each input port to the SyncValue (Signal Type, Signal Value)
   *
   */
  std::unordered_map<std::string, next::udex::SyncValue> m_sync_value_;

  std::string component_name_;

  // TODO(( remove once trigger extraction is available
  uint64_t test_sync_value_ = 300000000;

  std::unordered_map<std::string, next::udex::TimestampedDataPacket> synced_data_;

  //! maps each input port to the shadow buffer containing the pointer to the data
  std::unordered_map<std::string, next::udex::TimestampedDataPacket> m_weak_shadow_buffer_;

  //! maps each input port to the port info
  std::unordered_map<std::string, PortInfo> m_port_info_;

  next::control::events::OrchestratorTrigger current_trigger_;

  bool sync_ref_set_up_{false};

  // map of SyncRefHandler objects for each flow
  std::map<std::string, std::shared_ptr<SyncRefHandler>> syncref_handler_map_;
  // map of SyncRefHandler objects for each flow
  std::map<std::string, std::shared_ptr<SyncValueHandler>> sync_value_handler_map_;
};
} // namespace next

#endif // SYNC_MANAGER_H_

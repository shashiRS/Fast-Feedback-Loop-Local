/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     i_orchestrator_master.h
 * @brief    interface class orchestrator for event handling for Next Player Control
 *
 **/

#ifndef __ORCHESTRATOR_MASTER_H__
#define __ORCHESTRATOR_MASTER_H__

#include <string>
#include <unordered_map>

#include "../control_config.h"

#include "flow_control_config.h"

namespace next {
namespace control {
namespace orchestrator {

class OrchestratorMasterImpl;
class CONTROL_LIBRARY_EXPORT OrchestratorMaster {

public:
  //! creates a OrchestratorMaster
  /*! initialize the OrchestratorMasterImpl as
   * private implementation as pimpl pattern
   */
  OrchestratorMaster();

  //! destroys the OrchestratorMaster
  /*! deletes  the private implementation
   */
  virtual ~OrchestratorMaster();

  //! loadConfiguration reads the json conf
  /*! @param config_path is the path to the json conf file
   * @return true if json reading is successful, false if otherwise
   */
  bool loadConfiguration(const std::string &config_path);

  //! loadConfiguration reads the  key value pairs in the config
  /*!
   * @return true if config reading is successful, false if otherwise
   */
  bool loadConfiguration();

  //! saveConfiguration saves the current configuration to the  json file
  /*! @param conf_file is the path to the json conf file
   * @return true if json reading is successful, false if otherwise
   */
  bool saveConfiguration(const std::string &conf_file);

  //! unRegisterFlow remove the flow from the list of current flows
  /*! @param flow is the flow to be removed
   */
  void unRegisterFlow(const FlowLeaf &flow);

  //! registerFlow adds the flow from the list of current flows
  /*! @param flow is the flow to be added
   */
  void registerFlow(const FlowLeaf &flow);

  //! triggerDataBased executes the respective data based trigger flow if the package topic hash matches
  /*! @param timeStamp          timestamp of the current package which will be passed to the trigger flow
   *  @param hash               hash of the current package to check for triggering
   *  @param trigger_statuses   output map of fired triggers and their status
   *  @return true if triggering was successful, false otherwise
   */
  bool triggerDataBased(uint64_t timeStamp, size_t hash, std::map<std::string, TriggerStatus> &trigger_statuses);

  //! triggerDataBased executes the respective data based trigger flow if the package topic hash matches
  /*! @param timeStamp          timestamp of the current package which will be passed to the trigger flow
   *  @param hash               hash of the current package to check for triggering
   *  @return true if triggering successful, false otherwise
   */
  inline bool triggerDataBased(uint64_t timeStamp, size_t hash) {
    std::map<std::string, TriggerStatus> trigger_statuses;

    return triggerDataBased(timeStamp, hash, trigger_statuses);
  };

  //! triggerDataBased executes the respective data based trigger flow if the package topic url matches
  /*! @param timeStamp          timestamp of the current package which will be passed to the trigger flow
   *  @param URL                URL of the current package to check for triggering
   *  @param trigger_statuses   output map of fired triggers and their status
   *  @return true if triggering successful, false otherwise
   */
  bool triggerDataBased(uint64_t timeStamp, const std::string &URL,
                        std::map<std::string, TriggerStatus> &trigger_statuses);

  //! triggerDataBased executes the respective data based trigger flow if the package topic url matches
  /*! @param timeStamp          timestamp of the current package which will be passed to the trigger flow
   *  @param URL                URL of the current package to check for triggering
   *  @return true if triggering successful, false otherwise
   */
  inline bool triggerDataBased(uint64_t timeStamp, const std::string &URL) {
    std::map<std::string, TriggerStatus> trigger_statuses;

    return triggerDataBased(timeStamp, URL, trigger_statuses);
  };

  //! triggerCycleBased executes the respective cycle based trigger flow if the package cycle ID matches
  /*! @param cycleId            cycleID of the current package which shall be checked for triggering
   *  @param is_cycle_start     if its a start or end of the cycle, boolean value
   *  @param trigger_statuses   output map of fired triggers and their status
   *  @param timestamp          timestamp of the current package which will be passed to the trigger flow
   *  @return true if triggering was successful, false otherwise
   */
  bool triggerCycleBased(uint32_t cycleId, std::map<std::string, TriggerStatus> &trigger_statuses,
                         bool is_cycle_start = true, uint64_t timestamp = 0);

  //! triggerCycleBased executes the respective cycle based trigger flow if the package cycle ID matches
  /*! @param cycleId            cycleID of the current package which shall be checked for triggering
   *  @param is_cycle_start     if its a start or end of the cycle, boolean value
   *  @param timestamp          timestamp of the current package which will be passed to the trigger flow
   *  @return true if triggering was successful, false otherwise
   */
  bool triggerCycleBased(uint32_t cycleId, bool is_cycle_start = true, uint64_t timestamp = 0) {
    std::map<std::string, TriggerStatus> trigger_statuses;

    return triggerCycleBased(cycleId, trigger_statuses, is_cycle_start, timestamp);
  }

  //! triggerTimestampBased executes the respective timestamp based trigger flow if time difference to the last
  //! triggering is above a threshold
  /*! @param timestamp          timestamp of the current package which will be checked for triggering and passed to the
   * trigger flow
   *  @param trigger_statuses   output map of fired triggers and their status
   *  @return true if triggering was successful, false otherwise
   */
  bool triggerTimestampBased(uint64_t timestamp, std::map<std::string, TriggerStatus> &trigger_statuses);

  //! triggerTimestampBased executes the respective timestamp based trigger flow if time difference to the last
  //! triggering is above a threshold
  /*! @param timestamp          timestamp of the current package which will be checked for triggering and passed to the
   * trigger flow
   *  @param trigger_statuses   output map of fired triggers and their status
   *  @return true if triggering was successful, false otherwise
   */
  inline bool triggerTimestampBased(uint64_t timestamp) {
    std::map<std::string, TriggerStatus> trigger_statuses;

    return triggerTimestampBased(timestamp, trigger_statuses);
  }

  //! fireTrigger publishs the trigger event of the trigger flow for triggering. The trigger flow name is determined
  //! based on the node name and the method name
  /*! @param nodeName       node name of the client
   *  @param methodName     remote method name which would be invoked
   *  @param timeStamp      timestamp to be passed to the trigger event
   *  @param optional_str   payload to be sent by the trigger event
   */
  TriggerStatus fireTrigger(const std::string &nodeName, const std::string &methodName, uint64_t timeStamp = 0,
                            const std::string &flow_id = "");

  //! setupDataBasedTriggering creates a list of available topic urls and their hashes based on the configured topics
  //! for data based triggering. Will be used by data based triggering to map the package hashes to the topic urls (data
  //! based triggers are specified by the topic url)
  /*! @param src_hash_to_url_map    map containing the available topic urls and their hashes
   */
  void setupDataBasedTriggering(const std::unordered_map<std::string, size_t> &src_hash_to_url_map);

  //! resetOrchestrator cleans the async operations handles and hash set for topic urls
  /*
        Function is needed for clients viz player when new or old recording is reloaded
  */
  void resetOrchestrator();

private:
  OrchestratorMasterImpl *p_impl_ = nullptr;
};

} // namespace orchestrator
} // namespace control
} // namespace next

#endif //__ORCHESTRATOR_MASTER_H__

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

  //! checkSignalForDataPkg important function which triggers the subscribers listening at OrchestratorTriggerHandler
  //! side
  /*! @param timeStamp  timestamp based triggers
   *  @param URL  signalname  based triggers
   *  @param trigger_statuses output map of fired triggers and their status
   *  @return true if triggering successful, false if otherwise
   */
  bool checkSignalForDataPkg(uint64_t timeStamp, const std::string &URL,
                             std::map<std::string, TriggerStatus> &trigger_statuses);

  //! checkSignalForDataPkg important function which triggers the subscribers listening at OrchestratorTriggerHandler
  //! side
  /*! @param timeStamp  timestamp based triggers
   *  @param URL  signalname  based triggers
   *  @return true if triggering successful, false if otherwise
   */
  bool checkSignalForDataPkg(uint64_t timeStamp, const std::string &URL) {
    std::map<std::string, TriggerStatus> trigger_statuses;

    return checkSignalForDataPkg(timeStamp, URL, trigger_statuses);
  };

  //! checkSignalForCyclePkg important function which triggers the subscribers listening at OrchestratorTriggerHandler
  //! side
  /*! @param cycleId  cycle value based triggers
   *  @param is_cycle_start  if its a start or end cycle, boolean value
   *  @param trigger_statuses output map of fired triggers and their status
   *  @param timestamp timestamp to be passed to the subscriber
   *  @return true if triggering successful, false if otherwise
   */
  bool checkSignalForCyclePkg(uint32_t cycleId, std::map<std::string, TriggerStatus> &trigger_statuses,
                              bool is_cycle_start = true, uint64_t timestamp = 0);

  //! checkSignalForCyclePkg important function which triggers the subscribers listening at OrchestratorTriggerHandler
  //! side
  /*! @param cycleId  cycle value based triggers
   *  @param is_cycle_start  if its a start or end cycle, boolean value
   *  @param timestamp timestamp to be passed to the subscriber
   *  @return true if triggering successful, false if otherwise
   */
  bool checkSignalForCyclePkg(uint32_t cycleId, bool is_cycle_start = true, uint64_t timestamp = 0) {
    std::map<std::string, TriggerStatus> trigger_statuses;

    return checkSignalForCyclePkg(cycleId, trigger_statuses, is_cycle_start, timestamp);
  }

  //! fireTrigger important function which triggers the subscribers listening at OrchestratorTriggerHandler
  //! side
  /*! @param nodeName node name of the client
   *  @param methodName remote method name which would be invoked
   *  @param timeStamp timestamp to be passed to the subscriber
   *  @param optional_str payload to be sent the subscriber
   */
  TriggerStatus fireTrigger(const std::string &nodeName, const std::string &methodName, uint64_t timeStamp = 0,
                            const std::string &flow_id = "");

  //! createAvailableSignalsForCurrentRecording utility function
  //! clients of this interface call this to filter incoming flows from recording with flows available in flows received
  //! from json config file
  /*! this function creates the internal hash map having hash as key and signal name as value
   *  @param src_hash_to_url_map  hash from client e.g player having signal hash key value from the recording publisher
  per cycle
   */

  void createAvailableSignalsForCurrentRecording(const std::unordered_map<std::string, size_t> &src_hash_to_url_map);

  //! getUrlFromHashValue utility function
  //! clients of this interface call this to get signal name

  /*! @param hash  hash from client e.g player
   *  @return  returns the filtered signal_name with respect to hash sent from player if found otherwise empty string
   */
  std::string getUrlFromHashValue(size_t hash);

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

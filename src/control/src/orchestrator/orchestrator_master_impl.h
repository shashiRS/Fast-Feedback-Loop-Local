/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     orchestrator_master_impl.h
 * @brief    orchestrator for event handling for Next Player Control
 *
 **/

#ifndef __ORCHESTRATOR_MASTER_IMPL_H__
#define __ORCHESTRATOR_MASTER_IMPL_H__

#include <list>
#include <map>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>

#include <next/control/orchestrator/flow_control_config.h>

namespace next {
namespace control {
namespace orchestrator {

class ControlDataGenerator;

class OrchestratorMasterImpl {
  DISABLE_COPY_MOVE(OrchestratorMasterImpl)
public:
  OrchestratorMasterImpl();
  virtual ~OrchestratorMasterImpl();

  bool loadConfiguration();

  void unRegisterFlow(const FlowLeaf &flow_leaf);
  void registerFlow(const FlowLeaf &flow_leaf);

  bool triggerDataBased(uint64_t timeStamp, size_t hash, std::map<std::string, TriggerStatus> &trigger_output);

  bool triggerDataBased(uint64_t timeStamp, const std::string &URL,
                        std::map<std::string, TriggerStatus> &trigger_output);

  // may be timestamp can also be sent , we will check it
  bool triggerCycleBased(uint32_t cycleId, std::map<std::string, TriggerStatus> &trigger_output,
                         bool is_cycle_start = true, uint64_t timestamp = 0);
  bool triggerTimestampBased(uint64_t timeStamp, std::map<std::string, TriggerStatus> &trigger_output);

  // for closed loop simulation
  TriggerStatus fireTrigger(const std::string &flowName, uint64_t timeStamp = 0, const std::string &flow_id = "") const;

  void setupDataBasedTriggering(const std::unordered_map<std::string, size_t> &src_hash_to_url_map);
  void resetOrchestrator();

protected:
  FlowLeaf createFlow(const std::string &flow_id) const;

private:
  void AddFlowToOrchestrator(const FlowLeaf &flow_leaf);

  void execFlowLeaf(const FlowLeaf &leaf, std::map<std::string, TriggerStatus> &trigger_output, uint64_t timestamp = 0);

  void eraseFlow(std::multimap<std::string, FlowLeaf> &multi_map, const std::string &key, const FlowLeaf &flow);

  bool addFlow(std::multimap<std::string, FlowLeaf> &multi_map, const std::string &key, const FlowLeaf &leaf);

  void generateControlData(const uint64_t timestamp, const std::string &flow_id, const std::string &client,
                           const std::string &method);

  bool getPackageUrlFromSignalUrl(const std::string &url, std::string &package_url);

  // will remove q_flows_ in future
  std::multimap<std::string, FlowLeaf> multi_map_timebased_triggering_, multi_map_data_triggering_,
      multi_map_cycle_triggering_;

  // cached flows depending on meta data from configuration file of property delay
  std::map<std::string, std::queue<std::pair<FlowLeaf, uint64_t>>> map_cached_flows_;

  // thread list for handling async trigger firing
  std::vector<std::thread> list_of_async_fired_triggers_;

  // hash_udex_signal_container
  std::unordered_map<size_t, std::string> hash_url_map_data_triggering_;

  const std::string component_name_ = "Orchestrator";

  std::unordered_map<std::string, std::shared_ptr<ControlDataGenerator>> ctrl_data_generators_;

public:
  friend class OrchestratorMasterImplTester;
};

} // namespace orchestrator
} // namespace control
} // namespace next

#endif //__ORCHESTRATOR_MASTER_IMPL_H__

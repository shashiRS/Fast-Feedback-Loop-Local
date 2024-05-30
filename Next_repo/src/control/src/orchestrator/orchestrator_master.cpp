#include <next/control/orchestrator/orchestrator_master.h>
#include "orchestrator_master_impl.h"

namespace next {
namespace control {
namespace orchestrator {

OrchestratorMaster::OrchestratorMaster() { p_impl_ = new OrchestratorMasterImpl(); }

OrchestratorMaster::~OrchestratorMaster() { delete p_impl_; }

bool OrchestratorMaster::loadConfiguration() {
  if (p_impl_)
    return p_impl_->loadConfiguration();
  return false;
}

void OrchestratorMaster::unRegisterFlow(const FlowLeaf &flow) {
  if (p_impl_)
    p_impl_->unRegisterFlow(flow);
}

void OrchestratorMaster::registerFlow(const FlowLeaf &flow) {
  if (p_impl_)
    p_impl_->registerFlow(flow);
}

bool OrchestratorMaster::triggerDataBased(uint64_t timeStamp, size_t hash,
                                          std::map<std::string, TriggerStatus> &trigger_output) {
  if (p_impl_)
    return p_impl_->triggerDataBased(timeStamp, hash, trigger_output);
  return false;
}

bool OrchestratorMaster::triggerDataBased(uint64_t timeStamp, const std::string &URL,
                                          std::map<std::string, TriggerStatus> &trigger_output) {
  if (p_impl_)
    return p_impl_->triggerDataBased(timeStamp, URL, trigger_output);
  return false;
}

bool OrchestratorMaster::triggerCycleBased(uint32_t cycleId, std::map<std::string, TriggerStatus> &trigger_output,
                                           bool is_cycle_start, uint64_t timestamp) {
  if (p_impl_)
    return p_impl_->triggerCycleBased(cycleId, trigger_output, is_cycle_start, timestamp);
  return false;
}

bool OrchestratorMaster::triggerTimestampBased(uint64_t timestamp,
                                               std::map<std::string, TriggerStatus> &trigger_statuses) {
  if (p_impl_)
    return p_impl_->triggerTimestampBased(timestamp, trigger_statuses);
  return false;
}

TriggerStatus OrchestratorMaster::fireTrigger(const std::string &nodeName, const std::string &methodName,
                                              uint64_t timeStamp, const std::string &flow_id) {

  if (p_impl_)
    return p_impl_->fireTrigger(nodeName + methodName, timeStamp, flow_id);

  return TriggerStatus::UNKNOWN;
}

void OrchestratorMaster::setupDataBasedTriggering(const std::unordered_map<std::string, size_t> &src_hash_to_url_map) {

  if (p_impl_)
    p_impl_->setupDataBasedTriggering(src_hash_to_url_map);
}

void OrchestratorMaster::resetOrchestrator() {
  if (p_impl_)
    return p_impl_->resetOrchestrator();
}

} // namespace orchestrator
} // namespace control
} // namespace next

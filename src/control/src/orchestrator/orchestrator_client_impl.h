/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     orchestrator_client_impl.h
 * @brief    orchestrator for event handling for Next Player Control
 *
 **/

#ifndef __ORCHESTRATOR_CLIENT_H__
#define __ORCHESTRATOR_CLIENT_H__

#include <string>
#include <unordered_map>

#include <next/control/orchestrator/flow_control_config.h>

namespace next {
namespace control {
namespace orchestrator {
class OrchestratorTriggerHandlerImpl {
  DISABLE_COPY_MOVE(OrchestratorTriggerHandlerImpl)
public:
  OrchestratorTriggerHandlerImpl(const std::string &node_name);
  virtual ~OrchestratorTriggerHandlerImpl();
  void registerHook(const std::string &method_name, trigger_hook cb);
  std::list<std::string> invokeTrigger(const std::string &node_name, const std::string &method_name,
                                       const std::string &timestamp = "", const std::string &flow_id = "");
  events::OrchestratorTrigger extractTriggerInfo(const std::string &method_name);

private:
  OrchestratorErrors m_error{OrchestratorErrors::SUCCESS};
  std::string node_name_;
  std::string method_name_;
  std::unordered_map<std::string, std::shared_ptr<next::control::events::OrchestratorEventBinary>> event_map_;
};
} // namespace orchestrator
} // namespace control
} // namespace next

#endif //__ORCHESTRATOR_CLIENT_H__

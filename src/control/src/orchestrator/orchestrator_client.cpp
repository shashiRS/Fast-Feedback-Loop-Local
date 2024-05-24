#include <next/control/orchestrator/orchestrator_client.h>
#include "orchestrator_client_impl.h"

namespace next {
namespace control {
namespace orchestrator {

OrchestratorTriggerHandler::OrchestratorTriggerHandler(const std::string &node_name) {
  p_impl_ = new OrchestratorTriggerHandlerImpl(node_name);
}

OrchestratorTriggerHandler ::~OrchestratorTriggerHandler() { delete p_impl_; }

void OrchestratorTriggerHandler::registerHook(const std::string &method_name, trigger_hook cb) {
  if (p_impl_)
    p_impl_->registerHook(method_name, cb);
}

std::list<std::string> OrchestratorTriggerHandler::invokeTrigger(const std::string &node_name,
                                                                 const std::string &method_name,
                                                                 const std::string &timestamp,
                                                                 const std::string &flow_id) {
  if (p_impl_) {
    return p_impl_->invokeTrigger(node_name, method_name, timestamp, flow_id);
  }
  return {};
}

std::list<std::string> OrchestratorTriggerHandler::invokeTrigger(const std::string &method_name,
                                                                 const std::string &timestamp,
                                                                 const std::string &flow_id) {
  if (p_impl_) {
    return p_impl_->invokeTrigger("", method_name, timestamp, flow_id);
  }
  return {};
}

next::control::events::OrchestratorTrigger
OrchestratorTriggerHandler::extractTriggerInfo(const std::string &method_name) {
  if (p_impl_)
    return p_impl_->extractTriggerInfo(method_name);
  return next::control::events::OrchestratorTrigger();
}

} // namespace orchestrator
} // namespace control
} // namespace next

/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     orchestrator_client.h
 * @brief    orchestrator for event handling for Next Player Control
 *
 **/

#include "orchestrator_client_impl.h"

namespace next {
namespace control {
namespace orchestrator {

OrchestratorTriggerHandlerImpl::OrchestratorTriggerHandlerImpl(const std::string &node_name) { node_name_ = node_name; }

OrchestratorTriggerHandlerImpl::~OrchestratorTriggerHandlerImpl() { event_map_.clear(); }

std::list<std::string> OrchestratorTriggerHandlerImpl::invokeTrigger(const std::string &node_name,
                                                                     const std::string &method_name,
                                                                     const std::string &timestamp,
                                                                     const std::string &flow_id) {
  std::string node_name_method_name = (node_name.empty() ? node_name_ : node_name) + method_name;
  events::OrchestratorEventBinary event(node_name_method_name);
  events::OrchestratorTrigger data;
  data.trigger_timestamp = timestamp;
  data.flow_id = flow_id;
  std::list<std::string> resp;
  event.publish(data, resp);
  return resp;
}

void OrchestratorTriggerHandlerImpl::registerHook(const std::string &method_name, trigger_hook cb) {

  this->method_name_ = method_name;
  std::string node_name_method_name = node_name_ + method_name;
  std::shared_ptr<events::OrchestratorEventBinary> event =
      std::make_shared<events::OrchestratorEventBinary>(node_name_method_name);

  event_map_[node_name_method_name] = event;

  event->addSyncEventHook([=](const next::sdk::events::payload_type &payload, std::string &response) {
    (void)payload;
    std::string err_message;
    cb(m_error, err_message);
    if (err_message.empty()) {
      err_message = "\"\"";
    }
    std::stringstream tmp_resp;
    tmp_resp << "{"
             << "\"return_type\":\"" << OrchestratorErrorsToString(m_error) << "\","
             << "\"callback_return\":" << err_message << "}";
    response = tmp_resp.str();
  });

  event->subscribe();
}

events::OrchestratorTrigger OrchestratorTriggerHandlerImpl::extractTriggerInfo(const std::string &method_name) {

  std::string node_name_method_name = node_name_ + method_name;
  auto event = event_map_[node_name_method_name];

  if (event) {
    return event->getEventData();
  }
  return events::OrchestratorTrigger{};
}

} // namespace orchestrator
} // namespace control
} // namespace next

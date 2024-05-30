/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     next_ui_backend_request_handler.cpp
 *  @brief    Implementation to receive and process the backend config request from GUI.
 */

#include "next_ui_backend_request_handler.h"
#include <next/sdk/logger/logger.hpp>
#include "event_handler_helper.h"

namespace next {
namespace databridge {
namespace ui_request {

void NextUiBackendRequestHandler::ForwardNextUiBackendRequest(
    std::function<void(DataClassManager::BackendConfigRequestItem request_item)> next_ui_backend_req_fwd,
    Json::Value command) {
  auto event_enum = EventHandlerHelper::StringToEventType(command["event"].asString());
  switch (event_enum) {

  case EventType::e_backend_config_changed: {
    if (!command["payload"].isMember("allowPackageDrop") || !command["payload"].isMember("enableDataSynchronization")) {
      error(__FILE__, " Backend config event could not be parsed. Payload doesn't contain "
                      "allowPackageDrop or enableDataSynchronization. Payload invalid. Event is ignored.");
      return;
    }
    bool package_drop_enabled = command["payload"]["allowPackageDrop"].asBool();
    bool synchronization_enabled = command["payload"]["enableDataSynchronization"].asBool();
    DataClassManager::BackendConfigRequestItem backend_request{synchronization_enabled, package_drop_enabled};
    next_ui_backend_req_fwd(backend_request);

  } break;
  default:
    error(__FILE__, " {0} event wasn't found", command["event"].asString());
    break;
  }
}

} // namespace ui_request
} // namespace databridge
} // namespace next

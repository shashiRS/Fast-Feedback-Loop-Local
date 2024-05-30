/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     next_ui_backend_request_handler.h
 *  @brief    Functions to receive and process the backend config request from GUI.
 */

#ifndef NEXT_UI_BACKEND_REQUEST_HANDLER_H_
#define NEXT_UI_BACKEND_REQUEST_HANDLER_H_

#include "event_handler_helper.h"

namespace next {
namespace databridge {
namespace ui_request {

/*!
 * The next ui backend request handler parse and
 * forward the ui backend requests to config request queue.
 */
class NextUiBackendRequestHandler {
public:
  //! CTor and DTor removed as static functions only
  NextUiBackendRequestHandler() = delete;
  ~NextUiBackendRequestHandler() = delete;

  //! Function to handle the event types to be added to backend request queue
  /*!
   * check the types of the event, process and add to the update request queue,
   * this function is called from HandleEvent
   * @param next_ui_backend_req_fwd callback function to DataClassManager::AddBackendRequestToQueue
   * @param command correponds to the request in JSON format
   */
  static void ForwardNextUiBackendRequest(
      std::function<void(DataClassManager::BackendConfigRequestItem request_item)> next_ui_backend_req_fwd,
      Json::Value command);
};
} // namespace ui_request
} // namespace databridge
} // namespace next

#endif // NEXT_UI_BACKEND_REQUEST_HANDLER_H_

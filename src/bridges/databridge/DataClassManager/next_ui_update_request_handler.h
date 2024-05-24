/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     next_ui_update_request_handler.h
 *  @brief    Functions to receive and update the update request from GUI.
 */

#ifndef NEXT_UI_UPDATE_REQUEST_HANDLER_H_
#define NEXT_UI_UPDATE_REQUEST_HANDLER_H_

#include <json/json.h>
#include <string>
#include "data_class_manager.h"

namespace next {
namespace databridge {
namespace ui_request {

/*!
 * The next ui update request handler parse and
 * forward the ui update requests to update request queue.
 */
class NextUiUpdateRequestHandler {
public:
  //! Constructor
  NextUiUpdateRequestHandler(){};
  //! A destructor.
  virtual ~NextUiUpdateRequestHandler(){};

  //! Function to handle the event types to be added to config request queue
  /*!
   * check the types of the event, process and add to the update request queue,
   * this function to be called from HandleEvent
   * @param next_ui_update_req_fwd callback function to DataClassManager::AddUpdateRequestToQueue
   * @param command correponds to the request in JSON format
   */
  void ForwardNextUiUpdateRequest(
      std::function<void(DataClassManager::DataRequestItem, std::function<void(std::string)>)> next_ui_update_req_fwd,
      Json::Value command);

private:
  //! helper method to check "name", "version" and "arguments" member of json structure
  /*!
   * @param data_class string coding the event.
   * @param check_string_args flag to check string arguments.
   */
  bool NameVersionArgumentsCheck(const Json::Value &data_class, const bool check_string_args = true);

private:
  Json::StreamWriterBuilder builder_;
};
} // namespace ui_request
} // namespace databridge
} // namespace next

#endif // NEXT_UI_UPDATE_REQUEST_HANDLER_H_

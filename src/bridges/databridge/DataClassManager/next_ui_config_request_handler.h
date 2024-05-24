/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     next_ui_config_request_handler.h
 *  @brief    Functions to receive and process the config request from GUI.
 */

#ifndef NEXT_UI_CONFIG_REQUEST_HANDLER_H_
#define NEXT_UI_CONFIG_REQUEST_HANDLER_H_

#include <json/json.h>
#include <functional>
#include "data_class_manager.h"

namespace next {
namespace databridge {
namespace ui_request {
/*!
 * The next ui config request handler parse and
 * forward the ui config requests to config request queue.
 */
class NextUiConfigRequestHandler {
public:
  // default CTor and DTor
  NextUiConfigRequestHandler(){};
  virtual ~NextUiConfigRequestHandler(){};

  //! Function to handle the event types to be added to config request queue
  /*!
   * check the types of the event, process and add to the config request queue,
   * this function is called from HandleEvent
   * @param next_ui_config_req_fwd delegate function to DataClassManager::AddConfigRequestToQueue
   * @param command correponds to the request in JSON format.
   */
  void ForwardNextUiConfigRequest(std::function<void(DataClassManager::ConfigDataRequestItem)> next_ui_config_req_fwd,
                                  Json::Value command);

private:
  //! Function to parse json command for the event type e_used_data_classes_changed
  /*!
   * check the types of the event, process and add to the config request queue,
   * this function is called from ForwardNextUiConfigRequest
   * @param config_req_callback delegate function to DataClassManager::AddConfigRequestToQueue
   * @param command correponds to the request in JSON format
   */
  void ParseJsonCommandUsedDataClass(std::function<void(DataClassManager::ConfigDataRequestItem)> config_req_callback,
                                     Json::Value command);

  //! Function to parse json command for the event type e_user_requested_available_data_classes
  /*!
   * check the types of the event, process and add to the config request queue,
   * this function is called from ForwardNextUiConfigRequest
   * @param config_req_callback delegate function to DataClassManager::AddConfigRequestToQueue
   * @param command correponds to the request in JSON format
   */
  void
  ParseJsonCommandAvailableDataClass(std::function<void(DataClassManager::ConfigDataRequestItem)> config_req_callback,
                                     Json::Value command);

  //! Function to parse json command for the event type e_user_requested_available_data_classes.
  /*!
   * check the types of the event, process and add to the config request queue,
   * this function is called from ForwardNextUiConfigRequest.
   * @param config_req_callback delegate function to DataClassManager::AddConfigRequestToQueue.
   * @param source source name received from GUI.
   * @param new_config_requests correponds to the config data requests received.
   */
  void HandleParsedConfig(std::function<void(DataClassManager::ConfigDataRequestItem)> config_req_callback,
                          const std::string &source,
                          std::map<std::string, DataClassManager::ConfigDataRequestItem> &new_config_requests);

private:
  // stores list of previous plugins and their configurations
  // e.g. to know if all configs from a plugin are removed
  // source -> plugin name, plugin config
  std::map<std::string, std::map<std::string, DataClassManager::ConfigDataRequestItem>> previous_config_requests_;
};

} // namespace ui_request
} // namespace databridge
} // namespace next

#endif // NEXT_UI_CONFIG_REQUEST_HANDLER_H_

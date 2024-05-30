/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     data_request_server.cpp
 *  @brief    Implementation for receiving, processing and handling the input events
 */

#include "event_handler_helper.h"

#include <next/sdk/logger/logger.hpp>

#include "json_to_plugin_config.h"

#include "next_ui_backend_request_handler.h"
#include "next_ui_config_request_handler.h"
#include "next_ui_update_request_handler.h"

namespace next {
namespace databridge {
namespace ui_request {

EventHandlerHelper::EventHandlerHelper() { builder_["indentation"] = ""; }

EventType EventHandlerHelper::StringToEventType(std::string const &in_string) {
  auto it = stringToEvent.find(in_string);
  if (it != stringToEvent.end()) {
    return it->second;
  } else {
    error(__FILE__, "Event not found for the command: {0}", in_string);
    return EventType::e_none;
  }
}

bool EventHandlerHelper::CheckNextUiEvent(Json::Value command) {
  if (!command.isMember("channel") || !command["channel"].isString() || command["channel"].isNull()) {
    error(__FILE__, "Event doesn't contain a valid "
                    "channel"
                    " member. Event is ignored.");
    return false;
  }

  if (!command.isMember("event") || !command["event"].isString() || command["event"].isNull()) {
    error(__FILE__, "Event doesn't contain a valid "
                    "event"
                    " member. Event is ignored.");
    return false;
  }

  if (!command.isMember("source") || !command["source"].isString() || command["source"].isNull()) {
    error(__FILE__, "Event doesn't contain a valid "
                    "source"
                    " member. Event is ignored.");
    return false;
  }

  if (!command.isMember("payload") || !command["payload"].isObject()) {
    error(__FILE__, "Event doesn't contain a valid "
                    "payload"
                    " member. Event is ignored.");
    return false;
  }
  return true;
}

bool EventHandlerHelper::IsUpdateRequest(const EventType &event_enum) {
  if ((event_enum == EventType::e_used_data_stream_uris_changed) ||
      (event_enum == EventType::e_user_requested_data_stream_format) ||
      (event_enum == EventType::e_widget_requests_data_stream_update_event) ||
      (event_enum == EventType::e_user_pressed_signal_update_button) ||
      (event_enum == EventType::e_user_pressed_signal_search_button) || (event_enum == EventType::e_3d_view) ||
      (event_enum == EventType::e_shutdown) || (event_enum == EventType::e_widget_startup_command_changed) ||
      (event_enum == EventType::e_general_widget_event)) {
    return true;
  } else {
    return false;
  }
}
bool EventHandlerHelper::IsDataClassRequest(const EventType &event_enum) {
  if ((event_enum == EventType::e_user_requested_available_data_classes) ||
      (event_enum == EventType::e_used_data_classes_changed)) {
    return true;
  } else {
    return false;
  }
}
bool EventHandlerHelper::IsBackendConfigRequest(const EventType &event_enum) {
  if (event_enum == EventType::e_backend_config_changed) {
    return true;
  } else {
    return false;
  }
}

void EventHandlerHelper::HandleEvent(Json::Value command,
                                     std::shared_ptr<next::databridge::DataClassManager> sp_data_class_manager) {

  if (!CheckNextUiEvent(command)) {
    return;
  }
  Json::StreamWriterBuilder writer;
  std::string jsonString = Json::writeString(writer, command);
  debug(__FILE__, " event received: {0}", command["event"].asString());
  auto event_enum = StringToEventType(command["event"].asString());
  if (IsUpdateRequest(event_enum)) {
    auto update_req_func = std::bind(&DataClassManager::AddUpdateRequestToQueue, sp_data_class_manager,
                                     std::placeholders::_1, std::placeholders::_2);
    update_request_handler_.ForwardNextUiUpdateRequest(update_req_func, command);
  } else if (IsDataClassRequest(event_enum)) {
    auto config_req_func =
        std::bind(&DataClassManager::AddConfigRequestToQueue, sp_data_class_manager, std::placeholders::_1);
    config_request_handler_.ForwardNextUiConfigRequest(config_req_func, command);
  } else if (IsBackendConfigRequest(event_enum)) {
    auto backend_req_func =
        std::bind(&DataClassManager::AddBackendRequestToQueue, sp_data_class_manager, std::placeholders::_1);
    NextUiBackendRequestHandler::ForwardNextUiBackendRequest(backend_req_func, command);
  } else {
    error(__FILE__, " No matching event found");
  }
}

} // namespace ui_request
} // namespace databridge
} // namespace next

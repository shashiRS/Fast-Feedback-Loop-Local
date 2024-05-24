/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     next_ui_update_request_handler.cpp
 *  @brief    Implementation to receive and update the update request from GUI.
 */

#include "next_ui_update_request_handler.h"
#include <next/sdk/logger/logger.hpp>
#include "event_handler_helper.h"

namespace next {
namespace databridge {
namespace ui_request {

bool NextUiUpdateRequestHandler::NameVersionArgumentsCheck(const Json::Value &data_class,
                                                           const bool check_string_args /*= true*/) {
  if (!data_class.isMember("name") || !data_class["name"].isString() || data_class["name"].isNull()) {
    return false;
  }
  if (!data_class.isMember("version") || !data_class["version"].isString() || data_class["version"].isNull()) {
    return false;
  }
  if (!check_string_args) {
    return true;
  }
  if (!data_class.isMember("arguments") || !data_class["arguments"].isArray()) {
    return false;
  }
  return true;
}

void NextUiUpdateRequestHandler::ForwardNextUiUpdateRequest(
    std::function<void(DataClassManager::DataRequestItem, std::function<void(std::string)>)> next_ui_update_req_fwd,
    Json::Value command) {
  builder_["indentation"] = ""; // reset to default indentation setting as the builder_ is static
  auto event_enum = EventHandlerHelper::StringToEventType(command["event"].asString());
  switch (event_enum) {

  case EventType::e_used_data_stream_uris_changed: {
    next_ui_update_req_fwd(
        {"signals", "0.0.1", command["source"].asString(), {Json::writeString(builder_, command["payload"])}},
        [](std::string ret) { debug(__FILE__, ret.c_str()); });
  } break;
  case EventType::e_user_requested_data_stream_format: {
    if (command["payload"].isMember("selectedNodeId")) {
      next_ui_update_req_fwd({"SignalList",
                              "1.0.0",
                              command["source"].asString(),
                              {Json::writeString(builder_, command["payload"]["selectedNodeId"])}},
                             [](std::string ret) { info(__FILE__, ret.c_str()); });
    } else {
      error(__FILE__, "selectedNodeId not a member of {0}. Event is ignored.", command["payload"].asString());
      return;
    }

  } break;

  case EventType::e_user_pressed_signal_update_button: {
    next_ui_update_req_fwd({"SignalList", "1.0.0", command["source"].asString(), {""}},
                           [](std::string ret) { info(__FILE__, ret.c_str()); });
  } break;

  case EventType::e_user_pressed_signal_search_button: {
    next_ui_update_req_fwd(
        {"SignalList", "1.0.0", command["source"].asString(), {Json::writeString(builder_, command["payload"])}},
        [](std::string ret) { info(__FILE__, ret.c_str()); });
  } break;
  case EventType::e_3d_view: {
    if (!command["payload"].isMember("dataClasses") || !command["payload"]["dataClasses"].isArray()) {
      error(__FILE__, "input doesn't contain valid 'dataClass'. Event 'e_3d_view' is ignored");
      return;
    }

    for (const Json::Value &data_class : command["payload"]["dataClasses"]) {
      if (!NameVersionArgumentsCheck(data_class)) {
        error(__FILE__, "member wasn't found. Event 'e_3d_view' is ignored");
        return;
      }
      std::vector<std::string> arguments;
      for (const Json::Value &argument : data_class["arguments"]) {
        arguments.push_back(argument.asString());
      }
      next_ui_update_req_fwd(
          {data_class["name"].asString(), data_class["version"].asString(), command["source"].asString(), arguments},
          [](std::string ret) { debug(__FILE__, ret.c_str()); });
    }
  } break;

  case EventType::e_shutdown: {
    next_ui_update_req_fwd({"Shutdown", "1.0.0", command["source"].asString(), std::vector<std::string>()},
                           [](const std::string &) { /* do nothing */ });
  } break;

  case EventType::e_general_widget_event: {

    if (!command["payload"].isMember("dataClass") || command["payload"]["dataClass"].isArray()) {
      error(__FILE__, "input doesn't contain valid 'dataClass'. Event 'e_general_widget_event' is "
                      "ignored");
      return;
    }
    Json::Value data_class = command["payload"]["dataClass"];
    if (!NameVersionArgumentsCheck(data_class)) {
      error(__FILE__, "member wasn't found. Event 'e_general_widget_event' is ignored");
      return;
    }
    std::vector<std::string> arguments;
    for (const Json::Value &argument : data_class["arguments"]) {
      arguments.push_back(argument.asString());
    }
    next_ui_update_req_fwd(
        {data_class["name"].asString(), data_class["version"].asString(), command["source"].asString(), arguments},
        [](std::string ret) { debug(__FILE__, "{}", ret); });

  } break;

  case EventType::e_widget_startup_command_changed: {
    if (!command["payload"].isMember("dataClasses") || !command["payload"]["dataClasses"].isArray()) {
      error(__FILE__, "input doesn't contain valid 'dataClass'. Event "
                      "'e_widget_startup_command_changed' is ignored");

      return;
    }
    for (const Json::Value &data_class : command["payload"]["dataClasses"]) {
      if (!NameVersionArgumentsCheck(data_class)) {
        error(__FILE__, "member wasn't found. Event 'e_widget_startup_command_changed' is ignored");
        return;
      }

      std::vector<std::string> arguments;
      for (const Json::Value &argument : data_class["arguments"]) {
        arguments.push_back(argument.asString());
      }
      next_ui_update_req_fwd(
          {data_class["name"].asString(), data_class["version"].asString(), command["source"].asString(), arguments},
          [](std::string ret) { info(__FILE__, ret.c_str()); });
    }
  } break;
  case EventType::e_widget_requests_data_stream_update_event: {
    if (!command["payload"].isMember("dataClass") || command["payload"]["dataClass"].isArray() ||
        !command["payload"].isMember("arguments")) {
      error(__FILE__, "input doesn't contain valid 'dataClass' or arguments . Event "
                      "'e_widget_requests_data_stream_update_event' is ignored");
      return;
    }
    const Json::Value &data_class = command["payload"]["dataClass"];
    if (!NameVersionArgumentsCheck(data_class, false)) {
      error(__FILE__, "member wasn't found. Event 'e_widget_requests_data_stream_update_event' is "
                      "ignored");
      return;
    }
    const Json::Value &arguments = command["payload"]["arguments"];
    if (!arguments.isString() && !arguments.isArray()) {
      error(__FILE__, "invalid argument. Event 'e_widget_requests_data_stream_update_event' is "
                      "ignored");
      return;
    }

    if (arguments.isString()) {
      next_ui_update_req_fwd({data_class["name"].asString(),
                              data_class["version"].asString(),
                              command["source"].asString(),
                              {arguments.asString()}},
                             [](std::string ret) { debug(__FILE__, ret.c_str()); });
    } else if (arguments.isArray()) {
      std::vector<std::string> line;
      for (const Json::Value &argument : arguments) {
        if (!argument.isString()) {
          error(__FILE__, "one of the argument is not a string. Event "
                          "'e_widget_requests_data_stream_update_event' is ignored");
          return;
        }
        line.push_back(argument.asString());
      }
      next_ui_update_req_fwd(
          {data_class["name"].asString(), data_class["version"].asString(), command["source"].asString(), line},
          [](std::string ret) { info(__FILE__, ret.c_str()); });
    }
  } break;
  default:
    error(__FILE__, "{0} event wasn't found", command["event"].asString());
    break;
  }
}

} // namespace ui_request
} // namespace databridge
} // namespace next

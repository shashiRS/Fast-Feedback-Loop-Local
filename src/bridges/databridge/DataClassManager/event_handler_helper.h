/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     data_request_server.h
 *  @brief    Functions for receiving, processing and handling the input events
 */

#ifndef NEXT_EVENT_HANDLER_REQUEST_SERVER_H_
#define NEXT_EVENT_HANDLER_REQUEST_SERVER_H_

#include <mutex>
#include <string>

#include <json/json.h>

#include "data_class_manager.h"

#include "next_ui_config_request_handler.h"
#include "next_ui_update_request_handler.h"

namespace next {
namespace databridge {

class DataClassManager;

namespace ui_request {

//! TODO: exchange this with a std::map<std::string, string_code> and use it in StringToEventType() instead of if ...
//! else ...
enum class EventType {
  e_used_data_stream_uris_changed,
  e_user_requested_data_stream_format,
  e_3d_view,
  e_shutdown,
  e_general_widget_event,
  e_widget_startup_command_changed,
  e_widget_requests_data_stream_update_event,
  e_user_pressed_signal_update_button,
  e_user_pressed_signal_search_button,
  e_user_requested_available_data_classes,
  e_used_data_classes_changed,
  e_backend_config_changed,
  e_none
};

static const std::map<std::string, EventType> stringToEvent = {
    {"UsedDataStreamUrisChanged", EventType::e_used_data_stream_uris_changed},
    {"UserRequestedDataStreamFormat", EventType::e_user_requested_data_stream_format},
    {"UserPressedSignalUpdateButton", EventType::e_user_pressed_signal_update_button},
    {"UserPressedSignalSearchButton", EventType::e_user_pressed_signal_search_button},
    {"3DView", EventType::e_3d_view},
    {"Shutdown", EventType::e_shutdown},
    {"GeneralWidgetEvent", EventType::e_general_widget_event},
    {"WidgetStartUpCommandChanged", EventType::e_widget_startup_command_changed},
    {"WidgetRequestsDataStreamUpdateEvent", EventType::e_widget_requests_data_stream_update_event},
    {"UserRequestedAvailableDataClasses", EventType::e_user_requested_available_data_classes},
    {"UsedDataClassesChanged", EventType::e_used_data_classes_changed},
    {"UserUpdatedBackendConfiguration", EventType::e_backend_config_changed}};

/*!
 * EventHandlerHelper is a helper class to handle events
 */
class EventHandlerHelper {
public:
  //! Constructor
  EventHandlerHelper();
  //! Function to handle events
  /*!
   * Checks if m_requestQueue is not empty,
   * And if it's so it handles its requests.
   * @param command correponds to the request in JSON format
   * @param sp_data_class_manager shared pointer to DataClassManager
   */
  void HandleEvent(Json::Value command, std::shared_ptr<DataClassManager> sp_data_class_manager);

  //! Function to check the validity of request
  /*!
   * check if the different field such as channel, event,source etc of input message is valid or not.
   * return false if not valid
   * @param command correponds to the request in JSON format
   * @return true if the fields of request are valid else false
   */
  bool CheckNextUiEvent(Json::Value command);

  //! converts the string corresponding to some event to the enumerator
  /*!
   * @param in_string string coding the event.
   * @return enumerator which correponds to the event.
   */
  static EventType StringToEventType(std::string const &in_string);

  //! A destructor.
  virtual ~EventHandlerHelper() = default;

private:
  //! Function to check event type
  /*!
   * check if the event type is of type "config update"
   * return false if not valid
   * @param event_enum event type
   * @return true if the event type is of type "config update"
   */
  bool IsUpdateRequest(const EventType &event_enum);

  //! Function to check event type
  /*!
   * check if the event type is of type "data class request"
   * return false if not valid
   * @param event_enum event type
   * @return true if the event type is of type "data class request"
   */
  bool IsDataClassRequest(const EventType &event_enum);

  //! Function to check event type
  /*!
   * check if the event type is of type "backend config request"
   * return false if not valid
   * @param event_enum event type
   * @return true if the event type is of type "backend config request"
   */
  bool IsBackendConfigRequest(const EventType &event_enum);
  Json::StreamWriterBuilder builder_;

  ui_request::NextUiConfigRequestHandler config_request_handler_;
  ui_request::NextUiUpdateRequestHandler update_request_handler_;
};

} // namespace ui_request
} // namespace databridge
} // namespace next

#endif // NEXT_EVENT_HANDLER_REQUEST_SERVER_H_

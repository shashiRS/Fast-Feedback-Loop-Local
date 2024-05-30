/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: Next_SDK
 * ========================== NEXT Project ==================================
 */
/**
 * @file    status_code_event.hpp
 * @brief   StatusCodeEvent is the class, publishes the statuscode
                        and receives on same event
 **/

#ifndef NEXT_SDK_STATUS_CODE_EVENT_H_
#define NEXT_SDK_STATUS_CODE_EVENT_H_

#include <string>

#include <next/sdk/logger/logger.hpp>
#include "../event_base/base_event_binary.h"
#include "../sdk_macros.h"

namespace next {
namespace sdk {
namespace events {

enum class StatusSeverity { SEV_DEFAULT = 0, SEV_INFO = 1, SEV_SUCCESS = 2, SEV_WARNING = 3, SEV_ERROR = 4 };
enum class StatusOrigin { DEFAULT = 1, NEXT_DATABRIDGE = 2, NEXT_CONTROLBRIDGE = 3, NEXT_PLAYER = 4, SIM_NODE = 5 };

struct StatusCodeMessage {
  StatusOrigin origin = StatusOrigin::DEFAULT;
  StatusSeverity severity = StatusSeverity::SEV_DEFAULT;
  int code_counter = 0;
  std::string component_name;
  std::string message;

  StatusCodeMessage(){};
  StatusCodeMessage(int code) {
    if (code < 100 || code > 9999) {
      return;
      // invalid status_code, 3 or 4 digits required
    }
    this->origin = code / 1000 ? safeCastOrigin(code / 1000) : StatusOrigin::DEFAULT; // second digit
    this->severity = safeCastSeverity(code % 1000 / 100);                             // first digit
    this->code_counter = code % 100;                                                  // last two digits
  };

  StatusCodeMessage(int code, const std::string &component_name, const std::string &message) {
    if (code < 100 || code > 9999) {
      // invalid status_code, 3 or 4 digits required, set at least component name and message
      this->component_name = component_name;
      this->message = message;
      return;
    }
    this->origin = code / 1000 ? safeCastOrigin(code / 1000) : StatusOrigin::DEFAULT; // second digit
    this->severity = safeCastSeverity(code % 1000 / 100);                             // first digit
    this->code_counter = code % 100;                                                  // last two digits
    this->component_name = component_name;
    this->message = message;
  };

  StatusCodeMessage &operator=(int code) {
    if (code < 100 || code > 9999) {
      // invalid status_code, 3 or 4 digits required
      return *this;
    }
    this->origin = code / 1000 ? safeCastOrigin(code / 1000) : StatusOrigin::DEFAULT; // second digit
    this->severity = safeCastSeverity(code % 1000 / 100);                             // first digit
    this->code_counter = code % 100;                                                  // last two digits
    return *this;
  }

  int getStatusCode() const {
    int ret = static_cast<int>(this->severity) * 100;
    ret += static_cast<int>(this->code_counter);
    return ret;
  }

  int toInt() const {
    int ret = static_cast<int>(this->origin) * 1000;
    ret += static_cast<int>(this->severity) * 100;
    ret += static_cast<int>(this->code_counter);
    return ret;
  }

  std::string getSeverityAsString() const {
    switch (this->severity) {
    case StatusSeverity::SEV_ERROR:
      return "Error";
    case StatusSeverity::SEV_INFO:
      return "Info";
    case StatusSeverity::SEV_SUCCESS:
      return "Success";
    case StatusSeverity::SEV_WARNING:
      return "Warning";
    case StatusSeverity::SEV_DEFAULT:
    default:
      return "Default";
    }
  }

  bool operator==(const StatusCodeMessage &rhs) {
    bool isEqual = true;
    isEqual &= this->origin == rhs.origin;
    isEqual &= this->severity == rhs.severity;
    isEqual &= this->component_name == rhs.component_name;
    isEqual &= this->message == rhs.message;

    return isEqual;
  }

private:
  StatusSeverity safeCastSeverity(int code) {
    switch (code) {
    case static_cast<int>(StatusSeverity::SEV_DEFAULT):
    case static_cast<int>(StatusSeverity::SEV_INFO):
    case static_cast<int>(StatusSeverity::SEV_SUCCESS):
    case static_cast<int>(StatusSeverity::SEV_WARNING):
    case static_cast<int>(StatusSeverity::SEV_ERROR):
      return static_cast<StatusSeverity>(code);
    default:
      logger::warn(__FILE__, "Received invalid severity code {0}. Default value will be used.", code);
      return StatusSeverity::SEV_DEFAULT;
    }
  }

  StatusOrigin safeCastOrigin(int code) {
    switch (code) {
    case static_cast<int>(StatusOrigin::DEFAULT):
    case static_cast<int>(StatusOrigin::NEXT_CONTROLBRIDGE):
    case static_cast<int>(StatusOrigin::NEXT_DATABRIDGE):
    case static_cast<int>(StatusOrigin::NEXT_PLAYER):
    case static_cast<int>(StatusOrigin::SIM_NODE):
      return static_cast<StatusOrigin>(code);
    default:
      logger::warn(__FILE__, "Received invalid origin code {0}. Default value will be used.", code);
      return StatusOrigin::DEFAULT;
    }
  }
};

NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS
class DECLSPEC_nextsdk StatusCodeEvent : public BaseEventBinary<StatusCodeMessage> {
  NEXT_RESTORE_WARNINGS_WINDOWS

public:
  StatusCodeEvent(const std::string &name = "") : BaseEventBinary<StatusCodeMessage>(name) { initialize(); }
  static const std::string getStatusOriginString(const StatusCodeMessage &status_code);

protected:
  payload_type serializeEventPayload(const StatusCodeMessage &message) const override;
  StatusCodeMessage deserializeEventPayload(const payload_type &buf) const override;
};

} // namespace events
} // namespace sdk
} // namespace next
#endif // NEXT_SDK_STATUS_CODE_EVENT_H_
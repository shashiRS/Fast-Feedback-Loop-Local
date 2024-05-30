/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     request_config_value_from_server.h
 * @brief    event to request a specific value from the configuration server
 *
 **/
#ifndef NEXTSDK_REQUEST_CONFIG_VALUE_SERVER_H_
#define NEXTSDK_REQUEST_CONFIG_VALUE_SERVER_H_

#include <memory>

#include "../event_base/base_event_binary.h"
#include "../sdk_config.hpp"

namespace next {
namespace sdk {
namespace events {
/*!
 * @brief data Request Config event contains
 * @brief Structure of the NEXT Request Config Information messages for nodes
 *
 * request_key config key requesting.
 * requester_name Name of the requester.
 */
struct RequestConfigValueFromServerInformationMessage {
  std::string request_key = "";
  std::string request_type = "";
  std::string request_default_value = "";
  std::string requester_name = "";
};

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4251)
#endif
class DECLSPEC_nextsdk RequestConfigValueFromServer
    : public next::sdk::events::BaseEventBinary<RequestConfigValueFromServerInformationMessage> {
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

public:
  RequestConfigValueFromServer(std::string name)
      : BaseEventBinary<RequestConfigValueFromServerInformationMessage>(name) {
    initialize();
  }

protected:
  payload_type serializeEventPayload(const RequestConfigValueFromServerInformationMessage &message) const override;
  RequestConfigValueFromServerInformationMessage deserializeEventPayload(const payload_type &buf) const override;
};

} // namespace events
} // namespace sdk
} // namespace next

#endif // NEXTSDK_REQUEST_CONFIG_VALUE_SERVER_H_

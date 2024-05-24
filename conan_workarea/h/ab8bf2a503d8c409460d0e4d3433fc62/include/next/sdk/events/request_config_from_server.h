/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     request_config.h
 * @brief    event to request nodes to send their config files to Next Control
 *
 **/
#ifndef NEXTSDK_REQUEST_CONFIG_SERVER_H_
#define NEXTSDK_REQUEST_CONFIG_SERVER_H_

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
 * TODO:
 * What it re If request_key is empty
 */
struct RequestConfigFromServerInformationMessage {
  std::string request_key = "";
  std::string requester_name = "";
};

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4251)
#endif
class DECLSPEC_nextsdk RequestConfigFromServer
    : public next::sdk::events::BaseEventBinary<RequestConfigFromServerInformationMessage> {
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

public:
  RequestConfigFromServer(std::string name) : BaseEventBinary<RequestConfigFromServerInformationMessage>(name) {
    initialize();
  }

protected:
  payload_type serializeEventPayload(const RequestConfigFromServerInformationMessage &message) const override;
  RequestConfigFromServerInformationMessage deserializeEventPayload(const payload_type &buf) const override;
};

} // namespace events
} // namespace sdk
} // namespace next

#endif // NEXTSDK_REQUEST_CONFIG_SERVER_H_

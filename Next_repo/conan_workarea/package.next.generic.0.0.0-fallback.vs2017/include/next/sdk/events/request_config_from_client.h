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
#ifndef NEXTSDK_REQUEST_CONFIG_CLIENT_H_
#define NEXTSDK_REQUEST_CONFIG_CLIENT_H_

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
 * request_key config key requested .
 * requested_componet Name of the componet requested.
 */
struct RequestConfigFromClientInformationMessage {
  std::string request_key = "";
  std::string requested_componet = "";
  std::string requester_name = "";
};

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4251)
#endif
class DECLSPEC_nextsdk RequestConfigFromClient
    : public next::sdk::events::BaseEventBinary<RequestConfigFromClientInformationMessage> {
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

public:
  RequestConfigFromClient(std::string name) : BaseEventBinary<RequestConfigFromClientInformationMessage>(name) {
    initialize();
  }

protected:
  payload_type serializeEventPayload(const RequestConfigFromClientInformationMessage &message) const override;
  RequestConfigFromClientInformationMessage deserializeEventPayload(const payload_type &buf) const override;
};

} // namespace events
} // namespace sdk
} // namespace next

#endif // NEXTSDK_REQUEST_CONFIG_CLIENT_H_

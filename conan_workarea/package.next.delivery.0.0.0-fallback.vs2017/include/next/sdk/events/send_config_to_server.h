/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     send_config.h
 * @brief    event emmitted by nodes to send configuration info to Next Control
 *
 **/
#ifndef NEXT_CORE_SDK_SEND_CONFIG_TO_SERVER_H_
#define NEXT_CORE_SDK_SEND_CONFIG_TO_SERVER_H_

#include <memory>
#include "../event_base/base_event_binary.h"
#include "../sdk_config.hpp"

namespace next {
namespace sdk {
namespace events {
/*!
 * @brief data the send event contains
 * @brief Structure of the NEXT Send Config Information messages for Next Control
 *
 * config_info Node configuration to send to Next Control.
 */
struct SendConfigToServerInformationMessage {
  std::string config_json = ""; // json would be a good solution
  std::string sender_componet_name = "";
};

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

class DECLSPEC_nextsdk SendConfigToServer
    : public next::sdk::events::BaseEventBinary<SendConfigToServerInformationMessage> {

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

public:
  SendConfigToServer(std::string name) : BaseEventBinary<SendConfigToServerInformationMessage>(name) { initialize(); }

protected:
  payload_type serializeEventPayload(const SendConfigToServerInformationMessage &message) const override;
  SendConfigToServerInformationMessage deserializeEventPayload(const payload_type &buf) const override;
};

} // namespace events
} // namespace sdk
} // namespace next

#endif // NEXT_CORE_SDK_SEND_CONFIG_TO_SERVER_H_

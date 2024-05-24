/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     send_config_to_client.h
 * @brief    event emmitted by nodes to send configuration info to Next Control
 *
 **/
#ifndef NEXT_CORE_SDK_SEND_CONFIG_H_
#define NEXT_CORE_SDK_SEND_CONFIG_H_

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
struct SendConfigToClientInformationMessage {
  std::string config_json = ""; // json would be a good solution
  std::string receiver_name = "";
  std::string sender_name = "";
  bool reset_active = false;
};

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

class DECLSPEC_nextsdk SendConfigToClient
    : public next::sdk::events::BaseEventBinary<SendConfigToClientInformationMessage> {

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

public:
  SendConfigToClient(std::string name) : BaseEventBinary<SendConfigToClientInformationMessage>(name) { initialize(); }

protected:
  payload_type serializeEventPayload(const SendConfigToClientInformationMessage &message) const override;
  SendConfigToClientInformationMessage deserializeEventPayload(const payload_type &buf) const override;
};

} // namespace events
} // namespace sdk
} // namespace next

#endif // NEXT_CORE_SDK_SEND_CONFIG_H_

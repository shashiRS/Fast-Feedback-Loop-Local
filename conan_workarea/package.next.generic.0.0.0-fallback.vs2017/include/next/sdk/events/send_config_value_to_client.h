/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     send_config_value_to_client.h
 * @brief    event emmitted by the config server with the single value os a config key
 *
 **/
#ifndef NEXT_CORE_SDK_SEND_CONFIG_VALUE_H_
#define NEXT_CORE_SDK_SEND_CONFIG_VALUE_H_

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
struct SendConfigValueToClientInformationMessage {
  std::string config_value = "";
  std::string receiver_name = "";
  std::string sender_name = "";
  std::string request_key = "";
  std::string request_type = "";
};

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

class DECLSPEC_nextsdk SendConfigValueToClient
    : public next::sdk::events::BaseEventBinary<SendConfigValueToClientInformationMessage> {

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

public:
  SendConfigValueToClient(std::string name) : BaseEventBinary<SendConfigValueToClientInformationMessage>(name) {
    initialize();
  }

protected:
  payload_type serializeEventPayload(const SendConfigValueToClientInformationMessage &message) const override;
  SendConfigValueToClientInformationMessage deserializeEventPayload(const payload_type &buf) const override;
};

} // namespace events
} // namespace sdk
} // namespace next

#endif // NEXT_CORE_SDK_SEND_CONFIG_VALUE_H_

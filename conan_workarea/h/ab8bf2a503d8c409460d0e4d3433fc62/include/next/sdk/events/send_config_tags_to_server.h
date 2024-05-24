/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     send_config_tags_to_Server.h
 * @brief    event emmitted by nodes to send configuration tag info to the config server
 *
 **/
#ifndef NEXT_CORE_SDK_SEND_CONFIG_TAGS_TO_SERVER_H_
#define NEXT_CORE_SDK_SEND_CONFIG_TAGS_TO_SERVER_H_

#include <memory>
#include "../event_base/base_event_binary.h"
#include "../sdk_config.hpp"

namespace next {
namespace sdk {
namespace events {
/*!
 * @brief data the send event contains
 * @brief Structure of the NEXT Send Config Taggs messages
 *
 * tags_json tags list to be sent to the config server.
 */
struct SendConfigTagsToServerInformationMessage {
  std::string tags_json = "";
  std::string sender_componet_name = "";
};

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4251)
#endif

class DECLSPEC_nextsdk SendConfigTagsToServer
    : public next::sdk::events::BaseEventBinary<SendConfigTagsToServerInformationMessage> {

#if defined(_MSC_VER)
#pragma warning(pop)
#endif

public:
  SendConfigTagsToServer(std::string name) : BaseEventBinary<SendConfigTagsToServerInformationMessage>(name) {}

protected:
  payload_type serializeEventPayload(const SendConfigTagsToServerInformationMessage &message) const override;
  SendConfigTagsToServerInformationMessage deserializeEventPayload(const payload_type &buf) const override;
};

} // namespace events
} // namespace sdk
} // namespace next

#endif // NEXT_CORE_SDK_SEND_CONFIG_TAGS_TO_SERVER_H_

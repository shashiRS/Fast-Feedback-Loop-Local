/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     send_config_to_client.cpp
 * @brief    event emmitted by nodes to send configuration info to Next Control
 *
 **/

#include <type_traits>
#include <vector>

#include <next/sdk/events/send_config.h>

namespace next {
namespace sdk {
namespace events {

next::sdk::events::payload_type
SendConfigToClient::serializeEventPayload(const SendConfigToClientInformationMessage &message) const {

  next::sdk::events::payload_type strbuf(std::ios_base::in | std::ios_base::out | std::ios_base::binary);

  size_t str_size = message.config_json.size();
  strbuf.sputn(reinterpret_cast<const char *>(&str_size), sizeof(str_size));
  strbuf.sputn(message.config_json.c_str(), str_size);
  return strbuf;
}

SendConfigToClientInformationMessage
SendConfigToClient::deserializeEventPayload(next::sdk::events::payload_type &strbuf) const {
  SendConfigToClientInformationMessage message{};
  if (strbuf.str().size() > 0) {
    size_t str_size = 0;
    strbuf.sgetn(reinterpret_cast<char *>(&str_size), sizeof(str_size));
    char *tmp = new char[str_size];
    strbuf.sgetn(tmp, str_size);
    message.config_json.append(tmp, str_size);
    delete[] tmp;
  }

  return message;
}

} // namespace events
} // namespace sdk
} // namespace next

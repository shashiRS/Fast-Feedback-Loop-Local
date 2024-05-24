/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     request_config_from_client_test.cpp
 * @brief
 *
 **/

#include <cip_test_support/gtest_reporting.h>

#include <next/sdk/events/request_config_from_client.h>
#include <next/sdk/sdk.hpp>
#include "event_manager.h"

namespace next {
namespace sdk {
namespace events {

class RequestConfigFromClientFixture : public testing::TestUsingEcal {
public:
  RequestConfigFromClientFixture() {
    logger::register_to_init_chain();
    this->instance_name_ = "RequestConfigFromClientFixture";
  }
};

class RequestConfigFixture : public RequestConfigFromClient {
public:
  RequestConfigFixture(std::string name) : RequestConfigFromClient(name) {}

  next::sdk::events::payload_type serialize(const RequestConfigFromClientInformationMessage &message) const {
    return serializeEventPayload(message);
  }

  RequestConfigFromClientInformationMessage deserialize(next::sdk::events::payload_type &buf) const {
    return deserializeEventPayload(buf);
  }
};

//! Test to serialize a message and deserialize it
TEST_F(RequestConfigFromClientFixture, de_serializeEventPayloadTest) {
  RequestConfigFixture bin_event("NextRequestConfigFromClient_de_serializeEventPayloadTest");
  RequestConfigFromClientInformationMessage initial_message, final_message;
  initial_message.request_key = "player.port";
  initial_message.requested_componet = "next_player";
  next::sdk::events::payload_type mess_buf = bin_event.serialize(initial_message);
  final_message = bin_event.deserialize(mess_buf);

  EXPECT_EQ(final_message.request_key, initial_message.request_key);
  EXPECT_EQ(final_message.requested_componet, initial_message.requested_componet);
}
TEST_F(RequestConfigFromClientFixture, de_serializeEventPayloadTestEmptyStrBuffer) {
  RequestConfigFixture bin_event("NextRequestConfigFromClient_de_serializeEventPayloadTestEmptyStrBuffer");
  RequestConfigFromClientInformationMessage final_message;
  next::sdk::events::payload_type mess_buf;
  final_message = bin_event.deserialize(mess_buf);
  EXPECT_EQ(final_message.request_key, "");
  EXPECT_EQ(final_message.requested_componet, "");
}

TEST_F(RequestConfigFromClientFixture, de_serializeEventPayloadTestWrongJson) {
  RequestConfigFixture bin_event("NextRequestConfigFromClient_de_serializeEventPayloadTestWrongJson");
  RequestConfigFromClientInformationMessage final_message;
  next::sdk::events::payload_type mess_buf = StringToPayload("{\"value\":3}");
  final_message = bin_event.deserialize(mess_buf);
  EXPECT_EQ(final_message.request_key, "");
  EXPECT_EQ(final_message.requested_componet, "");
}
TEST_F(RequestConfigFromClientFixture, serializeEventPayloadTestEmptyKey) {
  RequestConfigFixture bin_event("NextRequestConfigFromClient_serializeEventPayloadTestEmptyKey");
  RequestConfigFromClientInformationMessage initial_message, final_message;
  initial_message.request_key = "";
  initial_message.requested_componet = "";
  next::sdk::events::payload_type mess_buf = bin_event.serialize(initial_message);
  final_message = bin_event.deserialize(mess_buf);

  EXPECT_EQ(final_message.request_key, initial_message.request_key);
  EXPECT_EQ(final_message.requested_componet, initial_message.requested_componet);
}
TEST_F(RequestConfigFromClientFixture, publishTest) {
  // Test to check the linkage of publish function
  RequestConfigFixture ver_event("NextRequestConfigFromClient_publishTest");
  RequestConfigFromClientInformationMessage message;
  std::list<std::string> response;
  message.request_key = "Player.Port";
  message.requested_componet = "next_player";
  ver_event.publish(message, response);
}

TEST_F(RequestConfigFromClientFixture, getNumberOfListenersTest) {
  // Test to check the linkage of getNumberOfListeners function
  RequestConfigFixture ver_event("NextRequestConfigFromClient_getNumberOfListenersTest");
  EXPECT_EQ(ver_event.getNumberOfListeners(), 1);
}
} // namespace events
} // namespace sdk
} // namespace next

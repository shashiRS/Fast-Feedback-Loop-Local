/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     request_config_from_server_test.cpp
 * @brief
 *
 **/

#include <cip_test_support/gtest_reporting.h>

#include <next/sdk/events/request_config_from_server.h>
#include <next/sdk/sdk.hpp>
#include "event_manager.h"

namespace next {
namespace sdk {
namespace events {

class RequestConfigFromServerFixture : public testing::TestUsingEcal {
public:
  RequestConfigFromServerFixture() {
    logger::register_to_init_chain();
    this->instance_name_ = "RequestConfigFromServerFixture";
  }
};

class RequestConfigFixture : public RequestConfigFromServer {
public:
  RequestConfigFixture(std::string name) : RequestConfigFromServer(name) {}

  next::sdk::events::payload_type serialize(const RequestConfigFromServerInformationMessage &message) const {
    return serializeEventPayload(message);
  }

  RequestConfigFromServerInformationMessage deserialize(next::sdk::events::payload_type &buf) const {
    return deserializeEventPayload(buf);
  }
};

//! Test to serialize a message and deserialize it
TEST_F(RequestConfigFromServerFixture, de_serializeEventPayloadTest) {
  RequestConfigFixture bin_event("RequestConfigFromServerFixture_de_serializeEventPayloadTest");
  RequestConfigFromServerInformationMessage initial_message, final_message;
  initial_message.request_key = "player.port";
  initial_message.requester_name = "next_player";
  next::sdk::events::payload_type mess_buf = bin_event.serialize(initial_message);
  final_message = bin_event.deserialize(mess_buf);

  EXPECT_EQ(final_message.request_key, initial_message.request_key);
  EXPECT_EQ(final_message.requester_name, initial_message.requester_name);
}
TEST_F(RequestConfigFromServerFixture, de_serializeEventPayloadTestEmptyStrBuffer) {
  RequestConfigFixture bin_event("RequestConfigFromServerFixture_de_serializeEventPayloadTestEmptyStrBuffer");
  RequestConfigFromServerInformationMessage final_message;
  next::sdk::events::payload_type mess_buf;
  final_message = bin_event.deserialize(mess_buf);
  EXPECT_EQ(final_message.request_key, "");
  EXPECT_EQ(final_message.requester_name, "");
}
TEST_F(RequestConfigFromServerFixture, de_serializeEventPayloadTestWrongJson) {
  RequestConfigFixture bin_event("RequestConfigFromServerFixture_de_serializeEventPayloadTestWrongJson");
  RequestConfigFromServerInformationMessage final_message;
  next::sdk::events::payload_type mess_buf = StringToPayload("{\"value\":3}");
  final_message = bin_event.deserialize(mess_buf);
  EXPECT_EQ(final_message.request_key, "");
  EXPECT_EQ(final_message.requester_name, "");
}

TEST_F(RequestConfigFromServerFixture, serializeEventPayloadTestEmptyKey) {
  RequestConfigFixture bin_event("RequestConfigFromServerFixture_serializeEventPayloadTestEmptyKey");
  RequestConfigFromServerInformationMessage initial_message, final_message;
  initial_message.request_key = "";
  initial_message.requester_name = "";
  next::sdk::events::payload_type mess_buf = bin_event.serialize(initial_message);
  final_message = bin_event.deserialize(mess_buf);

  EXPECT_EQ(final_message.request_key, initial_message.request_key);
  EXPECT_EQ(final_message.requester_name, initial_message.requester_name);
}

TEST_F(RequestConfigFromServerFixture, publishTest) {
  // Test to check the linkage of publish function
  RequestConfigFixture ver_event("RequestConfigFromServerFixture_publishTest");
  RequestConfigFromServerInformationMessage message;
  std::list<std::string> response;
  message.request_key = "Player.Port";
  message.requester_name = "next_player";
  ver_event.publish(message, response);
}

TEST_F(RequestConfigFromServerFixture, getNumberOfListenersTest) {
  // Test to check the linkage of getNumberOfListeners function
  RequestConfigFixture ver_event("RequestConfigFromServerFixture_getNumberOfListenersTest");
  EXPECT_EQ(ver_event.getNumberOfListeners(), 1);
}
} // namespace events
} // namespace sdk
} // namespace next

/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     request_config_value_from_server_test.cpp
 * @brief
 *
 **/

#include <cip_test_support/gtest_reporting.h>

#include <next/sdk/events/request_config_value_from_server.h>
#include <next/sdk/sdk.hpp>
#include "event_manager.h"

namespace next {
namespace sdk {
namespace events {

class RequestConfigValueFromServerFixture : public testing::TestUsingEcal {
public:
  RequestConfigValueFromServerFixture() {
    logger::register_to_init_chain();
    this->instance_name_ = "RequestConfigValueFromServerFixture";
  }
};

class RequestConfigFixture : public RequestConfigValueFromServer {
public:
  RequestConfigFixture(std::string name) : RequestConfigValueFromServer(name) {}

  next::sdk::events::payload_type serialize(const RequestConfigValueFromServerInformationMessage &message) const {
    return serializeEventPayload(message);
  }

  RequestConfigValueFromServerInformationMessage deserialize(next::sdk::events::payload_type &buf) const {
    return deserializeEventPayload(buf);
  }
};

//! Test to serialize a message and deserialize it
TEST_F(RequestConfigValueFromServerFixture, de_serializeEventPayloadTest) {
  RequestConfigFixture bin_event("RequestConfigValueFromServerFixture_de_serializeEventPayloadTest");
  RequestConfigValueFromServerInformationMessage initial_message, final_message;
  initial_message.requester_name = "player";
  initial_message.request_default_value = "1";
  initial_message.request_key = "comp.test";
  initial_message.request_type = "int";
  next::sdk::events::payload_type mess_buf = bin_event.serialize(initial_message);
  final_message = bin_event.deserialize(mess_buf);

  EXPECT_EQ(final_message.request_key, initial_message.request_key);
  EXPECT_EQ(final_message.requester_name, initial_message.requester_name);
}
TEST_F(RequestConfigValueFromServerFixture, de_serializeEventPayloadTestEmptyStrBuffer) {
  RequestConfigFixture bin_event("RequestConfigValueFromServerFixture_de_serializeEventPayloadTestEmptyStrBuffer");
  RequestConfigValueFromServerInformationMessage final_message;
  next::sdk::events::payload_type mess_buf;
  final_message = bin_event.deserialize(mess_buf);
  EXPECT_EQ(final_message.request_key, "");
  EXPECT_EQ(final_message.request_default_value, "");
  EXPECT_EQ(final_message.request_type, "");
  EXPECT_EQ(final_message.requester_name, "");
}
TEST_F(RequestConfigValueFromServerFixture, de_serializeEventPayloadTestWrongJson) {
  RequestConfigFixture bin_event("RequestConfigValueFromServerFixture_de_serializeEventPayloadTestWrongJson");
  RequestConfigValueFromServerInformationMessage final_message;
  next::sdk::events::payload_type mess_buf = StringToPayload("{\"value\":3}");
  final_message = bin_event.deserialize(mess_buf);
  EXPECT_EQ(final_message.request_key, "");
  EXPECT_EQ(final_message.request_default_value, "");
  EXPECT_EQ(final_message.request_type, "");
  EXPECT_EQ(final_message.requester_name, "");
}

TEST_F(RequestConfigValueFromServerFixture, serializeEventPayloadTestEmptyKey) {
  RequestConfigFixture bin_event("RequestConfigValueFromServerFixture_serializeEventPayloadTestEmptyKey");
  RequestConfigValueFromServerInformationMessage initial_message, final_message;
  initial_message.requester_name = "";
  initial_message.request_default_value = "";
  initial_message.request_key = "";
  initial_message.request_type = "";
  next::sdk::events::payload_type mess_buf = bin_event.serialize(initial_message);
  final_message = bin_event.deserialize(mess_buf);

  EXPECT_EQ(final_message.request_key, initial_message.request_key);
  EXPECT_EQ(final_message.requester_name, initial_message.requester_name);
}

TEST_F(RequestConfigValueFromServerFixture, publishTest) {
  // Test to check the linkage of publish function
  RequestConfigFixture ver_event("RequestConfigValueFromServerFixture_publishTest");
  RequestConfigValueFromServerInformationMessage message;
  std::list<std::string> response;
  message.requester_name = "player";
  message.request_default_value = "1";
  message.request_key = "comp.test";
  message.request_type = "int";
  ver_event.publish(message, response);
}

TEST_F(RequestConfigValueFromServerFixture, getNumberOfListenersTest) {
  // Test to check the linkage of getNumberOfListeners function
  RequestConfigFixture ver_event("RequestConfigValueFromServerFixture_getNumberOfListenersTest");
  EXPECT_EQ(ver_event.getNumberOfListeners(), 1);
}
} // namespace events
} // namespace sdk
} // namespace next

/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     send_config_to_client_test.cpp
 * @brief
 *
 **/

#include <cip_test_support/gtest_reporting.h>

#include <next/sdk/events/send_config_to_client.h>
#include <next/sdk/sdk.hpp>

namespace next {
namespace sdk {
namespace events {

class SendConfigToClientFixture : public testing::TestUsingEcal {
public:
  SendConfigToClientFixture() {
    logger::register_to_init_chain();
    this->instance_name_ = "SendConfigToClientFixture";
  }
};

class SendConfigFixture : public SendConfigToClient {
public:
  SendConfigFixture(std::string name) : SendConfigToClient(name) {}

  next::sdk::events::payload_type serialize(const SendConfigToClientInformationMessage &message) const {
    return serializeEventPayload(message);
  }

  SendConfigToClientInformationMessage deserialize(next::sdk::events::payload_type &buf) const {
    return deserializeEventPayload(buf);
  }
};

//! Test to serialize a message and deserialize it
TEST_F(SendConfigToClientFixture, de_serializeEventPayloadTest) {
  SendConfigFixture bin_event("ConfigToClient_de_serializeEventPayloadTest");
  SendConfigToClientInformationMessage initial_message, final_message;
  initial_message.config_json = "player config";
  initial_message.receiver_name = "player config";
  initial_message.sender_name = "sender_name";
  initial_message.reset_active = true;
  next::sdk::events::payload_type mess_buf = bin_event.serialize(initial_message);
  final_message = bin_event.deserialize(mess_buf);

  EXPECT_EQ(final_message.config_json, initial_message.config_json);
  EXPECT_EQ(final_message.receiver_name, initial_message.receiver_name);
  EXPECT_EQ(final_message.sender_name, initial_message.sender_name);
  EXPECT_EQ(final_message.reset_active, initial_message.reset_active);
}
TEST_F(SendConfigToClientFixture, de_serializeEventPayloadTestEmptyStrBuffer) {
  SendConfigFixture bin_event("ConfigToClient_de_serializeEventPayloadTestEmptyStrBuffer");
  SendConfigToClientInformationMessage final_message;
  next::sdk::events::payload_type mess_buf;
  final_message = bin_event.deserialize(mess_buf);
  EXPECT_EQ(final_message.config_json, "");
  EXPECT_EQ(final_message.receiver_name, "");
  EXPECT_EQ(final_message.sender_name, "");
  EXPECT_EQ(final_message.reset_active, false);
}
TEST_F(SendConfigToClientFixture, de_serializeEventPayloadTestWrongJson) {
  SendConfigFixture bin_event("ConfigToClient_de_serializeEventPayloadTestWrongJson");
  SendConfigToClientInformationMessage final_message;
  next::sdk::events::payload_type mess_buf = StringToPayload("{\"value\":3}");
  final_message = bin_event.deserialize(mess_buf);
  EXPECT_EQ(final_message.config_json, "");
  EXPECT_EQ(final_message.receiver_name, "");
  EXPECT_EQ(final_message.sender_name, "");
  EXPECT_EQ(final_message.reset_active, false);
}
TEST_F(SendConfigToClientFixture, serializeEventPayloadTestEmptyKey) {
  SendConfigFixture bin_event("ConfigToClient_serializeEventPayloadTestEmptyKey");
  SendConfigToClientInformationMessage initial_message, final_message;
  initial_message.config_json = "";
  initial_message.receiver_name = "";
  initial_message.sender_name = "";
  initial_message.reset_active = false;
  next::sdk::events::payload_type mess_buf = bin_event.serialize(initial_message);
  final_message = bin_event.deserialize(mess_buf);

  EXPECT_EQ(final_message.config_json, initial_message.config_json);
  EXPECT_EQ(final_message.receiver_name, initial_message.receiver_name);
  EXPECT_EQ(final_message.sender_name, initial_message.sender_name);
  EXPECT_EQ(final_message.reset_active, initial_message.reset_active);
}
} // namespace events
} // namespace sdk
} // namespace next

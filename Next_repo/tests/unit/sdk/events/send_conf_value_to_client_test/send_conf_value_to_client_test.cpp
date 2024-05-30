/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     send_config_value_to_client_test.cpp
 * @brief
 *
 **/

#include <cip_test_support/gtest_reporting.h>

#include <next/sdk/events/send_config_value_to_client.h>
#include <next/sdk/sdk.hpp>

namespace next {
namespace sdk {
namespace events {

class SendConfigValueToClientFixture : public testing::TestUsingEcal {
public:
  SendConfigValueToClientFixture() {
    logger::register_to_init_chain();
    this->instance_name_ = "SendConfigValueToClientFixture";
  }
};

class SendConfigFixture : public SendConfigValueToClient {
public:
  SendConfigFixture(std::string name) : SendConfigValueToClient(name) {}

  next::sdk::events::payload_type serialize(const SendConfigValueToClientInformationMessage &message) const {
    return serializeEventPayload(message);
  }

  SendConfigValueToClientInformationMessage deserialize(next::sdk::events::payload_type &buf) const {
    return deserializeEventPayload(buf);
  }
};

//! Test to serialize a message and deserialize it
TEST_F(SendConfigValueToClientFixture, de_serializeEventPayloadTest) {
  SendConfigFixture bin_event("ConfigValueToClient_de_serializeEventPayloadTest");
  SendConfigValueToClientInformationMessage initial_message, final_message;
  initial_message.config_value = "player config";
  initial_message.receiver_name = "player config";
  next::sdk::events::payload_type mess_buf = bin_event.serialize(initial_message);
  final_message = bin_event.deserialize(mess_buf);

  EXPECT_EQ(final_message.config_value, initial_message.config_value);
  EXPECT_EQ(final_message.receiver_name, initial_message.receiver_name);
}
TEST_F(SendConfigValueToClientFixture, de_serializeEventPayloadTestEmptyStrBuffer) {
  SendConfigFixture bin_event("ConfigValueToClient_de_serializeEventPayloadTestEmptyStrBuffer");
  SendConfigValueToClientInformationMessage final_message;
  next::sdk::events::payload_type mess_buf;
  final_message = bin_event.deserialize(mess_buf);
  EXPECT_EQ(final_message.config_value, "");
  EXPECT_EQ(final_message.receiver_name, "");
}
TEST_F(SendConfigValueToClientFixture, de_serializeEventPayloadTestWrongJson) {
  SendConfigFixture bin_event("ConfigValueToClient_de_serializeEventPayloadTestWrongJson");
  SendConfigValueToClientInformationMessage final_message;
  next::sdk::events::payload_type mess_buf = StringToPayload("{\"value\":3}");
  final_message = bin_event.deserialize(mess_buf);
  EXPECT_EQ(final_message.config_value, "");
  EXPECT_EQ(final_message.receiver_name, "");
}
TEST_F(SendConfigValueToClientFixture, serializeEventPayloadTestEmptyKey) {
  SendConfigFixture bin_event("ConfigValueToClient_serializeEventPayloadTestEmptyKey");
  SendConfigValueToClientInformationMessage initial_message, final_message;
  initial_message.config_value = "";
  initial_message.receiver_name = "";
  next::sdk::events::payload_type mess_buf = bin_event.serialize(initial_message);
  final_message = bin_event.deserialize(mess_buf);

  EXPECT_EQ(final_message.config_value, initial_message.config_value);
  EXPECT_EQ(final_message.receiver_name, initial_message.receiver_name);
}
} // namespace events
} // namespace sdk
} // namespace next

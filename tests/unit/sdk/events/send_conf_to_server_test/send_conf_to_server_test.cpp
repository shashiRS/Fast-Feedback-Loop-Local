/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     send_config_to_server_test.cpp
 * @brief
 *
 **/

#include <cip_test_support/gtest_reporting.h>

#include <next/sdk/events/send_config_to_server.h>
#include <next/sdk/sdk.hpp>

namespace next {
namespace sdk {
namespace events {

class SendConfigToServerFixture : public testing::TestUsingEcal {
public:
  SendConfigToServerFixture() {
    logger::register_to_init_chain();
    this->instance_name_ = "SendConfigToServerFixture";
  }
};

class OverwriteConfigFixture : public SendConfigToServer {
public:
  OverwriteConfigFixture(std::string name) : SendConfigToServer(name) {}

  next::sdk::events::payload_type serialize(const SendConfigToServerInformationMessage &message) const {
    return serializeEventPayload(message);
  }

  SendConfigToServerInformationMessage deserialize(next::sdk::events::payload_type &buf) const {
    return deserializeEventPayload(buf);
  }
};

//! Test to serialize a message and deserialize it
TEST_F(SendConfigToServerFixture, de_serializeEventPayloadTest) {
  OverwriteConfigFixture bin_event("ConfigToServer_de_serializeEventPayloadTest");
  SendConfigToServerInformationMessage initial_message, final_message;
  initial_message.config_json = "player config";
  initial_message.sender_componet_name = "next_player";
  next::sdk::events::payload_type mess_buf = bin_event.serialize(initial_message);
  final_message = bin_event.deserialize(mess_buf);

  EXPECT_EQ(final_message.config_json, initial_message.config_json);
  EXPECT_EQ(final_message.sender_componet_name, initial_message.sender_componet_name);
}
TEST_F(SendConfigToServerFixture, de_serializeEventPayloadTestEmptyStrBuffer) {
  OverwriteConfigFixture bin_event("ConfigToServer_de_serializeEventPayloadTestEmptyStrBuffer");
  SendConfigToServerInformationMessage final_message;
  next::sdk::events::payload_type mess_buf;
  final_message = bin_event.deserialize(mess_buf);
  EXPECT_EQ(final_message.config_json, "");
  EXPECT_EQ(final_message.sender_componet_name, "");
}
TEST_F(SendConfigToServerFixture, de_serializeEventPayloadTestWrongJson) {
  OverwriteConfigFixture bin_event("ConfigToServer_de_serializeEventPayloadTestWrongJson");
  SendConfigToServerInformationMessage final_message;
  next::sdk::events::payload_type mess_buf = StringToPayload("{\"value\":3}");
  final_message = bin_event.deserialize(mess_buf);
  EXPECT_EQ(final_message.config_json, "");
  EXPECT_EQ(final_message.sender_componet_name, "");
}
TEST_F(SendConfigToServerFixture, serializeEventPayloadTestEmptyKey) {
  OverwriteConfigFixture bin_event("ConfigToServer_serializeEventPayloadTestEmptyKey");
  SendConfigToServerInformationMessage initial_message, final_message;
  initial_message.config_json = "";
  initial_message.sender_componet_name = "";
  next::sdk::events::payload_type mess_buf = bin_event.serialize(initial_message);
  final_message = bin_event.deserialize(mess_buf);

  EXPECT_EQ(final_message.config_json, initial_message.config_json);
  EXPECT_EQ(final_message.sender_componet_name, initial_message.sender_componet_name);
}
} // namespace events
} // namespace sdk
} // namespace next

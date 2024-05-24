/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     version_info_test.cpp
 * @brief
 *
 **/

#include <cip_test_support/gtest_reporting.h>

#include <next/sdk/events/version_info.h>
#include <next/sdk/sdk.hpp>

namespace next {
namespace sdk {
namespace events {

class VersionInfoTest : public testing::TestUsingEcal {
public:
  VersionInfoTest() { this->instance_name_ = "VersionInfoTest"; }
};

class TestingVersionInfo : public VersionInfo {

public:
  TestingVersionInfo(std::string name) : VersionInfo(name) {}

  next::sdk::events::payload_type serialize(const VersionInfoPackage &message) const {
    return serializeEventPayload(message);
  }
  VersionInfoPackage deserialize(next::sdk::events::payload_type &buf) const { return deserializeEventPayload(buf); }
};

TEST_F(VersionInfoTest, serializeAndDeserializeEventPayload_1) {

  TestingVersionInfo ver_event("VersionInfoEvent");
  VersionInfoPackage message;
  message.component_name = "Test event";
  message.component_version = "0.1.100";
  message.documentation_link = "test_link";

  next::sdk::events::payload_type mess_buf = ver_event.serialize(message);

  auto message_out = ver_event.deserialize(mess_buf);
  EXPECT_EQ(message.component_name, message_out.component_name);
  EXPECT_EQ(message.component_version, message_out.component_version);
  EXPECT_EQ(message.documentation_link, message_out.documentation_link);
}

TEST_F(VersionInfoTest, serializeAndDeserializeEventPayload_2) {

  TestingVersionInfo ver_event("VersionInfoEvent");
  VersionInfoPackage message;
  message.component_name = "TestForSerializationAndDeserializationVersionInfoEvents";
  message.component_version = "100.500.8000";
  message.documentation_link = "dudjjfjfjjfjjjshwdwhewurhuwehfriuhweifh";

  next::sdk::events::payload_type mess_buf = ver_event.serialize(message);

  auto message_out = ver_event.deserialize(mess_buf);
  EXPECT_EQ(message.component_name, message_out.component_name);
  EXPECT_EQ(message.component_version, message_out.component_version);
  EXPECT_EQ(message.documentation_link, message_out.documentation_link);
}

TEST_F(VersionInfoTest, publishVersionInfoEvent) {
  // Test to check publish is linking an callable
  TestingVersionInfo ver_event("VersionInfoEvent");
  VersionInfoPackage message;
  std::list<std::string> response;
  message.component_name = "TestForSerializationAndDeserializationVersionInfoEvents";
  message.component_version = "100.500.8000";
  message.documentation_link = "dudjjfjfjjfjjjshwdwhewurhuwehfriuhweifh";
  ver_event.publish(message, response);
}

TEST_F(VersionInfoTest, getNumberOfListenersTest) {
  // Test to check publish is linking an callable
  TestingVersionInfo ver_event("VersionInfoEvent");
  EXPECT_EQ(ver_event.getNumberOfListeners(), 0);
}

} // namespace events
} // namespace sdk
} // namespace next

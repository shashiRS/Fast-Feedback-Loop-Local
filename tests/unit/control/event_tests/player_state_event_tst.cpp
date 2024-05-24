/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: Next-Control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     player_state_event_tst.cpp
 * @brief
 *
 **/

#include <gtest/gtest.h>

#include <array>
#include <atomic>
#include <iostream>
#include <thread>

#include <ecal/ecal.h>

#include <next/sdk/sdk.hpp>

#include <next/control/event_types/player_state_event.h>

namespace next {
namespace control {
namespace events {

class PlayerStatusEventTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  PlayerStatusEventTestFixture() { this->instance_name_ = "PlayerStatusEventTestFixture"; }
};

//! flag for endless loop
std::atomic<bool> publishMessage(true);
std::atomic<bool> test_publisher_ready(false);
std::atomic<int> subscriber_hook_called(0);

void subscriber_hook() { subscriber_hook_called++; }

TEST_F(PlayerStatusEventTestFixture, DISABLED_multiple_subscribers) {

  // send one message
  PlayerStateEvent eventPublisher("");
  auto publisher_thread = std::thread([&] {
    eventPublisher._message.recording_name = "HelloWorld";
    eventPublisher._message.timestamp = 1234;
    while (publishMessage) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      eventPublisher.send();
      test_publisher_ready = true;
    }
  });
  while (!test_publisher_ready) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  eCAL::Util::EnableLoopback(true);
  PlayerStateEvent eventSubscriber1("");
  next::sdk::events::notify_hook hook = subscriber_hook;
  eventSubscriber1.addEventHook(hook);
  eventSubscriber1.subscribe();

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  PlayerStateEvent eventSubscriber2("");
  next::sdk::events::notify_hook hook2 = subscriber_hook;
  eventSubscriber2.addEventHook(hook2);
  eventSubscriber2.subscribe();

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  uint32_t timeout = 0;
  while (!subscriber_hook_called && timeout < 200) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    timeout++;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  // stop publisher thread
  publishMessage = false;
  publisher_thread.join();
  PlayerMetaInformationMessage received_msg_1;
  received_msg_1 = eventSubscriber1.getEventData();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  PlayerMetaInformationMessage received_msg_2;
  received_msg_2 = eventSubscriber2.getEventData();

  // check if header was also receivced
  EXPECT_TRUE(received_msg_1.timestamp == received_msg_2.timestamp);
}

TEST(PlayerStatusEvent, UpdateStatus) {

  PlayerStateEvent stateEvent("event");
  stateEvent._message.recording_name = "HelloWorld";
  stateEvent._message.min_timestamp = 1;
  stateEvent._message.max_timestamp = 100;
  stateEvent._message.speed_factor = 1.50;
  stateEvent._message.status = PlayerState::ENTER_OPENING;
  stateEvent._message.timestamp = 1234;
  stateEvent._message.stepping_type = "type";
  stateEvent._message.stepping_value = 1;
  auto mess_buf = stateEvent.serializeEventPayload(stateEvent._message);
  PlayerMetaInformationMessage received_msg = stateEvent.deserializeEventPayload(mess_buf);
}
TEST(PlayerStatusEvent, SerializeAndDesedrialize) {

  // send one message
  PlayerStateEvent stateEvent("event");
  stateEvent._message.recording_name = "HelloWorld";
  stateEvent._message.min_timestamp = 1;
  stateEvent._message.max_timestamp = 100;
  stateEvent._message.speed_factor = 1.50;
  stateEvent._message.status = PlayerState::ENTER_OPENING;
  stateEvent._message.timestamp = 1234;
  stateEvent._message.stepping_type = "type";
  stateEvent._message.stepping_value = 1;
  auto mess_buf = stateEvent.serializeEventPayload(stateEvent._message);
  PlayerMetaInformationMessage received_msg = stateEvent.deserializeEventPayload(mess_buf);

  EXPECT_TRUE(received_msg.max_timestamp == stateEvent._message.max_timestamp);
  EXPECT_TRUE(received_msg.min_timestamp == stateEvent._message.min_timestamp);
  EXPECT_TRUE(received_msg.recording_name == stateEvent._message.recording_name);
  EXPECT_TRUE(received_msg.speed_factor == stateEvent._message.speed_factor);
  EXPECT_TRUE(received_msg.status == stateEvent._message.status);
  EXPECT_TRUE(received_msg.stepping_type == stateEvent._message.stepping_type);
  EXPECT_TRUE(received_msg.stepping_value == stateEvent._message.stepping_value);
  EXPECT_TRUE(received_msg.timestamp == stateEvent._message.timestamp);

  stateEvent.updateStatus(PlayerState::ENTER_READY);
  mess_buf = stateEvent.serializeEventPayload(stateEvent._message);
  received_msg = stateEvent.deserializeEventPayload(mess_buf);

  EXPECT_TRUE(received_msg.status == PlayerState::ENTER_READY);
}

TEST(PlayerStatusEvent, SerializeAndDeserializeEmptyData) {
  PlayerStateEvent stateEvent("event");
  PlayerMetaInformationMessage empty_msg;
  auto ser_msg = stateEvent.serializeEventPayload(empty_msg);
  PlayerMetaInformationMessage orig_msg = stateEvent.deserializeEventPayload(ser_msg);
  EXPECT_TRUE(empty_msg == orig_msg);
}

TEST(PlayerStatusEvent, DeserializeBadDataJson) {
  PlayerStateEvent stateEvent("event");
  PlayerMetaInformationMessage empty_msg;
  PlayerMetaInformationMessage orig_msg =
      stateEvent.deserializeEventPayload(next::sdk::events::StringToPayload("TEMP=HELLO"));
  EXPECT_TRUE(empty_msg == orig_msg);
}

} // namespace events
} // namespace control
} // namespace next

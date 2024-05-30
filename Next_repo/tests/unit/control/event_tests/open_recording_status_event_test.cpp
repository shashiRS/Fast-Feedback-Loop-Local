/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: Next-Control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     open_recording_status_event_test.cpp
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

#include <next/control/event_types/open_recording_status_event.h>

namespace next {
namespace control {
namespace events {

class OpenRecordingStatusEventTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  OpenRecordingStatusEventTestFixture() { this->instance_name_ = "OpenRecordingStatusEventTestFixture"; }
};

TEST(OpenRecordingStatusEventTestFixture, SerializeAndDesedrialize) {

  // send one message
  OpenRecordingStatusEvent statusEvent("event");
  statusEvent.message_.executed_function = "functionName";
  statusEvent.message_.description = "description";
  statusEvent.message_.areIndexesUsed = false;
  statusEvent.message_.currentIndex = 1;
  statusEvent.message_.maxIndex = 2;
  next::sdk::events::payload_type mess_buf = statusEvent.serializeEventPayload(statusEvent.message_);
  OpenRecordingStatusInformationMessage received_msg = statusEvent.deserializeEventPayload(mess_buf);

  EXPECT_TRUE(received_msg.executed_function == statusEvent.message_.executed_function);
  EXPECT_TRUE(received_msg.description == statusEvent.message_.description);
  EXPECT_TRUE(received_msg.areIndexesUsed == statusEvent.message_.areIndexesUsed);
  EXPECT_TRUE(received_msg.currentIndex == statusEvent.message_.currentIndex);
  EXPECT_TRUE(received_msg.maxIndex == statusEvent.message_.maxIndex);
}

} // namespace events
} // namespace control
} // namespace next

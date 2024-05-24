/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     status_code_event_test.cpp
 *  @brief    Testing the status_code_event class
 */

#include <chrono>
#include <condition_variable>
#include <optional>
#include <thread>

#include <ecal/ecal.h>
#include <gtest/gtest.h>

#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>
#include <next/appsupport/next_status_code/next_status_code.hpp>
#include <next/sdk/events/status_code_event.hpp>
#include <next/sdk/sdk.hpp>

using next::sdk::InitChain;
using namespace std::chrono_literals;

class StatusCodeEventTest : public next::sdk::events::StatusCodeEvent {
public:
  StatusCodeEventTest(const std::string &name = "test") : StatusCodeEvent(name) {}

  next::sdk::events::payload_type serialize(const next::sdk::events::StatusCodeMessage &message) const {
    return serializeEventPayload(message);
  }

  next::sdk::events::StatusCodeMessage deserialize(const next::sdk::events::payload_type &buf) const {
    return deserializeEventPayload(buf);
  }
};

class StatusCodeEventTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  StatusCodeEventTestFixture() { this->instance_name_ = "StatusCodeEventTestFixture"; }
  StatusCodeEventTest status_code_event_test_;
};

TEST_F(StatusCodeEventTestFixture, pub_sub_status_code_event_test) {
  eCAL::Util::EnableLoopback(true);
  next::sdk::events::StatusCodeMessage status_code(504);
  status_code.component_name = "test";
  status_code.origin = next::sdk::events::StatusOrigin::SIM_NODE;
  status_code.message = "test msg";
  auto before_ser_data = status_code_event_test_.serialize(status_code);

  std::condition_variable wait_for_response_cv;
  std::mutex wait_for_response_mutex;
  std::atomic_bool wait_for_response_flag{false};

  std::atomic<bool> has_passed(false);
  status_code_event_test_.addEventHook([&]() {
    auto ser_data = status_code_event_test_.getEventData();
    if (status_code == ser_data) {
      has_passed = true;
    }
    wait_for_response_flag = true;
    wait_for_response_cv.notify_one();
  });

  status_code_event_test_.subscribe();
  status_code_event_test_.publish(status_code);

  constexpr int kTimeoutMs = 2000;

  std::unique_lock<std::mutex> lock(wait_for_response_mutex);
  wait_for_response_cv.wait_for(lock, std::chrono::milliseconds(kTimeoutMs), [&] {
    if (wait_for_response_flag)
      return true;
    return false;
  });
  lock.unlock();

  EXPECT_TRUE(wait_for_response_flag) << "callback wasn't hit and timer was timed out";
  EXPECT_EQ(true, has_passed);
}

TEST_F(StatusCodeEventTestFixture, serialize_deserialize_status_code_event_test) {
  next::sdk::events::StatusCodeMessage status_code(504);
  status_code.component_name = "test";
  status_code.origin = next::sdk::events::StatusOrigin::SIM_NODE;
  status_code.message = "test msg";

  auto before_ser_data = status_code_event_test_.serialize(status_code);
  auto code_after_deser = status_code_event_test_.deserialize(before_ser_data);

  EXPECT_EQ((code_after_deser == status_code), true);
}

TEST_F(StatusCodeEventTestFixture, wrong_status_code_data_deserialize_test) {
  next::sdk::events::StatusCodeMessage empty_status_code;
  std::string wrong_data_json_str =
      "{\"component_name\":\"abc\",\"origin\":\"4\",\"status_code\":\"four_o_four\",\"status_msg\":\"1213\"}";
  next::sdk::events::StatusCodeMessage code_after_deser =
      status_code_event_test_.deserialize(next::sdk::events::StringToPayload(wrong_data_json_str));

  EXPECT_EQ((empty_status_code == code_after_deser), true);
}

TEST_F(StatusCodeEventTestFixture, wrong_origin_data_deserialize_test) {
  next::sdk::events::StatusCodeMessage empty_status_code;
  std::string wrong_data_json_str = "{\"component_name\":\"test\",\"origin\":\"abc\",\"status_code\":\"404\",\"status_"
                                    "msg\":\"testing statuscodeevent\"}";

  next::sdk::events::StatusCodeMessage code_after_deser =
      status_code_event_test_.deserialize(next::sdk::events::StringToPayload(wrong_data_json_str));

  EXPECT_EQ((empty_status_code == code_after_deser), true);
}

TEST_F(StatusCodeEventTestFixture, missing_component_data_deserialize_test) {
  next::sdk::events::StatusCodeMessage empty_status_code;
  std::string wrong_data_json_str =
      "{\"component_name\":\"\",\"origin\":\"3\",\"status_code\":\"404\",\"status_msg\":\"testing statuscodeevent\"}";
  next::sdk::events::StatusCodeMessage code_after_deser =
      status_code_event_test_.deserialize(next::sdk::events::StringToPayload(wrong_data_json_str));

  EXPECT_EQ((empty_status_code == code_after_deser), true);
}

TEST_F(StatusCodeEventTestFixture, empty_str_data_deserialize_test) {
  next::sdk::events::StatusCodeMessage empty_status_code;
  std::string wrong_data_json_str = "";
  next::sdk::events::StatusCodeMessage code_after_deser =
      status_code_event_test_.deserialize(next::sdk::events::StringToPayload(wrong_data_json_str));

  EXPECT_EQ((empty_status_code == code_after_deser), true);
}

TEST_F(StatusCodeEventTestFixture, next_status_code_pub_sub_function_test) {
  next::sdk::events::StatusCodeEvent event;
  next::appsupport::ConfigClient::do_init("next_controlbridge");

  eCAL::Util::EnableLoopback(true);

  std::condition_variable wait_for_response_cv;
  std::mutex wait_for_response_mutex;
  std::atomic_bool wait_for_response_flag{false};

  event.addEventHook([&]() {
    wait_for_response_flag = true;
    wait_for_response_cv.notify_one();
  });
  event.subscribe();

  next::appsupport::StatusCode next_status_code = next::appsupport::StatusCode::ERR_COMP_INIT_FAILED;
  std::optional<next::sdk::events::StatusCodeMessage> ret_status =
      next::appsupport::NextStatusCode::PublishStatusCode(next_status_code, "init has failed");

  constexpr int kTimeoutMs = 2000;

  std::unique_lock<std::mutex> lock(wait_for_response_mutex);
  wait_for_response_cv.wait_for(lock, std::chrono::milliseconds(kTimeoutMs), [&] {
    if (wait_for_response_flag)
      return true;
    return false;
  });
  lock.unlock();

  EXPECT_TRUE(wait_for_response_flag) << "callback wasn't hit and timer was timed out";

  next::appsupport::ConfigClient::finish_init();
  next::appsupport::ConfigClient::do_reset();
}

/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     multiple_subscribes_test.cpp
 * @brief
 *
 **/

#include <cip_test_support/gtest_reporting.h>

#include <array>
#include <atomic>
#include <iostream>
#include <thread>

#ifdef _WIN32
#pragma warning(push, 0) // disable all warnings from ecal
#endif
#include <ecal/core/pb/monitoring.pb.h>
#ifdef _WIN32
#pragma warning(pop)
#endif
#include <ecal/ecal.h>

#include <next/sdk/event_base/base_event_binary.h>
#include <next/sdk/event_base/base_event_binary_blocking.h>
#include <next/sdk/sdk.hpp>
#include "event_manager.h"
namespace next {
namespace sdk {
namespace events {

struct TestMsg {
  int a = 0;
  int b = 0;
  std::string test = "";
};
class TestBinary : public BaseEventBinary<TestMsg> {
public:
  TestBinary(std::string name) : BaseEventBinary<TestMsg>(name){};
  ~TestBinary(){};
  TestMsg message_;
  payload_type serializeEventPayload(const TestMsg &message) const override {
    std::stringbuf strbuf(std::ios_base::in | std::ios_base::out | std::ios_base::binary);

    strbuf.sputn(reinterpret_cast<const char *>(&message.a), sizeof(message.a));
    strbuf.sputn(reinterpret_cast<const char *>(&message.b), sizeof(message.b));

    size_t str_size = message.test.size();
    strbuf.sputn(reinterpret_cast<const char *>(&str_size), sizeof(str_size));
    strbuf.sputn(message.test.c_str(), static_cast<long long>(str_size));
    return StringToPayload(strbuf.str());
  }
  TestMsg deserializeEventPayload(const payload_type &strbuf) const override {
    TestMsg message{};
    std::stringbuf bufferdummy(std::ios_base::in | std::ios_base::out | std::ios_base::binary);
    bufferdummy.str() = PayloadToString(strbuf);
    if (bufferdummy.str().size() > 0) {
      bufferdummy.sgetn(reinterpret_cast<char *>(&message.a), sizeof(message.a));
      bufferdummy.sgetn(reinterpret_cast<char *>(&message.b), sizeof(message.b));

      size_t str_size = 0;
      bufferdummy.sgetn(reinterpret_cast<char *>(&str_size), sizeof(str_size));
      char *tmp = new char[str_size];
      bufferdummy.sgetn(tmp, static_cast<long long>(str_size));
      message.test.append(tmp, str_size);
      delete[] tmp;
    }

    return message;
  }
  void publish(const TestMsg &message) {
    std::list<std::string> res;
    BaseEventBinary<TestMsg>::publish(message, res);
  }
  void send() { publish(message_); }
};

class MultipleSubscribersTestFixture : public testing::TestUsingEcal {
public:
  MultipleSubscribersTestFixture() {
    logger::register_to_init_chain();
    this->instance_name_ = "PlayerStatusEventTestFixture";
  }
};

//! flag for endless loop
std::atomic<bool> publishMessage(true);
std::atomic<bool> test_publisher_ready(false);
std::atomic<int> subscriber_hook_called(0);

void subscriber_hook() { subscriber_hook_called++; }

TEST_F(MultipleSubscribersTestFixture, multiple_subscribers) {
  // send one message
  TestBinary eventPublisher("");
  auto publisher_thread = std::thread([&] {
    eventPublisher.message_.test = "HelloWorld";
    eventPublisher.message_.a = 1234;
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
  TestBinary eventSubscriber1("");
  notify_hook hook = subscriber_hook;
  eventSubscriber1.addEventHook(hook);
  eventSubscriber1.subscribe();

  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  TestBinary eventSubscriber2("");
  notify_hook hook2 = subscriber_hook;
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
  TestMsg received_msg_1;
  received_msg_1 = eventSubscriber1.getEventData();

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  TestMsg received_msg_2;
  received_msg_2 = eventSubscriber2.getEventData();

  // check if header was also receivced
  EXPECT_TRUE(received_msg_1.a == received_msg_2.a);
}
} // namespace events
} // namespace sdk
} // namespace next

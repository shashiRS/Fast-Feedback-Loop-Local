/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     event_manager_test.cpp
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
#include <next/sdk/sdk.hpp>

#include "event_manager.h"
#include "event_publisher.h"
#include "event_subscriber.h"

constexpr const long long kWaitIncrementMs = 100;
constexpr const long long kMaxWaitTimeMs = 5000;

namespace next {
namespace sdk {
namespace events {

void sub_event_callback(std::string /* topic */) {}

void SetupEcalMonitor() {
  NEXT_DISABLE_DEPRECATED_WARNING
  // activate monitoring publisher
  std::string monitoring;
  eCAL::Monitoring::GetMonitoring(monitoring);
  // activate logging publisher
  std::string log;
  eCAL::Monitoring::GetLogging(log);
  NEXT_RESTORE_WARNINGS
}

struct DummyMsg {
  int a = 0;
};

class OverrideBinary : public BaseEventBinary<DummyMsg> {
public:
  OverrideBinary(std::string name) : BaseEventBinary<DummyMsg>(name){};

  ~OverrideBinary(){};
  payload_type serializeEventPayload(const DummyMsg &message) const override {
    return StringToPayload(std::to_string(message.a));
  }

  DummyMsg message_;
  //! calls BaseEventBinary::publish with _message
  void send() {
    std::list<std::string> res;
    publish(message_, res);
  }

  DummyMsg deserializeEventPayload(const payload_type &buf) const override {
    DummyMsg a = {std::stoi(PayloadToString(buf)) + 1};
    return a;
  }
};

template <typename EventClass>
bool ProcessInEcalMonitor(EventClass &event) {
  // check in ecal monitor if subscription was created
  NEXT_DISABLE_DEPRECATED_WARNING
  eCAL::pb::Monitoring ecal_monitor;
  std::string monitoring_s;
  bool found_process = false;

  for (const auto start = std::chrono::high_resolution_clock::now(),
                  end = start + std::chrono::milliseconds(kMaxWaitTimeMs);
       end > std::chrono::high_resolution_clock::now() && false == found_process;
       std::this_thread::sleep_for(std::chrono::milliseconds(kWaitIncrementMs))) {
    /*auto get_monitoring = */ eCAL::Monitoring::GetMonitoring(monitoring_s);
    ecal_monitor.ParseFromString(monitoring_s);
    if (ecal_monitor.topics().size() > 0) {
      for (auto topic : ecal_monitor.topics()) {
        if (topic.tname() ==
            event.getClassName() + "_" +
                event.getEventName()) { // the topic name should look like this "struct
                                        // next::control::events::NextMetaInformationMessage_EventManagerTestPub"
          std::string test = event.getClassName();
          std::string test2 = event.getEventName();
          found_process = true;
          break;
        }
      }
    }
  }
  return found_process;
  NEXT_RESTORE_WARNINGS
}
constexpr const int kPublisherCount = 10;

class EventManagerTest : public testing::TestUsingEcal {
public:
  EventManagerTest() {
    // enable for debugging testcase
    // initmap_[logger::getConsoleLoglevelCfgKey()] = logger::LOGLEVEL::DEBUG;
    logger::register_to_init_chain();

    this->instance_name_ = "EventManagerTest";
    send_ecal_messages = 0;
    publisher_ready = false;
    send_ecal_messages = true;
  }
  // sets the amount of publisher which sends messages with the same topic to one single subscriber
  std::mutex mutex_sub;
  std::array<bool, kPublisherCount> received_callbacks{false};

  void subscriber_callback(const payload_type &data) {
    std::lock_guard<std::mutex> lock(mutex_sub);
    if (!received_callbacks[std::stoul(PayloadToString(data))]) {
      ++hook_was_called;
      received_callbacks[std::stoul(PayloadToString(data))] = true;
    }
  }
  void test_hook() { hook_was_called++; }

  bool SubscribeAndCheck() {
    SetupEcalMonitor();

    hook_was_called = 0;

    eCAL::Util::EnableLoopback(true);
    OverrideBinary event("EventManagerSubscribeCleanup");
    event.subscribe();
    notify_hook hook = [this]() { test_hook(); };
    event.addEventHook(hook);

    uint32_t timeout_subscriber_created = 0;
    bool subscriber_found = ProcessInEcalMonitor(event);
    while (!subscriber_found && timeout_subscriber_created < 20) {
      std::this_thread::sleep_for(std::chrono::milliseconds(500));
      subscriber_found = ProcessInEcalMonitor(event);
      timeout_subscriber_created++;
    }

    if (!subscriber_found) {
      EXPECT_TRUE(subscriber_found) << "Subscriber not found";
      return false;
    }

    event.send();

    uint32_t timeout_data_received = 0;
    while (!hook_was_called && timeout_data_received < 200) {
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      timeout_data_received++;
    }

    if (hook_was_called.load() <= 0) {
      EXPECT_GT(hook_was_called.load(), 0) << "Subscriber hook not called";
      return false;
    }

    return true;
  }

  std::atomic<bool> send_ecal_messages{true};
  std::atomic<bool> publisher_ready{false};
  std::atomic<int> hook_was_called{0};

  virtual void TearDown() override {}
};

//! This tests create one single subscriber thread and AMOUNT_OF_PUBLISHER publisher threads. It expects that the
//! subscriber receives from each publisher a callback at least once
TEST_F(EventManagerTest, multiplePubsSingleSubTest) {
  // reset defaults
  send_ecal_messages = true;
  hook_was_called = 0;
  std::vector<std::thread> pubs(kPublisherCount);
  for (long unsigned int i = 0; i < kPublisherCount; ++i) {
    pubs[i] = std::thread([&] {
      EventPublisher *pub = new EventPublisher();
      payload_type buf;
      std::string s = std::to_string(i);
      buf = StringToPayload(s);
      std::list<std::string> res;
      pub->publish("multiplePubsSingleSubTestClass", "additional", buf, {}, res);

      while (send_ecal_messages) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        pub->publish("multiplePubsSingleSubTestClass", "additional", buf, {}, res);
      }
      delete pub;
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  // create subscriber
  eCAL::Util::EnableLoopback(true);
  auto sub_thread = std::thread([&] {
    EventSubscriber sub;
    RegisteredEvent event;
    auto binded_callback = [this](const payload_type &payload, long long, long long) { subscriber_callback(payload); };
    sub.subscribe("multiplePubsSingleSubTestClass", "additional", {1111, binded_callback, sub_event_callback, nullptr});

    while (eCAL::Ok() && send_ecal_messages) {
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
  });

  uint32_t timeout = 0;
  while (hook_was_called < kPublisherCount && timeout < 400) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    timeout++;
  }

  send_ecal_messages = false;

  for (auto &thread : pubs) {
    thread.join();
  }
  sub_thread.join();

  EXPECT_EQ(hook_was_called.load(), kPublisherCount);
  for (size_t i = 0; received_callbacks.size() > i; ++i) {
    EXPECT_TRUE(received_callbacks[i]) << "callback " << i << " was not called";
  }
}
TEST_F(EventManagerTest, createECALPublisher) {
  SetupEcalMonitor();

  OverrideBinary event("EventManagerTestPub");
  send_ecal_messages = true;
  auto pub_thread = std::thread([&] {
    event.message_.a = 1;
    while (send_ecal_messages) {
      event.send();
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
      publisher_ready = true;
    }
  });

  // wait for the thread above to be initilized
  while (!publisher_ready) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  EXPECT_TRUE(ProcessInEcalMonitor(event));
  send_ecal_messages = false;
  pub_thread.join();
}

TEST_F(EventManagerTest, subscribeAndReceiveMessage) {
  // reset defaults
  send_ecal_messages = true;
  publisher_ready = false;
  hook_was_called = 0;

  // create publisher
  OverrideBinary event("EventManagerTestSub");
  auto pub_thread = std::thread([&] {
    event.message_.a = 1;
    while (send_ecal_messages) {
      event.send();
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
      publisher_ready = true;
    }
  });

  // wait for the thread above to be initilized
  while (!publisher_ready) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  eCAL::Util::EnableLoopback(true);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));
  notify_hook hook = [this]() { test_hook(); };
  event.addEventHook(hook);

  event.subscribe();

  uint32_t timeout = 0;
  while (!hook_was_called && timeout < 200) {
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    timeout++;
  }

  // stop publisher thread
  send_ecal_messages = false;
  pub_thread.join();

  // check if header was also received
  EXPECT_TRUE(0 != event.getHeader().id);
  EXPECT_TRUE(0 != event.getHeader().timestamp);
  std::string topic_name{};
  topic_name.append(demangle_name(typeid(DummyMsg).name()).append("EventManagerTestSub"));
  std::string class_name = event.getClassName();
  std::string event_name = event.getEventName();
  EXPECT_TRUE(event.getClassName().append(event.getEventName()) == topic_name);

  EXPECT_GT(hook_was_called.load(), 0);
}

// subscribe to the same event with the same event name in the two tests
// subscribeCleanup1 and subscribeCleanup2 to check if cleanup of the events works properly
// so that the second time the subscription to the event works again
TEST_F(EventManagerTest, subscribeCleanup1) { EXPECT_TRUE(SubscribeAndCheck()); }

// subscribe to the same event with the same event name in the two tests
// subscribeCleanup1 and subscribeCleanup2 to check if cleanup of the events works properly
// so that the second time the subscription to the event works again
TEST_F(EventManagerTest, subscribeCleanup2) { EXPECT_TRUE(SubscribeAndCheck()); }

// subscribe to the same event with the same event name in the two tests
// subscribeCleanup1 and subscribeCleanup2 to check if cleanup of the events works properly
// so that the second time the subscription to the event works again
TEST_F(EventManagerTest, publishMultiThreadCleanupSafe) {
  OverrideBinary event1("");
  event1.initialize();
  event1.subscribe();
  event1.addEventHook([&event1]() {
    // add timeout to ensure a deadlock not happening
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    if (event1.getEventData().a == 2) {
      // try publishing a different number for no endless loop
      event1.publish({3});
    }
    event1.getNumberOfListeners();
    event1.subscribe();
    // add timeout to ensure a deadlock not happening
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  });

  auto thread = std::thread([]() {
    OverrideBinary event("");
    event.publish({1});
    if (event.getEventData().a == 2) {
      // try publishing a different number for no endless loop
      event.publish({3});
    }
    event.getNumberOfListeners();
    event.subscribe();
  });

  auto thread1 = std::thread([]() {
    OverrideBinary event("");
    event.publish({1});
    if (event.getEventData().a == 2) {
      // try publishing a different number for no endless loop
      event.publish({3});
    }
    event.getNumberOfListeners();
    event.subscribe();
  });

  auto thread2 = std::thread([]() {
    OverrideBinary event("");
    event.publish({1});
    if (event.getEventData().a == 2) {
      // try publishing a different number for no endless loop
      event.publish({3});
    }
    event.getNumberOfListeners();
    event.subscribe();
  });

  auto thread3 = std::thread([]() {
    OverrideBinary event("");
    event.publish({1});
    if (event.getEventData().a == 2) {
      // try publishing a different number for no endless loop
      event.publish({3});
    }
    event.getNumberOfListeners();
    event.subscribe();
  });

  auto thread4 = std::thread([]() {
    OverrideBinary event("4");
    event.publish({});
    if (event.getEventData().a == 2) {
      // try publishing a different number for no endless loop
      event.publish({3});
    }
    event.getNumberOfListeners();
    event.subscribe();
  });
  auto thread5 = std::thread([]() {
    OverrideBinary event("5");
    event.publish({});
    if (event.getEventData().a == 2) {
      // try publishing a different number for no endless loop
      event.publish({3});
    }
    event.getNumberOfListeners();
    event.subscribe();
  });
  auto thread6 = std::thread([]() {
    OverrideBinary event("6");
    event.publish({});
    if (event.getEventData().a == 2) {
      // try publishing a different number for no endless loop
      event.publish({3});
    }
    event.getNumberOfListeners();
    event.subscribe();
  });

  auto thread7 = std::thread([]() {
    OverrideBinary event("7");
    event.publish({});
    if (event.getEventData().a == 2) {
      // try publishing a different number for no endless loop
      event.publish({3});
    }
    event.getNumberOfListeners();
    event.subscribe();
  });

  thread.join();
  thread1.join();
  thread2.join();
  thread3.join();
  thread4.join();
  thread5.join();
  thread6.join();
  thread7.join();
}
} // namespace events
} // namespace sdk
} // namespace next

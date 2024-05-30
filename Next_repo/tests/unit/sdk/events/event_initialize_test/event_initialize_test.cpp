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
#include <chrono>
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
#include "event_publisher.h"
#include "event_subscriber.h"

constexpr const long long kWaitIncrementMs = 100;
constexpr const long long kMaxWaitTimeMs = 5000;

namespace next {
namespace sdk {
namespace events {

NEXT_DISABLE_DEPRECATED_WARNING
void SetupEcalMonitor() {
  // activate monitoring publisher
  std::string monitoring;
  eCAL::Monitoring::GetMonitoring(monitoring);
  // activate logging publisher
  std::string log;
  eCAL::Monitoring::GetLogging(log);
}

template <typename EventClass>
bool ProcessInEcalMonitor(EventClass &event) {
  // check in ecal monitor if subscription was created
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
    if (ecal_monitor.clients().size() > 0) {
      for (auto client : ecal_monitor.clients()) {
        if (client.sname() ==
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
}
NEXT_RESTORE_WARNINGS

class EventInitializeTest : public testing::TestUsingEcal {
public:
  EventInitializeTest() {
    // enable for debugging testcase
    // initmap_[logger::getConsoleLoglevelCfgKey()] = logger::LOGLEVEL::DEBUG;
    logger::register_to_init_chain();

    this->instance_name_ = "EventManagerTest";
  }

  virtual void TearDown() override {}
};

struct DummyMsg {
  int a = 0;
};

class OverrideBinary : public BaseEventBinary<DummyMsg> {
public:
  OverrideBinary(std::string name) : BaseEventBinary<DummyMsg>(name){};
  ~OverrideBinary(){};
  payload_type serializeEventPayload(const DummyMsg & /*message*/) const override { return StringToPayload("1"); }
  DummyMsg deserializeEventPayload(const payload_type & /*buf*/) const override {
    DummyMsg a = {2};
    return a;
  }
  DummyMsg message_;
  //! calls BaseEventBinary::publish with message_
  void send() {
    std::list<std::string> res;
    publish(message_, res);
  }
};

TEST_F(EventInitializeTest, testInitEvent_success) {
  SetupEcalMonitor();

  eCAL::Util::EnableLoopback(true);
  OverrideBinary event("testInitEvent");
  event.initialize();

  const auto end = std::chrono::steady_clock::now() + std::chrono::seconds(5);
  bool subscriber_found = ProcessInEcalMonitor(event);
  while (!subscriber_found && std::chrono::steady_clock::now() < end) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    subscriber_found = ProcessInEcalMonitor(event);
  }

  EXPECT_TRUE(subscriber_found) << "Subscriber not found";
}

TEST_F(EventInitializeTest, testInitEvent_fail) {
  SetupEcalMonitor();

  eCAL::Util::EnableLoopback(true);
  OverrideBinary event("testInitEvent_fail");
  // do not initialize the event here
  // it should not be possbile to subscribe to a not initialized event
  // event.initialize();

  const auto end = std::chrono::steady_clock::now() + std::chrono::seconds(5);
  bool subscriber_found = ProcessInEcalMonitor(event);
  while (!subscriber_found && std::chrono::steady_clock::now() < end) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    subscriber_found = ProcessInEcalMonitor(event);
  }

  EXPECT_FALSE(subscriber_found) << "Subscriber found but should not exist";
}

TEST_F(EventInitializeTest, TestPayloadToSTring) {
  std::string mymessage = "asdf";
  payload_type payload = StringToPayload(mymessage);
  EXPECT_EQ(payload.size(), 4);
  EXPECT_EQ(mymessage, PayloadToString(payload));
}

TEST_F(EventInitializeTest, TestPayloadToString_invalidStringNoCrash) {
  std::string mymessage = "asdf\n19asldkfjasd}\0asdföalskoie\n";
  payload_type payload = StringToPayload(mymessage);
  EXPECT_EQ(payload.size(), mymessage.size());
  EXPECT_EQ(mymessage, PayloadToString(payload));
}

class OverrideBinaryBlocking : public BaseEventBinaryBlocking<DummyMsg> {
public:
  OverrideBinaryBlocking(std::string name) : BaseEventBinaryBlocking<DummyMsg>(name){};
  ~OverrideBinaryBlocking(){};
  payload_type serializeEventPayload(const DummyMsg & /*message*/) const override { return StringToPayload("1"); }
  DummyMsg deserializeEventPayload(const payload_type & /*buf*/) const override {
    DummyMsg a = {2};
    return a;
  }

  DummyMsg message_;
  //! calls BaseEventBinary::publish with _message
  void send() {
    std::list<std::string> res;
    publish(message_, res);
  }
};

TEST_F(EventInitializeTest, testInitEventBlocking_success) {
  SetupEcalMonitor();

  eCAL::Util::EnableLoopback(true);
  OverrideBinaryBlocking event("testInitEventBlocking");
  event.initialize();

  const auto end = std::chrono::steady_clock::now() + std::chrono::seconds(5);
  bool subscriber_found = ProcessInEcalMonitor(event);
  while (!subscriber_found && std::chrono::steady_clock::now() < end) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    subscriber_found = ProcessInEcalMonitor(event);
  }

  EXPECT_TRUE(subscriber_found) << "Subscriber not found";
}

TEST_F(EventInitializeTest, testInitEventBlocking_fail) {
  SetupEcalMonitor();

  eCAL::Util::EnableLoopback(true);
  OverrideBinaryBlocking event("testInitEventBlocking_fail");
  // do not initialize the event here
  // it should not be possbile to subscribe to a not initialized event
  // event.initialize();

  const auto end = std::chrono::steady_clock::now() + std::chrono::seconds(5);
  bool subscriber_found = ProcessInEcalMonitor(event);
  while (!subscriber_found && std::chrono::steady_clock::now() < end) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    subscriber_found = ProcessInEcalMonitor(event);
  }

  EXPECT_FALSE(subscriber_found) << "Subscriber found but should not exist";
}
} // namespace events
} // namespace sdk
} // namespace next

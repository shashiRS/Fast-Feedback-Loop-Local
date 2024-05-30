/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     request_eventupdate_test.cpp
 * @brief
 *
 **/

#include <condition_variable>
#include <thread>

#include <ecal/ecal.h>

#include <next/sdk/events/version_info.h>
#include <next/sdk/sdk.hpp>

namespace next {
namespace sdk {
namespace events {

constexpr int k_wait_for_ecal_register = 1000;
static constexpr int ECAL_WAIT_TIME = 1000; // in miliseconds

class PullEventFeatureTest : public testing::TestUsingEcal {
public:
  PullEventFeatureTest() : TestUsingEcal() {
    this->instance_name_ = "PullEventFeatureTest";
    logger::register_to_init_chain();
  }

  ~PullEventFeatureTest() {}
};

void checkForCallback(std::atomic<bool> &check_flag) {
  std::mutex synchronization_mutex_;
  std::unique_lock<std::mutex> lock(synchronization_mutex_);
  std::condition_variable cv_check_hook;
  cv_check_hook.wait_for(lock, std::chrono::milliseconds(k_wait_for_ecal_register),
                         [&check_flag]() { return check_flag == true; });
}
std::string generateUniqueName(const std::string &in) {
  return in + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                                 std::chrono::steady_clock::now().time_since_epoch())
                                 .count());
}

/*!
 * @startuml
 * alt start up ev and ev1
 * main -> "Event ev.publish" : create objects
 * main -> "Event ev1.subscribe" : create objects
 * "Event ev.publish" -> "Event ev1.subscribe" : publish data Package_1
 *
 * else start up second event ev2
 * main -> "Event ev2.subscribe" : create objects
 * "Event ev2.subscribe" -> "Event ev.publish" : request update (requestEventUpdate)
 * "Event ev.publish" -> "Event ev1.subscribe" : publish buffered old event (Package_1)
 * "Event ev.publish" -> "Event ev2.subscribe" : publish buffered old event (Package_1)
 * end
 * @enduml
 * @startuml
 * alt start up ev and ev1
 * main -> "Event ev.publish" : create objects
 *
 * else start up second event ev2
 * main -> "Event ev2.subscribe" : create objects
 * "Event ev2.subscribe" -> "Event ev.publish" : request update (requestEventUpdate)
 * "Event ev.publish" -> "Event ev2.subscribe" : publish buffered old event (Package_1)
 * end
 * @enduml
 */
TEST_F(PullEventFeatureTest, check_pull_request_single_event) {

  std::string event_name = generateUniqueName("check_pull_request_single_event");
  std::shared_ptr<next::sdk::events::VersionInfo> _version_event =
      std::make_shared<next::sdk::events::VersionInfo>(event_name);

  std::atomic<bool> callback_hit = false;
  _version_event->addOnEventRequestedHook([&callback_hit](const payload_type &) { callback_hit = true; });

  eCAL::Util::EnableLoopback(true);
  _version_event->requestEventUpdate();

  checkForCallback(callback_hit);
  EXPECT_TRUE(callback_hit);
}

TEST_F(PullEventFeatureTest, requestEventUpdate_test_with_hook_at_subscriber) {

  std::string event_name = generateUniqueName("requestEventUpdate_test_with_hook_at_subscriber");

  eCAL::Util::EnableLoopback(true);
  auto _version_event_1 = std::make_shared<next::sdk::events::VersionInfo>(event_name);
  auto _version_event_2 = std::make_shared<next::sdk::events::VersionInfo>(event_name);

  VersionInfoPackage _version_event_1_send_data;
  _version_event_1_send_data.component_name = "Test_requestEventUpdate_test";
  _version_event_1_send_data.documentation_link = "docu_link";
  _version_event_1_send_data.component_version = "0.1.100";

  _version_event_1->publish(_version_event_1_send_data);

  std::atomic<bool> callback_hit = false;
  _version_event_2->addEventHook([&callback_hit]() { callback_hit = true; });
  _version_event_2->subscribe();

  _version_event_2->requestEventUpdate();

  checkForCallback(callback_hit);
  ASSERT_TRUE(callback_hit);
}
/*
 * @startuml
 * alt start up ev and ev1
 * main -> "Event ev.publish" : create objects
 *
 * else start up second event ev2
 * main -> "Event ev2.subscribe" : create objects
 * "Event ev2.subscribe" -> "Event ev.publish" : request update (requestEventUpdate)
 * "Event ev.publish" -> "Event ev2.subscribe" : publish buffered old event (Package_1)
 * end
 * @enduml
 */

TEST_F(PullEventFeatureTest, requestEventUpdate_expect_restult_after_time) {
  std::string event_name = generateUniqueName("requestEventUpdate_expect_restult_after_time");
  eCAL::Util::EnableLoopback(true);
  auto _version_event_1 = std::make_shared<next::sdk::events::VersionInfo>(event_name);
  auto _version_event_2 = std::make_shared<next::sdk::events::VersionInfo>(event_name);
  VersionInfoPackage _version_event_1_send_data;
  _version_event_1_send_data.component_name = "Test_requestEventUpdate_test";
  _version_event_1_send_data.documentation_link = "docu_link";
  _version_event_1_send_data.component_version = "0.1.100";

  _version_event_1->publish(_version_event_1_send_data);

  _version_event_2->subscribe();

  auto data = _version_event_2->getEventData();
  EXPECT_EQ(data.component_name, "");
  EXPECT_EQ(data.component_version, "");
  EXPECT_EQ(data.documentation_link, "");

  _version_event_2->requestEventUpdate();
  std::this_thread::sleep_for(std::chrono::milliseconds(2 * k_wait_for_ecal_register));
  auto data1 = _version_event_2->getEventData();
  EXPECT_EQ(data1.component_name, "Test_requestEventUpdate_test");
  EXPECT_EQ(data1.component_version, "0.1.100");
  EXPECT_EQ(data1.documentation_link, "docu_link");
}

TEST_F(PullEventFeatureTest, requestEventUpdate_expect_restult_after_time_multiple_events) {
  /*
  _version_event_1 - publishing
  _version_event_2 - subscribe and request for _version_event_1
  _version_event_3 - publishing
  _version_event_2 - subscribe and request for _version_event_3
  */
  std::string event_name = generateUniqueName("requestEventUpdate_expect_restult_after_time_multiple_events");
  eCAL::Util::EnableLoopback(true);
  auto _version_event_1 = std::make_shared<next::sdk::events::VersionInfo>(event_name);
  auto _version_event_2 = std::make_shared<next::sdk::events::VersionInfo>(event_name);
  auto _version_event_3 = std::make_shared<next::sdk::events::VersionInfo>(event_name);

  VersionInfoPackage _version_event_1_send_data;
  _version_event_1_send_data.component_name = "Next-Player_Test";
  _version_event_1_send_data.documentation_link = "docu_link";
  _version_event_1_send_data.component_version = "0.1.100";

  _version_event_1->publish(_version_event_1_send_data);

  _version_event_2->subscribe();

  _version_event_2->requestEventUpdate();
  std::this_thread::sleep_for(std::chrono::milliseconds(k_wait_for_ecal_register));
  auto data = _version_event_2->getEventData();

  EXPECT_EQ(data.component_name, "Next-Player_Test");
  EXPECT_EQ(data.documentation_link, "docu_link");
  EXPECT_EQ(data.component_version, "0.1.100");

  VersionInfoPackage _version_event_3_send_data;
  _version_event_3_send_data.component_name = "Next-DataBridge_Test";
  _version_event_3_send_data.documentation_link = "docu_link";
  _version_event_3_send_data.component_version = "1.2.200";

  _version_event_3->publish(_version_event_3_send_data);

  _version_event_2->subscribe();

  _version_event_2->requestEventUpdate();
  std::this_thread::sleep_for(std::chrono::milliseconds(k_wait_for_ecal_register));
  auto data1 = _version_event_2->getEventData();

  EXPECT_EQ(data1.component_name, "Next-DataBridge_Test");
  EXPECT_EQ(data1.documentation_link, "docu_link");
  EXPECT_EQ(data1.component_version, "1.2.200");
}

TEST_F(PullEventFeatureTest, requestEventTestMultipleEvents_shuffle_test) {
  /*
  _version_event_1 - publishing
  _version_event_2 - subscribe and request for _version_event_1
  _version_event_3 - publishing
  _version_event_2 - subscribe and request for _version_event_3
  */
  std::string event_name = generateUniqueName("requestEventTestMultipleEvents_shuffle_test");
  eCAL::Util::EnableLoopback(true);
  auto _version_event_1 = std::make_shared<next::sdk::events::VersionInfo>(event_name);
  auto _version_event_2 = std::make_shared<next::sdk::events::VersionInfo>(event_name);
  auto _version_event_3 = std::make_shared<next::sdk::events::VersionInfo>(event_name);
  auto _version_event_4 = std::make_shared<next::sdk::events::VersionInfo>(event_name);

  VersionInfoPackage _version_event_1_send_data;
  _version_event_1_send_data.component_name = "Next-Player_Test";
  _version_event_1_send_data.documentation_link = "docu_link";
  _version_event_1_send_data.component_version = "0.1.100";

  _version_event_1->publish(_version_event_1_send_data);

  _version_event_2->subscribe();

  _version_event_2->requestEventUpdate();
  std::this_thread::sleep_for(std::chrono::milliseconds(k_wait_for_ecal_register));
  auto data = _version_event_2->getEventData();

  EXPECT_EQ(data.component_name, "Next-Player_Test");
  EXPECT_EQ(data.documentation_link, "docu_link");
  EXPECT_EQ(data.component_version, "0.1.100");

  VersionInfoPackage _version_event_3_send_data;
  _version_event_3_send_data.component_name = "Next-DataBridge_Test";
  _version_event_3_send_data.documentation_link = "docu_link";
  _version_event_3_send_data.component_version = "1.2.200";

  _version_event_3->publish(_version_event_3_send_data);

  _version_event_2->subscribe();

  _version_event_2->requestEventUpdate();
  std::this_thread::sleep_for(std::chrono::milliseconds(k_wait_for_ecal_register));
  auto data1 = _version_event_2->getEventData();

  EXPECT_EQ(data1.component_name, "Next-DataBridge_Test");
  EXPECT_EQ(data1.documentation_link, "docu_link");
  EXPECT_EQ(data1.component_version, "1.2.200");
}

} // namespace events
} // namespace sdk
} // namespace next

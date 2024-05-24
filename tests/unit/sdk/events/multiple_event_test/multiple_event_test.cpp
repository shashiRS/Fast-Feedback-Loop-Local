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
#include <next/sdk/events/request_config_from_server.h>
#include <next/sdk/events/version_info.h>
#include <next/sdk/sdk.hpp>

namespace next {
namespace sdk {
namespace events {

std::atomic<int> hook_listener_added_vers_info_was_called(0);
std::atomic<int> hook_listener_added_req_config_called(0);

static constexpr int ECAL_WAIT_TIME = 1500; // in miliseconds

void event_hook_versioninfo() {
  std::cout << "VersionInfo Hook funciton called " << std::endl;
  hook_listener_added_vers_info_was_called++;
}

void event_hook_reqconfig() {
  hook_listener_added_req_config_called++;
  std::cout << "ReqConfig Hook funciton called " << std::endl;
}

class MultipleEventTest : public testing::TestUsingEcal {
public:
  MultipleEventTest() {
    logger::register_to_init_chain();
    this->instance_name_ = "MultipleEventTest";
  }
};

std::string generateUniqueName(const std::string &in) {
  return in + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                                 std::chrono::steady_clock::now().time_since_epoch())
                                 .count());
}

// Publishing only VersionInfo and not ReRequestConfig , verify only VersioInfo hook called not the RequestConfig hook
TEST_F(MultipleEventTest, publishVersionInfo) {
  // wait to cleanup previous test
  std::this_thread::sleep_for(std::chrono::milliseconds(ECAL_WAIT_TIME));

  hook_listener_added_vers_info_was_called = 0;
  hook_listener_added_req_config_called = 0;
  // give the publisher unique name so that interference from other tests in jenkins can be avoided
  std::string pub_name = generateUniqueName("publishVersionInfo");

  eCAL::Util::EnableLoopback(true);
  VersionInfoPackage send_data;
  VersionInfo version_event_pub(pub_name);
  events::RequestConfigFromServer req_config_event_pub(pub_name);

  auto hook_version_info = std::bind(&event_hook_versioninfo);
  auto hook_req_config = std::bind(&event_hook_reqconfig);

  version_event_pub.addEventHook(hook_version_info);
  req_config_event_pub.addEventHook(hook_req_config);

  version_event_pub.subscribe();
  req_config_event_pub.subscribe();

  std::this_thread::sleep_for(std::chrono::milliseconds(ECAL_WAIT_TIME));

  send_data.component_name = "TestMultipleSubs";
  send_data.component_version = "0.1.100";
  send_data.documentation_link = "docu_link";

  version_event_pub.publish(send_data);
  std::this_thread::sleep_for(std::chrono::milliseconds(ECAL_WAIT_TIME));
  EXPECT_EQ(hook_listener_added_vers_info_was_called.load(), 1);
  EXPECT_EQ(hook_listener_added_req_config_called, 0);
}

TEST_F(MultipleEventTest, publishVersionInfoAndReqConfig) {
  // wait to cleanup previous test
  std::this_thread::sleep_for(std::chrono::milliseconds(ECAL_WAIT_TIME));

  hook_listener_added_vers_info_was_called = 0;
  hook_listener_added_req_config_called = 0;
  std::string pub_name = generateUniqueName("publishVersionInfoAndReqConfig");
  eCAL::Util::EnableLoopback(true);
  VersionInfoPackage send_data;
  RequestConfigFromServerInformationMessage send_data_req;
  VersionInfo version_event_pub(pub_name);
  std::this_thread::sleep_for(std::chrono::milliseconds(ECAL_WAIT_TIME));
  events::RequestConfigFromServer req_config_event_pub(pub_name);
  std::this_thread::sleep_for(std::chrono::milliseconds(ECAL_WAIT_TIME));

  auto hook_version_info = std::bind(&event_hook_versioninfo);
  auto hook_req_config = std::bind(&event_hook_reqconfig);

  version_event_pub.addEventHook(hook_version_info);
  req_config_event_pub.addEventHook(hook_req_config);

  version_event_pub.subscribe();
  req_config_event_pub.subscribe();

  std::this_thread::sleep_for(std::chrono::milliseconds(ECAL_WAIT_TIME));

  send_data.component_name = "TestMultipleSubs";
  send_data.component_version = "0.1.100";
  send_data.documentation_link = "docu_link";

  send_data_req.request_key = "Test_Receiver";
  send_data_req.requester_name = "Test_Receiver";

  version_event_pub.publish(send_data);
  req_config_event_pub.publish(send_data_req);

  std::this_thread::sleep_for(std::chrono::milliseconds(ECAL_WAIT_TIME));
  EXPECT_EQ(hook_listener_added_vers_info_was_called, 1);
  EXPECT_EQ(hook_listener_added_req_config_called, 1);
}

TEST_F(MultipleEventTest, publishVersionInfoAndReqConfig_Multiplehooks) {
  // wait to cleanup previous test
  std::this_thread::sleep_for(std::chrono::milliseconds(ECAL_WAIT_TIME));

  hook_listener_added_vers_info_was_called = 0;
  hook_listener_added_req_config_called = 0;
  std::string pub_name = generateUniqueName("publishVersionInfoAndReqConfig_Multiplehooks");
  eCAL::Util::EnableLoopback(true);
  VersionInfoPackage send_data;
  RequestConfigFromServerInformationMessage send_data_req;
  VersionInfo version_event_pub(pub_name);
  events::RequestConfigFromServer req_config_event_pub(pub_name);

  auto hook_version_info = std::bind(&event_hook_versioninfo);
  auto hook_req_config = std::bind(&event_hook_reqconfig);

  version_event_pub.addEventHook(hook_version_info);
  req_config_event_pub.addEventHook(hook_req_config);
  // add same hook functions once again to get called two times
  version_event_pub.addEventHook(hook_version_info);
  req_config_event_pub.addEventHook(hook_req_config);

  version_event_pub.subscribe();
  req_config_event_pub.subscribe();

  std::this_thread::sleep_for(std::chrono::milliseconds(ECAL_WAIT_TIME));

  send_data.component_name = "TestMultipleSubs";
  send_data.component_version = "0.1.100";
  send_data.documentation_link = "docu_link";
  send_data_req.request_key = "Player.port";
  send_data_req.requester_name = "next_bridges";

  version_event_pub.publish(send_data);
  req_config_event_pub.publish(send_data_req);
  std::this_thread::sleep_for(std::chrono::milliseconds(ECAL_WAIT_TIME));
  EXPECT_EQ(hook_listener_added_vers_info_was_called, 2);
  EXPECT_EQ(hook_listener_added_req_config_called, 2);
}

TEST_F(MultipleEventTest, publishVersionInfo_DynamicObject) {
  // wait to cleanup previous test
  std::this_thread::sleep_for(std::chrono::milliseconds(ECAL_WAIT_TIME));
  hook_listener_added_vers_info_was_called = 0;
  hook_listener_added_req_config_called = 0;
  std::string pub_name = generateUniqueName("publishVersionInfo_DynamicObject");
  eCAL::Util::EnableLoopback(true);
  VersionInfoPackage send_data;
  VersionInfo *version_event_pub = new VersionInfo(pub_name);

  auto hook_version_info = std::bind(&event_hook_versioninfo);

  version_event_pub->addEventHook(hook_version_info);
  version_event_pub->subscribe();
  std::this_thread::sleep_for(std::chrono::milliseconds(ECAL_WAIT_TIME));

  send_data.component_name = "TestMultipleSubs";
  send_data.component_version = "0.1.100";
  send_data.documentation_link = "docu_link";

  version_event_pub->publish(send_data);

  std::this_thread::sleep_for(std::chrono::milliseconds(ECAL_WAIT_TIME));
  EXPECT_EQ(hook_listener_added_vers_info_was_called, 1);
  EXPECT_EQ(hook_listener_added_req_config_called, 0);
  delete version_event_pub; // should not crash
}

} // namespace events
} // namespace sdk
} // namespace next

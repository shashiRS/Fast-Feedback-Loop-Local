/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     test_using_ecal.hpp
 * @brief    Test fixture for providing a eCAL initalization in unit tests
 *
 **/

#ifndef NEXTSDK_TEST_USING_ECAL_H_
#define NEXTSDK_TEST_USING_ECAL_H_

#include <mutex>

#include <cip_test_support/gtest_reporting.h>
#include <ecal/ecal_util.h>
#ifndef NEXTSDK_SDK_H_
#error "Please include sdk.hpp, not this file directly"
#endif

namespace next {
namespace sdk {
namespace testing {

class TestUsingEcal : public ::testing::Test {
public:
  TestUsingEcal() {
    std::cout << "TestUsingEcal started" << std::endl;
    next::sdk::EcalSingleton::register_to_init_chain();
    next::sdk::logger::register_to_init_chain();
    initmap_.emplace(logger::getLogRouterDisableCfgKey(), true);
  }

  virtual ~TestUsingEcal() {
    std::cout << "~TestUsingEcal started" << std::endl;
    next::sdk::InitChain::Reset();
    std::cout << "~TestUsingEcal ended" << std::endl;
  }

  virtual void SetUp() override {
    ASSERT_FALSE(instance_name_.empty()) << "the eCAL instance name must be set";
    initmap_.emplace(next::sdk::EcalSingleton::getInstancenameCfgKey(), instance_name_);
    ASSERT_TRUE(next::sdk::InitChain::Run(initmap_)) << "initialization of eCAL failed";
    ASSERT_TRUE(next::sdk::logger::isInitialized())
        << "eCAL singleton is using the logger, the logger must be initialized";
    next::sdk::EcalSingleton::get_instance();
    eCAL::Util::EnableLoopback(true);
  }

  template <class publisherTemplate>
  bool CheckIsSubscribed(publisherTemplate &pub, size_t wait_ms = 5000) {
    // take this time so the parts are connected. We miss an API here!
    constexpr int loops = 10;
    bool success = true;

    size_t wait_ms_per_loop = wait_ms / loops;
    int loop_count = 0;
    for (loop_count = 0; loop_count < loops; loop_count++) {
      success = true;

      success = pub.IsSubscribed();
      if (success) {
        std::this_thread::sleep_for(std::chrono::milliseconds(wait_ms_per_loop));
        return true;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(wait_ms_per_loop));
    }
    EXPECT_TRUE(false) << "publisher is not connected " << pub.GetTopicName();
    return false;
  }

  template <class subscriberTemplate>
  bool CheckConnection(subscriberTemplate &sub, std::vector<std::string> urls_to_check, size_t wait_ms = 5000) {
    // take this time so the parts are connected. We miss an API here!
    std::map<std::string, bool> connection_status_map;
    constexpr int loops = 10;
    bool success = true;

    for (auto url : urls_to_check) {
      connection_status_map[url] = false;
    }

    size_t wait_ms_per_loop = wait_ms / loops;

    int loop_count = 0;
    for (loop_count = 0; loop_count < loops; loop_count++) {
      success = true;
      for (auto url : urls_to_check) {
        connection_status_map[url] = sub.IsConnected(url);
      }

      for (auto url_status : connection_status_map) {
        if (!url_status.second) {
          success = false;
          break;
        }
      }

      if (success) {
        std::this_thread::sleep_for(std::chrono::milliseconds(wait_ms_per_loop));
        return true;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(wait_ms_per_loop));
    }

    for (auto url : urls_to_check) {
      EXPECT_TRUE(connection_status_map[url]) << "Subscriber not connected: " << url;
    }

    return false;
  }

  template <class subscriberTemplate>
  bool CheckConnectionShared(std::shared_ptr<subscriberTemplate> sub, std::vector<std::string> urls_to_check,
                             size_t wait_ms) {
    // take this time so the parts are connected. We miss an API here!
    std::map<std::string, bool> connection_status_map;
    constexpr int loops = 10;
    bool success = true;

    size_t wait_ms_per_loop = wait_ms / loops;

    int loop_count = 0;
    for (loop_count = 0; loop_count < loops; loop_count++) {
      for (auto url : urls_to_check) {
        connection_status_map[url] = false;
      }
      success = true;
      for (auto url : urls_to_check) {
        connection_status_map[url] = sub->IsConnected(url);
      }

      for (auto url_status : connection_status_map) {
        if (!url_status.second) {
          success = false;
          break;
        }
      }

      if (success) {
        std::this_thread::sleep_for(std::chrono::milliseconds(wait_ms_per_loop));
        return true;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(wait_ms_per_loop));
    }

    for (auto url : urls_to_check) {
      EXPECT_TRUE(connection_status_map[url]) << "Subscriber not connected: " << url;
    }

    return false;
  }

  std::string generateUniqueName(const std::string &in) {
    return in + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                                   std::chrono::steady_clock::now().time_since_epoch())
                                   .count());
  }

protected:
  InitChain::ConfigMap initmap_;
  std::string instance_name_{""};
};

} // namespace testing
} // namespace sdk
} // namespace next

#endif // NEXTSDK_TEST_USING_ECAL_H_

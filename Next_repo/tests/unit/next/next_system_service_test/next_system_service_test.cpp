#include <cip_test_support/gtest_reporting.h>

#include <iostream>

#include <ecal/ecal.h>

#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>

#include "next_system_service.hpp"

#include "next/sdk/sdk.hpp"
#include "next/udex/publisher/data_publisher.hpp"

namespace next {
namespace system_service {

inline std::string GetSyncPortName() { return "array_root.simple_float_arrays"; }

class TimeServiceTester : public TimeService {
public:
  TimeServiceTester() : TimeService("test_component"){};
  std::shared_ptr<next::udex::subscriber::DataSubscriber> GetSubscriber() { return data_subscriber_; };
};

class NextSystemServiceTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  NextSystemServiceTestFixture() { this->instance_name_ = "NextOutportPortFixture"; }
  void SetUp() override {
    TestUsingEcal::SetUp();
    next::appsupport::ConfigClient::do_init("test_component");
    next::appsupport::ConfigClient::finish_init();
    test_array = (uint32_t *)malloc(array_size);
  };
  uint32_t *test_array = nullptr;
  size_t array_size = 38;

  void TearDown() override {
    next::appsupport::ConfigClient::getConfig()->do_reset();
    TestUsingEcal::TearDown();
    free(test_array);
  };
};

TEST_F(NextSystemServiceTestFixture, CheckConfigUrl) {

  auto tester = TimeServiceTester();
  next::appsupport::ConfigClient::getConfig()->put(appsupport::configkey::getTimestampUrlKey("test_component") + "[0]",
                                                   "asdf.asdfasdf.asdfasdfasdf");
  next::appsupport::ConfigClient::getConfig()->put(appsupport::configkey::getTimestampUrlKey("test_component") + "[1]",
                                                   "asdf.asdfasdf.asdfasdfasdf1");
  tester.SetupSystemService();
  auto info = tester.GetSubscriber()->getSubscriptionsInfo();
  ASSERT_EQ(info.size(), 1);

  bool found = false;
  for (const auto &sub : info) {
    if (sub.second.url == "asdf.asdfasdf.asdfasdfasdf") {
      found = true;
    }
  }
  EXPECT_TRUE(found);
  tester.FillCurrentTimestampMicroseconds();
  auto value = tester.getSystemTimeMicroseconds();
  EXPECT_EQ(value, 0u);
}

TEST_F(NextSystemServiceTestFixture, CheckTimeStampManinpulationWithUrlList) {

  next::appsupport::ConfigClient::getConfig()->put(appsupport::configkey::getTimestampUrlKey("test_component") + "[0]",
                                                   "asdf.asdfasdf.asdfasdfasdf");
  next::appsupport::ConfigClient::getConfig()->put(appsupport::configkey::getTimestampUrlKey("test_component") + "[1]",
                                                   "SIM VFB.AlgoVehCycle.VehDyn.sSigHeader.uiTimeStamp");

  auto pub = next::udex::publisher::DataPublisher("tester");
  ASSERT_TRUE(pub.RegisterDataDescription("ARS5xx.sdl", true));
  eCAL::Util::EnableLoopback(true);

  auto tester = TimeServiceTester();
  tester.SetupSystemService();

  next::appsupport::ConfigClient::getConfig()->put(appsupport::configkey::getTimestampUrlKey("test_component"),
                                                   "SIM VFB.AlgoVehCycle.VehDyn.sSigHeader.uiTimeStamp");
  auto check = CheckConnectionShared(tester.GetSubscriber(), {"SIM VFB.AlgoVehCycle.VehDyn"}, 5000);
  EXPECT_EQ(check, true);

  // this is an 8 byte offset as described in the sdl
  test_array[1] = 1111;
  pub.publishData("SIM VFB.AlgoVehCycle.VehDyn", test_array, array_size, false, 1);

  tester.FillCurrentTimestampMicroseconds();
  auto value = tester.getSystemTimeMicroseconds();
  EXPECT_EQ(value, 1111u);

  std::cout << "finish UrlList" << std::endl;
}

TEST_F(NextSystemServiceTestFixture, CheckTimeStampManinpulationChangingUnitsToSeconds) {

  next::appsupport::ConfigClient::getConfig()->put(appsupport::configkey::getTimestampUrlKey("test_component"),
                                                   "SIM VFB.AlgoVehCycle.VehDyn.sSigHeader.uiTimeStamp");
  next::appsupport::ConfigClient::getConfig()->put(appsupport::configkey::getTimestampUnitKey("test_component"), "s");

  auto pub = next::udex::publisher::DataPublisher("tester");
  ASSERT_TRUE(pub.RegisterDataDescription("ARS5xx.sdl", true));
  eCAL::Util::EnableLoopback(true);

  auto tester = TimeServiceTester();
  tester.SetupSystemService();
  next::appsupport::ConfigClient::getConfig()->put(appsupport::configkey::getTimestampUrlKey("test_component"),
                                                   "SIM VFB.AlgoVehCycle.VehDyn.sSigHeader.uiTimeStamp");
  CheckConnectionShared(tester.GetSubscriber(), {"SIM VFB.AlgoVehCycle.VehDyn"}, 5000);

  // this is an 8 byte offset as described in the sdl
  test_array[1] = 1111;
  pub.publishData("SIM VFB.AlgoVehCycle.VehDyn", test_array, array_size, false, 1);

  tester.FillCurrentTimestampMicroseconds();
  auto value = tester.getSystemTimeMicroseconds();
  EXPECT_EQ(value, 1111000000u);
}

} // namespace system_service
} // namespace next

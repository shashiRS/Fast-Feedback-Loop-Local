
#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <next/udex/data_types.hpp>
#include <next/udex/publisher/data_publisher.hpp>

TEST(register_data_description_test, reset_db) {
  // TODO: check the actual database content after each reset function

  auto publisher_test1 = next::udex::publisher::DataPublisher("test1");
  EXPECT_TRUE(publisher_test1.RegisterDataDescription((std::string) "test1.sdl", false, DESCRIPTION_TYPE_SDL))
      << "RegisterDataDescription failed";
  //  publisher_test1.Reset();

  auto publisher_test2 = next::udex::publisher::DataPublisher("test2");
  EXPECT_TRUE(publisher_test2.SetDataSourceInfo("ARS5xx", 6403, 0)) << "SetDataSourceInfo failed";

  EXPECT_TRUE(publisher_test2.RegisterDataDescription((std::string) "test2.sdl", false, DESCRIPTION_TYPE_SDL))
      << "RegisterDataDescription failed";
  //  publisher_test2.Reset();
}

TEST(register_data_description_test, register_sdl) {
  auto publisher_test = next::udex::publisher::DataPublisher("test");
  EXPECT_TRUE(publisher_test.RegisterDataDescription((std::string) "test1.sdl", false, DESCRIPTION_TYPE_SDL))
      << "RegisterDataDescription failed";
}

TEST(register_data_description_test, register_cdl) {
  auto publisher_test = next::udex::publisher::DataPublisher("test");
  EXPECT_TRUE(publisher_test.RegisterDataDescription((std::string) "test3.cdl", false, DESCRIPTION_TYPE_CDL))
      << "RegisterDataDescription failed";
}

TEST(register_data_description_test, register_dbc) {
  auto publisher_test = next::udex::publisher::DataPublisher("test");
  EXPECT_TRUE(publisher_test.RegisterDataDescription((std::string) "test4.dbc", false, DESCRIPTION_TYPE_DBC))
      << "RegisterDataDescription failed";
}

TEST(register_data_description_test, register_fibex) {
  auto publisher_test = next::udex::publisher::DataPublisher("test");
  EXPECT_TRUE(publisher_test.RegisterDataDescription((std::string) "test5.xml", false, DESCRIPTION_TYPE_FIBEX))
      << "RegisterDataDescription failed";
}

TEST(register_data_description_test, register_sdl_fail) {
  auto publisher_test = next::udex::publisher::DataPublisher("test");
  EXPECT_FALSE(publisher_test.RegisterDataDescription((std::string) "test1.sdl", false, DESCRIPTION_TYPE_FIBEX))
      << "RegisterDataDescription failed";
}

TEST(register_data_description_test, register_dbc_fail) {
  auto publisher_test = next::udex::publisher::DataPublisher("test");
  EXPECT_FALSE(publisher_test.RegisterDataDescription((std::string) "test4.dbc", false, DESCRIPTION_TYPE_SDL))
      << "RegisterDataDescription failed";
}

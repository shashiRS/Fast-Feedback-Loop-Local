#include <fstream>

#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <next/appsupport/cmd_options/cmd_options.hpp>
#include <next/appsupport/config/config_client.hpp>
#include <publisher_manager.hpp>
#include <resource_manager.hpp>

TEST(PublisherManager, Initialize) {
  const char *argv[] = {"cmdoption.exe"};

  next::appsupport::ConfigClient::do_init("next_player");
  auto cmd_options = next::appsupport::CmdOptions("next_player");

  next::appsupport::ConfigClient::getConfig()->putCfg("tst.json");

  bool ret = cmd_options.parseCmd(1, argv);
  ASSERT_EQ(ret, true);

  next::appsupport::ConfigClient::finish_init();
  std::shared_ptr<ResourceManager> resource_manager_sp(new ResourceManager());
  resource_manager_sp->Initialize();
  std::shared_ptr<PublisherManager> publisher_manager_sp(new PublisherManager());

  publisher_manager_sp->Initialize(resource_manager_sp);
  publisher_manager_sp->Setup();
  EXPECT_EQ(publisher_manager_sp->GetCycleIdFromConfigFile(), 0u);
  EXPECT_EQ(publisher_manager_sp->GetRealTimeFactorFromConfigFile(), 10.f);
  EXPECT_EQ(publisher_manager_sp->GetTimeStampFromConfigFile(), 40000u);

  next::appsupport::ConfigClient::do_reset();
}

TEST(PlayManager, check_recording_path) {
  std::string rec_file_name{"tst.rrec"};
  const char *argv[] = {"cmdoption.exe", "-r", rec_file_name.c_str()};

  next::appsupport::ConfigClient::do_init("Next_Player");
  auto cmd_options = next::appsupport::CmdOptions("Next_Player");
  cmd_options.addParameterString("recording,r", "path to a recording", "Next_Player:recording_path");

  ASSERT_TRUE(cmd_options.parseCmd(3, argv));
  next::appsupport::ConfigClient::finish_init();

  std::string recording_path =
      next::appsupport::ConfigClient::getConfig()->get_string("Next_Player:recording_path", "");
  EXPECT_EQ(recording_path, rec_file_name);
  std::ifstream file(recording_path.c_str());
  EXPECT_TRUE(file.good());
  next::appsupport::ConfigClient::do_reset();
}

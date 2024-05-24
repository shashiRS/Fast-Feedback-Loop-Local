/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     cmdoptions_test.cpp
 *  @brief    Testing the command line options interface
 */

#include <cip_test_support/gtest_reporting.h>

#include <regex>

#include <next/appsupport/cmd_options/cmd_options.hpp>
#include <next/appsupport/config/config_client.hpp>
#include <next/appsupport/config/config_key_generator.hpp>

#include "config.h"

using next::appsupport::CmdOptions;
using next::appsupport::ConfigClient;

const std::string kComponentName = "next_sdk";

class cmdoptions_test : public ::testing::Test {
protected:
  cmdoptions_test() : sbuf{nullptr}, sbuf_cerr{nullptr} {
    // intentionally empty
  }

  ~cmdoptions_test() override = default;

  // Called before each unit test
  void SetUp() override {
    // Save cout's buffer...
    sbuf = std::cout.rdbuf();
    sbuf_cerr = std::cerr.rdbuf();
    // Redirect cout to our stringstream buffer or any other ostream
    std::cout.rdbuf(buffer.rdbuf());
    next::appsupport::ConfigClient::do_init(kComponentName);
    auto comp_name = next::appsupport::ConfigClient::getConfig()->getRootName();
    next::appsupport::ConfigClient::getConfig()->put(next::appsupport::configkey::getMajorVersionKey(comp_name),
                                                     _major_number);
    next::appsupport::ConfigClient::getConfig()->put(next::appsupport::configkey::getMinorVersionKey(comp_name),
                                                     _minor_number);
    next::appsupport::ConfigClient::getConfig()->put(next::appsupport::configkey::getPatchVersionKey(comp_name),
                                                     _patch_number);
  }

  // Called after each unit test
  void TearDown() override {
    // When done redirect cout to its old self
    std::cout.rdbuf(sbuf);
    sbuf = nullptr;
    std::cerr.rdbuf(sbuf_cerr);
    sbuf_cerr = nullptr;
    ConfigClient::do_reset();
  }

  void RemoveInitChainOutput(std::string &text) {
    std::regex re(R"(\[InitChain\] .*\n)");
    text = std::regex_replace(text, re, "");
  }

  // This can be an ofstream as well or any other ostream
  std::stringstream buffer{};
  // Save cout's buffer here
  std::streambuf *sbuf;
  std::streambuf *sbuf_cerr;
  std::string _major_number = "100";
  std::string _minor_number = "200";
  std::string _patch_number = "300";
};

TEST_F(cmdoptions_test, basic_functionality) {
  auto cmd_options = CmdOptions("Testinterface");
  const char *argv[] = {"cmdoption.exe"};
  bool t = cmd_options.parseCmd(1, argv);

  std::string output{buffer.str()};
  RemoveInitChainOutput(output);
  EXPECT_EQ(t, true);
  EXPECT_EQ(output, "");
}

TEST_F(cmdoptions_test, help_functionality) {
  std::string help_result = "Testinterface:\n"
                            "  -h [ --help ]             produce help message\n"
                            "  -v [ --version ]          produce version message\n"
                            "  -c [ --config-files ] arg set one or multiple configuration files\n\n";

  auto cmd_options = CmdOptions("Testinterface");
  const char *argv[] = {"cmdoption.exe", "-h"};
  bool t = cmd_options.parseCmd(2, argv);

  std::string output{buffer.str()};
  RemoveInitChainOutput(output);
  EXPECT_EQ(t, false);
  EXPECT_EQ(output, help_result); // TODO make this a better check

  buffer.str(std::string());
  const char *argv1[] = {"cmdoption.exe", "--help"};
  t = cmd_options.parseCmd(2, argv1);

  std::string output1{buffer.str()};
  RemoveInitChainOutput(output1);
  EXPECT_EQ(t, false);
  EXPECT_EQ(output1, help_result); // TODO make this a better check
}

TEST_F(cmdoptions_test, version_functionality) {
  auto cmd_options = CmdOptions("Testinterface");
  const char *argv[] = {"cmdoption.exe", "-v"};
  bool t = cmd_options.parseCmd(2, argv);

  std::string output{buffer.str()};
  RemoveInitChainOutput(output);

  EXPECT_EQ(t, false);

  std::string t_ver = kComponentName;
  t_ver += " v";
  t_ver.append(_major_number);
  t_ver.append(".");
  t_ver.append(_minor_number);
  t_ver.append(".");
  t_ver.append(_patch_number);
  t_ver.append("\n");

  EXPECT_EQ(output, t_ver);

  buffer.str(std::string());
  const char *argv1[] = {"cmdoption.exe", "--version"};
  t = cmd_options.parseCmd(2, argv1);

  std::string output1{buffer.str()};
  RemoveInitChainOutput(output1);

  EXPECT_EQ(t, false);
  EXPECT_EQ(output1, t_ver);
}

TEST_F(cmdoptions_test, adding_parameters) {
  ConfigClient::do_init("test");
  auto cmd_options = CmdOptions("Testinterface");
  cmd_options.addParameterInt("debug_level,d", "debug level", "next_sdk:debug_level");
  cmd_options.addParameterFloat("frames_per_second,f", "player frequency", "next_sdk:frames_per_second");
  cmd_options.addParameterString("recording,r", "path to a recording", "next_sdk:recording_path");
  cmd_options.addParameterBool("testmode,t", "app test mode", "next_sdk:testmode");

  const char *argv[] = {"cmdoption.exe", "-d", "3", "-f", "10.7", "-r", "D:/super_track.txt", "--testmode"};
  bool t = cmd_options.parseCmd(8, argv);
  ConfigClient::finish_init();
  EXPECT_EQ(t, true);
  EXPECT_EQ(ConfigClient::getConfig()->get_int("next_sdk:debug_level", 0), 3);
  EXPECT_EQ(ConfigClient::getConfig()->get_float("next_sdk:frames_per_second", 0.0), 10.7F);
  EXPECT_EQ(ConfigClient::getConfig()->get_string("next_sdk:recording_path", ""), "D:/super_track.txt");
  EXPECT_EQ(ConfigClient::getConfig()->get_option("next_sdk:testmode", false), true);
}

TEST_F(cmdoptions_test, load_config_file) {
  ConfigClient::do_init("test");
  ConfigClient::getConfig()->addChangeHook([](std::string) -> bool { return true; });

  auto cmd_options = CmdOptions("Testinterface");
  const char *argv[] = {"cmdoption.exe", "-c", "cfg.json"};
  bool t = cmd_options.parseCmd(3, argv);
  EXPECT_EQ(t, true);

  ConfigClient::finish_init();
  // TODO: adapt with ticket SIMEN-9682
  // will be currently ignored because component name is different to added components
  // shall be changed when the config communication is able to filter out all components
  // which are not from this one when it's sending information to the server
  EXPECT_EQ(ConfigClient::getConfig()->get_string("CEM200_COH:version", ""), "");
  EXPECT_EQ(ConfigClient::getConfig()->get_int("CEM200_COH:in_port:m_pSefInputIf:sync:sync_timestamp", 0), 0);
  EXPECT_EQ(ConfigClient::getConfig()->get_int("not_available", 0), 0);

  EXPECT_EQ(ConfigClient::getConfig()->get_string("test:version", ""), "0.0.1");
  EXPECT_EQ(ConfigClient::getConfig()->get_int("test:in_port:m_pSefInputIf:sync:sync_timestamp", 0), 35);
  EXPECT_EQ(ConfigClient::getConfig()->get_int("not_available", 0), 0);

  // TODO fix exception here
  // const char *argv1[] = {"cmdoption.exe", "--ini", "tst_notValid.ini"};
  // t = cmd_options.parseCmd(3, argv1);

  std::string output{buffer.str()};
  RemoveInitChainOutput(output);
  EXPECT_EQ(t, true);
  EXPECT_EQ(output, "");
}
TEST_F(cmdoptions_test, help_as_string_functionality) {
  std::string help_result = "Testinterface:\n"
                            "  -h [ --help ]             produce help message\n"
                            "  -v [ --version ]          produce version message\n"
                            "  -c [ --config-files ] arg set one or multiple configuration files\n";

  auto cmd_options = CmdOptions("Testinterface");
  std::string helpString = cmd_options.getHelperString();
  EXPECT_EQ(helpString, help_result); // TODO make this a better check
}

TEST_F(cmdoptions_test, test_DuplicateComands_Success) {
  ConfigClient::do_init("test");
  buffer.str(std::string());
  std::cerr.rdbuf(buffer.rdbuf());
  auto cmd_options = CmdOptions("Test");
  cmd_options.addParameterString("different,r", "description", "test.different");
  cmd_options.addParameterString("different1,r", "description", "test.different1");
  std::string output{buffer.str()};
  std::string expected = "[CmdOptions] Key [r] is already defined; it will be ignored.";
  EXPECT_EQ(output.find(expected) != std::string::npos, true);

  buffer.str(std::string());
  cmd_options.addParameterString("identical,i1", "description", "test.identical");
  cmd_options.addParameterString("identical,i2", "description", "test.identical");
  std::string output2{buffer.str()};
  expected = "[CmdOptions] Key [identical] is already defined; it will be ignored.";
  EXPECT_EQ(output2.find(expected) != std::string::npos, true);

  const char *argv[] = {"cmdoption.exe", "-r", "recording", "--i1", "parValue"};
  bool t = cmd_options.parseCmd(5, argv);
  EXPECT_EQ(t, true);
}

TEST_F(cmdoptions_test, test_StichedComands_Fail) {
  ConfigClient::do_init("test");
  auto cmd_options = CmdOptions("Testinterface");
  cmd_options.addParameterInt("firstCMD,key", "debug level", "next_sdk:debug_level");
  cmd_options.addParameterFloat("secondCMD,key2", "player frequency", "next_sdk:frames_per_second");

  const char *argv[] = {"cmdoption.exe", "--key", "3--key2", "4"};
  buffer.str(std::string());
  std::cerr.rdbuf(buffer.rdbuf());
  bool t = cmd_options.parseCmd(4, argv);
  std::string output{buffer.str()};
  std::string expected =
      "[CmdOptions] Exception detected parsing command: the argument ('3--key2') for option '--firstCMD' is invalid";
  EXPECT_EQ(output.find(expected) != std::string::npos, true);
  ConfigClient::finish_init();
  EXPECT_EQ(t, false);
}

TEST_F(cmdoptions_test, test_SingleDashComands_Fail) {
  ConfigClient::do_init("test");
  auto cmd_options = CmdOptions("Testinterface");
  cmd_options.addParameterInt("firstCMD,key", "debug level", "next_sdk:debug_level");
  cmd_options.addParameterFloat("secondCMD,key2", "player frequency", "next_sdk:frames_per_second");

  const char *argv[] = {"cmdoption.exe", "-key", "3", "--key2", "4"};

  buffer.str(std::string());
  std::cerr.rdbuf(buffer.rdbuf());
  bool t = cmd_options.parseCmd(5, argv);
  std::string output{buffer.str()};
  std::string expected = "[CmdOptions] please use double dash [--] for command: [key]";
  EXPECT_EQ(output.find(expected) != std::string::npos, true);
  ConfigClient::finish_init();
  EXPECT_EQ(t, true);
}

TEST_F(cmdoptions_test, test_OptionUsed) {
  ConfigClient::do_init("test");
  buffer.str(std::string());
  std::cerr.rdbuf(buffer.rdbuf());
  auto cmd_options = CmdOptions("Test");
  cmd_options.addParameterString("recording,r", "recording name", "test.recording_name");

  const char *argv[] = {"cmdoption.exe", "-r", "recording"};
  bool t = cmd_options.parseCmd(3, argv);
  EXPECT_EQ(t, true);

  bool used = cmd_options.OptionUsed("recording");
  EXPECT_EQ(used, true);

  bool not_used = cmd_options.OptionUsed("invalid_option");
  EXPECT_EQ(not_used, false);
}

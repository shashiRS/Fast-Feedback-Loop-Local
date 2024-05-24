/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     user_hash_test.cpp
 *  @brief    testing the user hash functionality
 */

#include <cip_test_support/gtest_reporting.h>

#include <next/sdk/user_hash/user_hash.hpp>

class UserHashTest : public next::sdk::UserHash {
public:
  UserHashTest() : UserHash() {}
  bool GetLoginName(std::string &user_name) { return next::sdk::UserHash::GetLoginName(user_name); }
  bool GetMacAdress(std::string &mac_address) { return next::sdk::UserHash::GetMacAdress(mac_address); }
};

TEST(USER_HASH_TEST, CreateHash) {
  size_t hash = 0;
  EXPECT_TRUE(next::sdk::UserHash::CreateHash(hash));
  EXPECT_NE(0, hash);
}

TEST(USER_HASH_TEST, GetLoginName) {
  UserHashTest userhash;
  std::string username = "";
  EXPECT_TRUE(userhash.GetLoginName(username));
  EXPECT_NE(username, "dummyUser");
}

TEST(USER_HASH_TEST, GetMacAdress) {
  UserHashTest userhash;
  std::string mac_adress = "00-00-00-00-00-00";
  EXPECT_TRUE(userhash.GetMacAdress(mac_adress));
  EXPECT_NE(mac_adress, "00-00-00-00-00-00");
}

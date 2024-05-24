/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file    package_hash.cpp
 *  @brief   Testing the package_hash
 */

#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <functional>

#include <next/sdk/types/package_hash.hpp>

TEST(PackageHash, check_hash_unique_linux_issue_success) {

  next::sdk::types::IPackage::PackageMetaInfo info;
  info.virtual_address = 0;
  info.cycle_id = 99;
  info.instance_number = 0;
  info.source_id = 783;
  auto hash1 = next::sdk::types::PackageHash::hashMetaInfo(info);

  next::sdk::types::IPackage::PackageMetaInfo info1;
  info1 = info;
  // override only instance
  info1.instance_number = 7;
  auto hash2 = next::sdk::types::PackageHash::hashMetaInfo(info1);

  EXPECT_NE(hash1, hash2) << "hashes are not unique";
}

TEST(PackageHash, check_hash_exaustive_success) {

  std::unordered_map<uint64_t, uint64_t> hash_list;
  uint64_t counter = 0;

  for (uint16_t i_sid = 0; i_sid < 300; i_sid++) {
    for (uint32_t i_insid = 0; i_insid < 10; i_insid++) {
      for (uint32_t i_cycid = 0; i_cycid < 10; i_cycid++) {
        next::sdk::types::IPackage::PackageMetaInfo info;
        info.virtual_address = 0x2000000;
        info.cycle_id = i_cycid;
        info.instance_number = i_insid;
        info.source_id = i_sid;
        auto hash1 = next::sdk::types::PackageHash::hashMetaInfo(info);
        auto element_hit = hash_list.find(hash1);
        if (element_hit != hash_list.end()) {
          EXPECT_TRUE(false);
          std::cout << hash1 << "was not unique found already at " << element_hit->second
                    << "checkhash: " << element_hit->first << std::endl;
        }
        hash_list.insert({hash1, counter});
        counter++;
      }
    }
  }
}

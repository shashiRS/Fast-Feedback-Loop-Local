/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     data_publisher_test.cpp
 *  @brief    testing high-level functionality of udex
 */

#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <ecal/ecal.h>
#include <ecal/msg/string/publisher.h>

#include <next/sdk/sdk.hpp>

#include <next/udex/publisher/data_publisher.hpp>
#include <next/udex/subscriber/data_subscriber.hpp>

#include <next/sdk/sdk_macros.h>

using next::udex::publisher::DataPublisher;

namespace {

class ComponentTestDataPublisherTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  ComponentTestDataPublisherTestFixture() { this->instance_name_ = "DataPublisherTestFixture"; }
};

TEST_F(ComponentTestDataPublisherTestFixture, DataPublisher) {
  DataPublisher *publisher = new DataPublisher("pub");
  delete publisher;
}

TEST_F(ComponentTestDataPublisherTestFixture, Reset) {
  DataPublisher publisher = DataPublisher("pub");
  publisher.Reset();
}

TEST_F(ComponentTestDataPublisherTestFixture, getName) {
  DataPublisher publisher = DataPublisher("pub");
  auto _ = publisher.getName();
  EXPECT_EQ(_, std::string("pub"));
}

TEST_F(ComponentTestDataPublisherTestFixture, SetDataSourceInfo) {
  DataPublisher publisher = DataPublisher("pub");
  EXPECT_TRUE(publisher.SetDataSourceInfo((const char *)"SDL_CONTENT", (size_t)1, (size_t)1));
}

TEST_F(ComponentTestDataPublisherTestFixture, RegisterDataDescription) {
  DataPublisher publisher = DataPublisher("pub");
  EXPECT_TRUE(publisher.RegisterDataDescription(std::string("SIM VFB.view.group1"), (size_t)1));
  EXPECT_TRUE(publisher.RegisterDataDescription(std::string("test.sdl")));
}

TEST_F(ComponentTestDataPublisherTestFixture, getTopicsList) {
  DataPublisher publisher = DataPublisher("pub");
  NEXT_DISABLE_DEPRECATED_WARNING
  auto &topic_list_from_memory = publisher.getTopicsAndHashes();
  NEXT_RESTORE_WARNINGS
  EXPECT_TRUE(topic_list_from_memory.empty());
}

TEST_F(ComponentTestDataPublisherTestFixture, getTopicHash) {
  DataPublisher publisher = DataPublisher("pub");
  publisher.RegisterDataDescription((std::string) "SIM VFB.view.group1", (size_t)1);
  publisher.RegisterDataDescription((std::string) "SIM VFB.view.group2", (size_t)1);
  auto &topic_list = publisher.getTopicsAndHashes();
  EXPECT_EQ(topic_list.size(), 2);
  auto found_hash = topic_list.find("view.group1");
  EXPECT_EQ(found_hash, topic_list.end());

  const uint64_t hash1_expected{18087543040950864322u};
  const uint64_t hash2_expected{14548804174869639121u};
  found_hash = topic_list.find("SIM VFB.view.group1");
  ASSERT_NE(found_hash, topic_list.end());
  EXPECT_EQ(found_hash->second, hash1_expected);
  found_hash = topic_list.find("SIM VFB.view.group2");
  EXPECT_EQ(found_hash->second, hash2_expected);
}

TEST_F(ComponentTestDataPublisherTestFixture, publishData) {
  std::string url = "url";
  std::string node_name = "generic_node_publisher";
  auto subscriber = next::udex::subscriber::DataSubscriber(node_name);
  auto extractor = std::make_shared<next::udex::extractor::DataExtractor>();
  auto publisher = DataPublisher(node_name);
  eCAL::Util::EnableLoopback(true);
  NEXT_DISABLE_DEPRECATED_WARNING
  auto id1 = subscriber.Subscribe(url, extractor);
  EXPECT_NE(id1, 0);
  size_t size = 1;
  publisher.RegisterDataDescription(url, size, true);
  auto sub_info = subscriber.getSubscriptionsInfo();
  auto &topic_list = publisher.getTopicsAndHashes();
  NEXT_RESTORE_WARNINGS
  EXPECT_GE(topic_list.size(), 1);
  std::string payload("payload");
  publisher.publishData(url, payload.data(), payload.size());
}

} // namespace

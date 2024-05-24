/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     data_subscriber_test.cpp
 *  @brief    testing high-level functionality of udex
 */

#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <next/sdk/sdk.hpp>

#include <next/udex/subscriber/data_subscriber.hpp>

using next::udex::subscriber::DataSubscriber;

namespace {

class DataSubscriberTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  DataSubscriberTestFixture() { this->instance_name_ = "DataSubscriberTestFixture"; }
};

TEST_F(DataSubscriberTestFixture, DataSubscriber) {
  DataSubscriber *subscriber = new DataSubscriber("sub");
  delete subscriber;
}

TEST_F(DataSubscriberTestFixture, Reset) {
  DataSubscriber subscriber = DataSubscriber("sub");
  subscriber.Reset();
}

TEST_F(DataSubscriberTestFixture, getName) {
  DataSubscriber subscriber = DataSubscriber("sub");
  auto _ = subscriber.getName();
}

TEST_F(DataSubscriberTestFixture, Subscribe) {
  std::string url = "url";
  auto extractor = std::make_shared<next::udex::extractor::DataExtractor>();
  DataSubscriber subscriber = DataSubscriber("sub");
  auto id = subscriber.Subscribe(url, extractor);
  EXPECT_NE(id, 0u);
}

TEST_F(DataSubscriberTestFixture, Unsubscribe) {
  std::string url = "url";
  auto extractor = std::make_shared<next::udex::extractor::DataExtractor>();
  DataSubscriber subscriber = DataSubscriber("sub");
  auto id = subscriber.Subscribe(url, extractor);
  subscriber.Unsubscribe(id);
}

TEST_F(DataSubscriberTestFixture, getSubscriptionsInfo) {
  DataSubscriber subscriber = DataSubscriber("sub");
  auto _ = subscriber.getSubscriptionsInfo();
  EXPECT_TRUE(_.empty());
}

} // namespace

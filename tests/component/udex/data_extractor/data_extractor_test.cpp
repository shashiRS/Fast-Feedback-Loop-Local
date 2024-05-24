/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     data_extractor_test.cpp
 *  @brief    testing high-level functionality of udex
 */

#include <atomic>
#include <numeric>

#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <next/sdk/sdk_macros.h>

#include <next/udex/publisher/data_publisher.hpp>
#include <next/udex/subscriber/data_subscriber.hpp>

using next::udex::extractor::DataExtractor;

namespace {

TEST(data_extractor_component, DataExtractor_Constructor_ConstructionPossible) {
  std::string adapter_dll = "adapter_dll";
  unsigned short extractionQueueMaxSize = 10;
  auto extractor1 = std::make_shared<DataExtractor>();
  auto extractor2 = std::make_shared<DataExtractor>(adapter_dll);
  auto extractor3 = std::make_shared<DataExtractor>(adapter_dll, extractionQueueMaxSize);
}

TEST(data_extractor_component, PushData) {
  const std::string topic_name = "topic_name";
  DataExtractor extractor;
  extractor.PushData(topic_name, nullptr);
}

TEST(data_extractor_component, GetRawData) {
  const std::string topic_name = "topic_name";
  DataExtractor extractor;
  EXPECT_NO_THROW(extractor.GetRawData(topic_name));
}

TEST(data_extractor_component, isDataAvailable) {
  const std::string topic_name = "topic_name";
  DataExtractor extractor;
  EXPECT_NO_THROW(extractor.isDataAvailable(topic_name));
}

TEST(data_extractor_component, GetExtractedData) {
  const std::string topic_name = "topic_name";
  DataExtractor extractor;
  EXPECT_NO_THROW(extractor.GetExtractedData(topic_name));
}

TEST(data_extractor_component, GetExtractedQueue) {
  const std::string topic_name = "topic_name";
  DataExtractor extractor;
  NEXT_DISABLE_DEPRECATED_WARNING
  EXPECT_NO_THROW(extractor.GetExtractedQueueWithTimestamp(topic_name));
  NEXT_RESTORE_WARNINGS
}

TEST(data_extractor_component, GetExtractedQueueWithTimestamp) {
  const std::string topic_name = "topic_name";
  DataExtractor extractor;
  EXPECT_NO_THROW(extractor.GetExtractedQueueWithTimestamp(topic_name));
}

TEST(data_extractor_component, ClearExtractedQueue) {
  const std::string topic_name = "topic_name";
  DataExtractor extractor;
  extractor.ClearExtractedQueue(topic_name);
}

TEST(data_extractor_component, RemoveElementsFromQueue) {
  const std::string topic_name = "topic_name";
  DataExtractor extractor;
  EXPECT_NO_THROW(extractor.RemoveElementsFromQueue(topic_name, 0));
}

TEST(data_extractor_component, GetValue) {
  const std::string signal_url = "signal_url";
  next::udex::dataType return_value;
  DataExtractor extractor;
  EXPECT_NO_THROW(extractor.GetValue(signal_url, next::udex::SignalType::SIGNAL_TYPE_FLOAT, return_value));
}

TEST(data_extractor_component, GetVectorValue) {
  const std::string signal_url = "signal_url";
  std::vector<next::udex::dataType> return_vector;
  DataExtractor extractor;
  EXPECT_NO_THROW(extractor.GetVectorValue(signal_url, next::udex::SignalType::SIGNAL_TYPE_FLOAT, return_vector));
}

TEST(data_extractor_component, GetValueAndInfo) {
  const std::string signal_url = "signal_url";
  next::udex::SignalInfo signal_info;
  std::vector<next::udex::dataType> return_vector;
  DataExtractor extractor;
  EXPECT_NO_THROW(extractor.GetValueAndInfo(signal_url, signal_info, return_vector));
}

TEST(data_extractor_component, GetInfo) {
  const std::string signal_url = "signal_url";
  DataExtractor extractor;
  EXPECT_NO_THROW(extractor.GetInfo(signal_url));
}

TEST(data_extractor_component, connectCallback) {
  next::udex::UdexCallbackT cb = [](const std::string &s) { (void)s; };
  std::string signal_url = "signal_url";
  DataExtractor extractor;
  EXPECT_NE(extractor.connectCallback(cb, signal_url), 0u);
}

TEST(data_extractor_component, disconnectCallback) {
  size_t id = 10;
  DataExtractor extractor;
  EXPECT_NO_THROW(extractor.disconnectCallback(id));
}

TEST(data_extractor_component, getCallbacksInfo) {
  DataExtractor extractor;
  EXPECT_NO_THROW(extractor.getCallbacksInfo());
}

TEST(data_extractor_component, SetExtractionVersion) {
  const std::string port_name = "port_name";
  const size_t port_size = 10;
  const std::string version = "1.2.3.4";
  const std::string data_type_name = "data_type_name";
  DataExtractor extractor;
  EXPECT_NO_THROW(extractor.SetExtractionVersion(port_name, port_size, version, data_type_name));
}

TEST(data_extractor_component, SetBlockingExtractionForUrl) {
  const std::string url = "url";
  const bool blocking_flag = false;
  DataExtractor extractor;
  EXPECT_NO_THROW(extractor.SetBlockingExtractionForUrl(url, blocking_flag));
}

TEST(data_extractor_component, GetSynchExtractedData) {
  const std::string topic_name = "topic_name";
  DataExtractor extractor;
  std::vector<next::udex::SyncValue> sync_values;

  EXPECT_NO_THROW(extractor.GetSyncExtractedData(topic_name, sync_values));
}

TEST(data_extractor_component, SetSyncAlgo) {
  const std::string topic_name = "topic_name";
  DataExtractor extractor;
  next::udex::SyncAlgoCfg sync_algo;
  EXPECT_NO_THROW(extractor.SetSyncAlgo(topic_name, sync_algo));
}

TEST(data_extractor_component, setDefaultSync) {
  const std::string topic_name = "topic_name";
  DataExtractor extractor;
  EXPECT_NO_THROW(extractor.setDefaultSync(topic_name));
}

TEST(data_extractor_component, SetDropExtraInput) {
  bool drop_extra_input = false;
  DataExtractor extractor;
  EXPECT_NO_THROW(extractor.SetDropExtraInput(drop_extra_input));
}

} // namespace

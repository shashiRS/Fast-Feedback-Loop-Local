/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
/**
 * @file     data_extractor.cpp
 * @brief    Class to handle data extraction, extracting signals value and info
 */

#include <next/udex/extractor/data_extractor.hpp>
#include "data_extractor_impl.h"

namespace next {
namespace udex {
namespace extractor {

DataExtractor::DataExtractor() { impl_ = std::make_unique<DataExtractorImpl>("", EXTRACTION_QUEUE_DEFAULT_SIZE); }

DataExtractor::DataExtractor(const std::string &adapter_dll) {
  impl_ = std::make_unique<DataExtractorImpl>(adapter_dll, EXTRACTION_QUEUE_DEFAULT_SIZE);
}

DataExtractor::DataExtractor(unsigned short extractionQueueMaxSize) {
  impl_ = std::make_unique<DataExtractorImpl>("", extractionQueueMaxSize);
}

DataExtractor::DataExtractor(const std::string &adapter_dll, unsigned short extractionQueueMaxSize) {
  impl_ = std::make_unique<DataExtractorImpl>(adapter_dll, extractionQueueMaxSize);
}

DataExtractor::~DataExtractor() { impl_.reset(); }

size_t DataExtractor::connectCallback(next::udex::UdexCallbackT cb, std::string signal_url) {
  if (cb != nullptr) {
    return impl_->connectCallback(cb, signal_url);
  }

  return 0;
}

void DataExtractor::disconnectCallback(size_t id) { impl_->disconnectCallback(id); }

std::map<size_t, CallbackInfo> DataExtractor::getCallbacksInfo() { return impl_->getCallbacksInfo(); }

// input functions
void DataExtractor::PushData(const std::string &topic_name, const next::udex::UdexDataPackage *data) {
  if (data != nullptr) {
    impl_->PushData(topic_name, data);
  }
}

// output functions
UdexDataPackage DataExtractor::GetRawData(const std::string &topic_name) { return impl_->GetRawData(topic_name); }

bool DataExtractor::isDataAvailable(const std::string &topic_name) { return impl_->isDataAvailable(topic_name); }

TimestampedDataPacket DataExtractor::GetExtractedData(const std::string &signal_url) {
  return impl_->GetExtractedData(signal_url);
}

std::list<std::shared_ptr<TimestampedDataPacket>>
DataExtractor::GetExtractedQueueWithTimestamp(const std::string &topic_name) {
  return impl_->GetExtractedQueueWithTimestamp(topic_name);
}

void DataExtractor::ClearExtractedQueue(const std::string &topic_name) { impl_->ClearExtractedQueue(topic_name); }

void DataExtractor::RemoveElementsFromQueue(const std::string &topic_name, const size_t nr_of_elements_to_remove) {
  impl_->RemoveElementsFromQueue(topic_name, nr_of_elements_to_remove);
}

bool DataExtractor::GetValue(const std::string &signal_url, const SignalType type, dataType &return_value,
                             const SignalInfo *signal_info) {
  return impl_->GetValue(signal_url, type, return_value, signal_info);
}

bool DataExtractor::GetVectorValue(const std::string &signal_url, const SignalType type,
                                   std::vector<dataType> &return_vector, const SignalInfo *signal_info) {
  return impl_->GetVectorValue(signal_url, type, return_vector, signal_info);
}

bool DataExtractor::GetValueAndInfo(const std::string &signal_url, SignalInfo &signal_info,
                                    std::vector<dataType> &return_vector) {
  return impl_->GetValueAndInfo(signal_url, signal_info, return_vector);
}

SignalInfo DataExtractor::GetInfo(const std::string &signal_url) { return impl_->GetInfo(signal_url); }

void DataExtractor::SetExtractionVersion(const std::string &port_name, const size_t port_size,
                                         const std::string &version, const std::string &data_type_name) {
  impl_->SetExtractionVersion(port_name, port_size, version, data_type_name);
}

unsigned short DataExtractor::GetExtractionQueueMaxSize() { return impl_->GetExtractionQueueMaxSize(); }

void DataExtractor::SetExtractionVersion(const std::string &port_name, const size_t port_size, const uint32_t version,
                                         const std::string &data_type_name) {
  impl_->SetExtractionVersion(port_name, port_size, version, data_type_name);
}

uint32_t DataExtractor::ConvertVersion(const std::string &version) { return impl_->ConvertVersion(version); }

void DataExtractor::SetBlockingExtraction(const bool blocking_flag) { impl_->SetBlockingExtraction(blocking_flag); }

void DataExtractor::SetBlockingExtractionForUrl(const std::string &url, const bool blocking_flag) {
  impl_->SetBlockingExtractionForUrl(url, blocking_flag);
}

TimestampedDataPacket DataExtractor::GetSyncExtractedData(const std::string &signal_url,
                                                          const std::vector<SyncValue> &sync_values) {
  return impl_->GetSyncExtractedData(signal_url, sync_values);
}

void DataExtractor::SetSyncAlgo(std::string signal_url, SyncAlgoCfg sync_cfg) {
  impl_->SetSyncAlgo(signal_url, sync_cfg);
}

void DataExtractor::setDefaultSync(std::string signal_url) { impl_->setDefaultSync(signal_url); }

void DataExtractor::SetDropExtraInput(const bool drop_extra_input) { impl_->SetDropExtraInput(drop_extra_input); }

void DataExtractor::setSizeDiffThreshold(const size_t diff) { impl_->setSizeDiffThreshold(diff); }

TimestampedDataPacket DataExtractor::InsertEmptyDataPacket(const std::string &signal_url, const std::size_t size) {
  return impl_->InsertEmptyDataPacket(signal_url, size);
}

} // namespace extractor
} // namespace udex
} // namespace next

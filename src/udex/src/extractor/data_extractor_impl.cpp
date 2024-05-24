/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
/*
 * @file     data_extractor_impl.cpp
 * @brief    matches the old Udex implementation to the new interface with
 *           regards to the data_extractor interface
 */

#include "data_extractor_impl.h"

#include <exception>

#include <next/sdk/logger/logger.hpp>
#include <next/sdk/string_helper/strings.hpp>

#include <next/udex/helper_data_extractor.h>

namespace next {
namespace udex {
namespace extractor {

void DataExtractorImpl::setSizeDiffThreshold(const size_t diff) { signal_sdl_size_diff_threshold_ = diff; }

void DataExtractorImpl::setDefaultSync(std::string signal_url) {
  if (sync_algo_for_signal_.count(signal_url) == 0) {
    sync_algo_for_signal_[signal_url] = std::make_shared<SyncAlgoLoader>();
  }
}

DataExtractorImpl::DataExtractorImpl(const std::string &adapter_dll, unsigned short extractionQueueMaxSize) {
  ProfileUdex_FLL;

  // set max queue size for each queue topic. This should be done in push data when we initialize a queue for a new
  // topic temporary
  extraction_queue_max_size_ = extractionQueueMaxSize;

  if (adapter_dll.empty()) {
    // maybe remove this if it's too much spam, it's info, not error
    debug(__FILE__, " No adapter dll provided");
  } else {
    adapter_ptr_ = gia_dll_handler_.GetAdapter(adapter_dll);
  }
}

DataExtractorImpl::~DataExtractorImpl() {

  std::lock_guard<std::recursive_mutex> lock(block_extraction_queue_mutex_);
  // Wait on all callbacks to be finished and clean up map afterwards
  for (auto &queue : udex_topic_queue_mp_) {
    std::map<size_t, std::pair<next::udex::UdexCallbackT, std::string>> dummy_clear_map;
    queue.second->setQueueCallbacks(dummy_clear_map);
  }
  udex_topic_queue_mp_.clear();
}

size_t DataExtractorImpl::connectCallback(next::udex::UdexCallbackT cb, std::string signal_url) {
  ProfileUdex_DTL;
  std::vector<std::string> topic_name = signal_explorer_->GetPackageUrl(signal_url);
  if (topic_name.size() == 0) {
    topic_name.push_back(signal_url);
  }
  callback_connection_id_++;

  CallbackInfo callback_info;
  callback_info.callback_function = cb;
  callback_info.url = signal_url;
  callback_info.topic = topic_name[0];

  callbacks_info_[callback_connection_id_] = callback_info;

  map_callbacks_and_urls_to_topics_[topic_name[0]][callback_connection_id_] = std::make_pair(cb, signal_url);

  if (udex_topic_queue_mp_.find(topic_name[0]) != udex_topic_queue_mp_.end()) {
    // if the queue already exists, forward to queue
    udex_topic_queue_mp_[topic_name[0]]->setQueueCallbacks(map_callbacks_and_urls_to_topics_[topic_name[0]]);
  }

  return callback_connection_id_;
}

void DataExtractorImpl::disconnectCallback(size_t id) {
  ProfileUdex_DTL;
  std::string topic_from_id = "";

  for (auto callback_and_url_map : map_callbacks_and_urls_to_topics_) {
    auto topic = callback_and_url_map.first;
    for (auto id_to_callback_url_pair : callback_and_url_map.second) {
      auto iterated_id = id_to_callback_url_pair.first;
      // auto callback_url_pair = id_to_callback_url_pair.second;

      if (id == iterated_id) {
        topic_from_id = topic;
        break;
      }
    }

    if (topic_from_id != "")
      break;
  }

  if (auto it = map_callbacks_and_urls_to_topics_.find(topic_from_id); it != map_callbacks_and_urls_to_topics_.end()) {
    it->second.erase(id);
  }

  if (auto it = udex_topic_queue_mp_.find(topic_from_id); it != udex_topic_queue_mp_.end()) {
    it->second->setQueueCallbacks(map_callbacks_and_urls_to_topics_[topic_from_id]);
  }

  callbacks_info_.erase(id);
}

std::map<size_t, CallbackInfo> DataExtractorImpl::getCallbacksInfo() { return callbacks_info_; }

void DataExtractorImpl::SetBlockingExtraction(const bool blocking_flag) {
  blocking_flag_ = blocking_flag;

  // set it for currently existing queue
  for (auto queue : udex_topic_queue_mp_) {
    queue.second->SetBlockingFlag(blocking_flag_);
  }
}

void DataExtractorImpl::SetDropExtraInput(const bool drop_extra_input) {
  // set it for queues that will be created
  drop_extra_input_ = drop_extra_input;

  // set it for currently existing queue
  for (auto queue : udex_topic_queue_mp_) {
    queue.second->SetDropExtraInput(drop_extra_input_);
  }
}

void DataExtractorImpl::SetBlockingExtractionForUrl(const std::string &url, const bool blocking_flag) {
  auto topic = signal_explorer_->GetPackageUrl(url);
  if (topic.size() == 0) {
    warn(__FILE__, "got empty reply from GetPackageUrl for url '{0}'", url);
    return;
  }

  if (topic[0] == "") { // getTopicByUrl failed, topic not yet in the database due to other issues,
    topic[0] = url;
  }

  if (udex_topic_queue_mp_.find(topic[0]) != udex_topic_queue_mp_.end()) {
    udex_topic_queue_mp_[topic[0]]->SetBlockingFlag(blocking_flag);
  }

  blocking_flag_map_[topic[0]] = blocking_flag;
}

// input functions
void DataExtractorImpl::PushData(const std::string &topic_name, const next::udex::UdexDataPackage *data) {
  ProfileUdex_DTL;
  std::lock_guard<std::recursive_mutex> lock(block_extraction_queue_mutex_);
  // check if the topic is new and no instance of the udex_queue is created yet.
  if (udex_topic_queue_mp_.find(topic_name) == udex_topic_queue_mp_.end()) {

    // pre init queue
    IConverter *converter_ptr = nullptr;
    if (adapter_ptr_) {
      std::string converter_name_s = struct_info_map_.at(topic_name).converter;
      std::wstring converter_name_ws = std::wstring(converter_name_s.begin(), converter_name_s.end());
      converter_ptr = adapter_ptr_->CreateConverter(converter_name_ws.c_str());
    }

    bool is_available = false;
    auto signal_description = signal_explorer_->getSignalDescription(topic_name, is_available);
    std::string schema(signal_description.binary_data_description.begin(),
                       signal_description.binary_data_description.end());

    SignalInfo signal_info = GetInfo(topic_name);

    // use format_type from UdexDataPackage because signal_description contains only
    // the general format type "PACKAGE_FORMAT_TYPE_MTA" and doesn't distinguish between
    // "PACKAGE_FORMAT_TYPE_MTA_SW" and "PACKAGE_FORMAT_TYPE_MTA_HW"
    PackageInfo package_info = {signal_description.basic_info.data_source_name,
                                signal_description.basic_info.source_id,
                                signal_description.basic_info.instance_id,
                                signal_description.basic_info.cycle_id,
                                data->info.format_type,
                                data->info.processor_required};

    // init queue
    udex_topic_queue_mp_[topic_name] = std::make_shared<UDex_Queue>(converter_ptr, schema, package_info, signal_info,
                                                                    blocking_flag_, drop_extra_input_);

    // set max que size for the new topic
    udex_topic_queue_mp_[topic_name]->setQueueMaxSize(extraction_queue_max_size_);

    udex_topic_queue_mp_[topic_name]->setSizeDiffThreshold(signal_sdl_size_diff_threshold_);

    // post init queue
    auto callbacks_and_urls_to_topics = map_callbacks_and_urls_to_topics_.find(topic_name);
    if (callbacks_and_urls_to_topics != map_callbacks_and_urls_to_topics_.end()) {
      udex_topic_queue_mp_[topic_name]->setQueueCallbacks(callbacks_and_urls_to_topics->second);
    }

    auto struct_info = struct_info_map_.find(topic_name);
    if (struct_info != struct_info_map_.end()) {
      udex_topic_queue_mp_[topic_name]->setStructInfo(&struct_info->second);
    }

    auto blocking_flag = blocking_flag_map_.find(topic_name);
    if (blocking_flag != blocking_flag_map_.end()) {
      udex_topic_queue_mp_[topic_name]->SetBlockingFlag(blocking_flag->second);
    }
  } else if (udex_topic_queue_mp_[topic_name]->getFormatType() ==
                 next::sdk::types::PackageFormatType::PACKAGE_FORMAT_TYPE_UNKNOWN &&
             data->info.format_type != next::sdk::types::PackageFormatType::PACKAGE_FORMAT_TYPE_UNKNOWN) {
    // it may be that we added a placeholder queue for non exisiting urls
    // we can update some data if actual data is received
    bool is_available = false;
    auto signal_description = signal_explorer_->getSignalDescription(topic_name, is_available);

    // use format_type from UdexDataPackage because signal_description contains only
    // the general format type "PACKAGE_FORMAT_TYPE_MTA" and doesn't distinguish between
    // "PACKAGE_FORMAT_TYPE_MTA_SW" and "PACKAGE_FORMAT_TYPE_MTA_HW"
    PackageInfo package_info = {signal_description.basic_info.data_source_name,
                                signal_description.basic_info.source_id,
                                signal_description.basic_info.instance_id,
                                signal_description.basic_info.cycle_id,
                                data->info.format_type,
                                data->info.processor_required};

    std::string schema(signal_description.binary_data_description.begin(),
                       signal_description.binary_data_description.end());
    udex_topic_queue_mp_[topic_name]->setSchema(schema);
    udex_topic_queue_mp_[topic_name]->setPackageInfo(package_info);
  }
  udex_topic_queue_mp_[topic_name]->Push(topic_name, data);
}

// output functions
UdexDataPackage DataExtractorImpl::GetRawData(const std::string &topic_name) {
  ProfileUdex_FLL;
  if (udex_topic_queue_mp_.find(topic_name) == udex_topic_queue_mp_.end()) {
    // no data related to that topic available
    return UdexDataPackage{};
  }

  return udex_topic_queue_mp_[topic_name]->GetRawData();
}

bool DataExtractorImpl::isDataAvailable(const std::string &topic_name) {
  std::shared_ptr<UDex_Queue> temp_queue;
  {
    std::lock_guard<std::recursive_mutex> lock(block_extraction_queue_mutex_);
    if (udex_topic_queue_mp_.find(topic_name) == udex_topic_queue_mp_.end()) {
      // no data related to that topic available
      return false;
    }
    temp_queue = udex_topic_queue_mp_[topic_name];
  }
  return temp_queue->isDataAvailable();
}

TimestampedDataPacket DataExtractorImpl::GetSyncExtractedData(const std::string &signal_url,
                                                              const std::vector<SyncValue> &sync_values) {
  ProfileUdex_FLL;
  bool extracted = false;

  auto sync_algo_it = sync_algo_for_signal_.find(signal_url);

  if (sync_algo_it == sync_algo_for_signal_.end()) {
    error(__FILE__, "URL {0} was not subscribed to before GetSyncExtractedData call", signal_url);
    return TimestampedDataPacket{nullptr, 0, 0};
  }

  size_t nr_of_elements_to_remove = 0;

  std::list<std::shared_ptr<TimestampedDataPacket>> local_list = GetExtractedQueueWithTimestamp(signal_url);
  const TimestampedDataPacket return_elem =
      sync_algo_it->second->CallSyncAlogo(local_list, extracted, sync_values, nr_of_elements_to_remove);
  if (extracted) {
    RemoveElementsFromQueue(signal_url, nr_of_elements_to_remove);

    topic_buffer_[signal_url] = return_elem;
    return return_elem;
  }

  return TimestampedDataPacket{nullptr, 0, 0};
}

void DataExtractorImpl::SetSyncAlgo(std::string signal_url, SyncAlgoCfg sync_cfg) {
  SignalInfo signal_info = GetInfo(sync_cfg.composition_sync_signal_url);

  if (sync_algo_for_signal_.count(signal_url) == 0) {
    sync_algo_for_signal_[signal_url] = std::make_shared<SyncAlgoLoader>();
  }

  sync_algo_for_signal_[signal_url]->ConfigureSyncAlgo(sync_cfg, signal_info);
}

TimestampedDataPacket DataExtractorImpl::GetExtractedData(const std::string &signal_url) {
  ProfileUdex_FLL;
  std::list<std::shared_ptr<TimestampedDataPacket>> local_list = GetExtractedQueueWithTimestamp(signal_url);

  if (local_list.size() > 0) {
    std::shared_ptr<TimestampedDataPacket> obj = local_list.back();
    return *obj;
  }
  // no data related to that topic available
  return TimestampedDataPacket{nullptr, 0, 0};
}

std::list<std::shared_ptr<std::tuple<void *, size_t>>>
DataExtractorImpl::GetExtractedQueue(const std::string &topic_name) {
  ProfileUdex_OV;
  if (udex_topic_queue_mp_.find(topic_name) == udex_topic_queue_mp_.end()) {
    // no data related to that topic available
    return std::list<std::shared_ptr<std::tuple<void *, size_t>>>();
  }

  auto temp_queue = udex_topic_queue_mp_[topic_name]->PullQueue();
  std::list<std::shared_ptr<std::tuple<void *, size_t>>> result_list;
  for (auto element : temp_queue) {
    result_list.push_back(std::make_shared<std::tuple<void *, size_t>>(std::make_tuple(element->data, element->size)));
  }
  return result_list;
}

std::list<std::shared_ptr<TimestampedDataPacket>>
DataExtractorImpl::GetExtractedQueueWithTimestamp(const std::string &topic_name) {
  std::shared_ptr<UDex_Queue> temp_queue;
  {
    std::lock_guard<std::recursive_mutex> lock(block_extraction_queue_mutex_);
    if (udex_topic_queue_mp_.find(topic_name) == udex_topic_queue_mp_.end()) {
      // no data related to that topic available
      return std::list<std::shared_ptr<TimestampedDataPacket>>();
    }
    temp_queue = udex_topic_queue_mp_[topic_name];
  }
  return temp_queue->PullQueue();
}

void DataExtractorImpl::RemoveElementsFromQueue(const std::string &topic_name, const size_t nr_of_elements_to_remove) {
  ProfileUdex_OV;

  if (udex_topic_queue_mp_.find(topic_name) != udex_topic_queue_mp_.end()) {
    // only clear queue for topics that were pushed at least once
    udex_topic_queue_mp_[topic_name]->RemoveElements(nr_of_elements_to_remove);
  }
}

void DataExtractorImpl::ClearExtractedQueue(const std::string &topic_name) {
  ProfileUdex_OV;

  if (udex_topic_queue_mp_.find(topic_name) != udex_topic_queue_mp_.end()) {
    // only clear queue for topics that were pushed at least once
    udex_topic_queue_mp_[topic_name]->ClearQueue();
  }
}

bool DataExtractorImpl::GetValue(const std::string &signal_url, const SignalType type, dataType &return_value,
                                 const SignalInfo *signal_info) {
  ProfileUdex_FLL;
  auto topic = signal_explorer_->GetPackageUrl(signal_url);
  if (topic.size() == 0) {
    warn(__FILE__, "got empty reply from GetPackageUrl for url '{0}'", signal_url);
    return false;
  }

  size_t out;
  void *data = nullptr;

  if (!topic_buffer_.empty() && topic_buffer_.count(topic[0]) != 0) {
    // use saved buffer for topic
    data = topic_buffer_[topic[0]].data;
    out = topic_buffer_[topic[0]].size;
  } else {
    auto package = GetExtractedData(topic[0]);
    data = package.data;
    out = package.size;
  }

  SignalInfo local_signal_info;
  if (signal_info == NULL) { // user did not provide signal info, we get it
    local_signal_info = GetInfo(signal_url);
  } else { // use the info the user provided
    local_signal_info = *signal_info;
  }

  switch (type) {
  case next::udex::SignalType::SIGNAL_TYPE_BOOL: {
    // TODO if parsing fails, the return_value should be untouched (or 0)
    bool return_value_bool{false};
    bool res = ParseDataFromBinaryBlob(data, out, local_signal_info, return_value_bool);
    return_value = return_value_bool;
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_CHAR: {
    char return_value_char = 0;
    bool res = ParseDataFromBinaryBlob(data, out, local_signal_info, return_value_char);
    return_value = return_value_char;
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_UINT8: {
    uint8_t return_value_uint8{0u};
    bool res = ParseDataFromBinaryBlob(data, out, local_signal_info, return_value_uint8);
    return_value = return_value_uint8;
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_INT8: {
    int8_t return_value_int8{0};
    bool res = ParseDataFromBinaryBlob(data, out, local_signal_info, return_value_int8);
    return_value = return_value_int8;
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_UINT16: {
    uint16_t return_value_uint16{0u};
    bool res = ParseDataFromBinaryBlob(data, out, local_signal_info, return_value_uint16);
    return_value = return_value_uint16;
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_INT16: {
    int16_t return_value_int16{0};
    bool res = ParseDataFromBinaryBlob(data, out, local_signal_info, return_value_int16);
    return_value = return_value_int16;
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_UINT32: {
    uint32_t return_value_uint32{0u};
    bool res = ParseDataFromBinaryBlob(data, out, local_signal_info, return_value_uint32);
    return_value = return_value_uint32;
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_INT32: {
    int32_t return_value_int32{0};
    bool res = ParseDataFromBinaryBlob(data, out, local_signal_info, return_value_int32);
    return_value = return_value_int32;
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_UINT64: {
    uint64_t return_value_uint64{0u};
    bool res = ParseDataFromBinaryBlob(data, out, local_signal_info, return_value_uint64);
    return_value = return_value_uint64;
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_INT64: {
    int64_t return_value_int64{0};
    bool res = ParseDataFromBinaryBlob(data, out, local_signal_info, return_value_int64);
    return_value = return_value_int64;
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_FLOAT: {
    float return_value_float{0.f};
    bool res = ParseDataFromBinaryBlob(data, out, local_signal_info, return_value_float);
    return_value = return_value_float;
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_DOUBLE: {
    double return_value_double{0.};
    bool res = ParseDataFromBinaryBlob(data, out, local_signal_info, return_value_double);
    return_value = return_value_double;
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_STRUCT: {
    warn(__FILE__, " Signal type struct is not supported yet.");
    return false;
  }
  }

  return false;
}

template <typename T>
void convert(std::vector<dataType> &return_type, std::vector<T> &to_convert) {
  return_type.clear();
  for (auto elem : to_convert) {
    return_type.push_back(elem);
  }
}

bool DataExtractorImpl::GetVectorValue(const std::string &signal_url, const SignalType type,
                                       std::vector<dataType> &return_vector, const SignalInfo *signal_info) {
  ProfileUdex_FLL;
  auto topic = signal_explorer_->GetPackageUrl(signal_url);
  if (topic.size() == 0) {
    warn(__FILE__, "got empty reply from GetPackageUrl for url '{0}'", signal_url);
    return false;
  }

  size_t out;
  void *data = nullptr;

  if (!topic_buffer_.empty() && topic_buffer_.count(topic[0]) != 0) {
    // use saved buffer for topic
    data = topic_buffer_[topic[0]].data;
    out = topic_buffer_[topic[0]].size;
  } else {
    auto package = GetExtractedData(topic[0]);
    data = package.data;
    out = package.size;
  }

  SignalInfo local_signal_info;
  if (signal_info == NULL) { // user did not provide signal info, we get it
    local_signal_info = GetInfo(signal_url);
  } else { // use the info the user provided
    local_signal_info = *signal_info;
  }

  switch (type) {
  case next::udex::SignalType::SIGNAL_TYPE_BOOL: {
    std::vector<bool> return_value_bool;
    bool res = ParseDataFromBinaryBlob(data, out, local_signal_info, return_value_bool);
    convert(return_vector, return_value_bool);
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_CHAR: {
    std::vector<char> return_value_char;
    bool res = ParseDataFromBinaryBlob(data, out, local_signal_info, return_value_char);
    convert(return_vector, return_value_char);
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_UINT8: {
    std::vector<uint8_t> return_value_uint8;
    bool res = ParseDataFromBinaryBlob(data, out, local_signal_info, return_value_uint8);
    convert(return_vector, return_value_uint8);
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_INT8: {
    std::vector<int8_t> return_value_int8;
    bool res = ParseDataFromBinaryBlob(data, out, local_signal_info, return_value_int8);
    convert(return_vector, return_value_int8);
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_UINT16: {
    std::vector<uint16_t> return_value_uint16;
    bool res = ParseDataFromBinaryBlob(data, out, local_signal_info, return_value_uint16);
    convert(return_vector, return_value_uint16);
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_INT16: {
    std::vector<int16_t> return_value_int16;
    bool res = ParseDataFromBinaryBlob(data, out, local_signal_info, return_value_int16);
    convert(return_vector, return_value_int16);
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_UINT32: {
    std::vector<uint32_t> return_value_uint32;
    bool res = ParseDataFromBinaryBlob(data, out, local_signal_info, return_value_uint32);
    convert(return_vector, return_value_uint32);
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_INT32: {
    std::vector<int32_t> return_value_int32;
    bool res = ParseDataFromBinaryBlob(data, out, local_signal_info, return_value_int32);
    convert(return_vector, return_value_int32);
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_UINT64: {
    std::vector<uint64_t> return_value_uint64;
    bool res = ParseDataFromBinaryBlob(data, out, local_signal_info, return_value_uint64);
    convert(return_vector, return_value_uint64);
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_INT64: {
    std::vector<int64_t> return_value_int64;
    bool res = ParseDataFromBinaryBlob(data, out, local_signal_info, return_value_int64);
    convert(return_vector, return_value_int64);
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_FLOAT: {
    std::vector<float> return_value_float;
    bool res = ParseDataFromBinaryBlob(data, out, local_signal_info, return_value_float);
    convert(return_vector, return_value_float);
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_DOUBLE: {
    std::vector<double> return_value_double;
    bool res = ParseDataFromBinaryBlob(data, out, local_signal_info, return_value_double);
    convert(return_vector, return_value_double);
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_STRUCT: {
    warn(__FILE__, " Signal type struct is not supported yet.");
    return false;
  }
  }

  return false;
}

bool DataExtractorImpl::GetValueAndInfo(const std::string &signal_url, SignalInfo &signal_info,
                                        std::vector<dataType> &return_vector) {
  ProfileUdex_FLL;
  auto topic = signal_explorer_->GetPackageUrl(signal_url);
  if (topic.size() == 0) {
    warn(__FILE__, "got empty reply from GetPackageUrl for url '{0}'", signal_url);
    return false;
  }

  size_t out;
  void *data = nullptr;

  if (!topic_buffer_.empty() && topic_buffer_.count(topic[0]) != 0) {
    // use saved buffer for topic
    data = topic_buffer_[topic[0]].data;
    out = topic_buffer_[topic[0]].size;
  } else {
    auto package = GetExtractedData(topic[0]);
    data = package.data;
    out = package.size;
  }

  signal_info = GetInfo(signal_url);

  switch (signal_info.signal_type) {
  case next::udex::SignalType::SIGNAL_TYPE_BOOL: {
    std::vector<bool> return_value_bool;
    bool res = ParseDataFromBinaryBlob(data, out, signal_info, return_value_bool);
    convert(return_vector, return_value_bool);
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_CHAR: {
    std::vector<char> return_value_char;
    bool res = ParseDataFromBinaryBlob(data, out, signal_info, return_value_char);
    convert(return_vector, return_value_char);
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_UINT8: {
    std::vector<uint8_t> return_value_uint8;
    bool res = ParseDataFromBinaryBlob(data, out, signal_info, return_value_uint8);
    convert(return_vector, return_value_uint8);
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_INT8: {
    std::vector<int8_t> return_value_int8;
    bool res = ParseDataFromBinaryBlob(data, out, signal_info, return_value_int8);
    convert(return_vector, return_value_int8);
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_UINT16: {
    std::vector<uint16_t> return_value_uint16;
    bool res = ParseDataFromBinaryBlob(data, out, signal_info, return_value_uint16);
    convert(return_vector, return_value_uint16);
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_INT16: {
    std::vector<int16_t> return_value_int16;
    bool res = ParseDataFromBinaryBlob(data, out, signal_info, return_value_int16);
    convert(return_vector, return_value_int16);
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_UINT32: {
    std::vector<uint32_t> return_value_uint32;
    bool res = ParseDataFromBinaryBlob(data, out, signal_info, return_value_uint32);
    convert(return_vector, return_value_uint32);
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_INT32: {
    std::vector<int32_t> return_value_int32;
    bool res = ParseDataFromBinaryBlob(data, out, signal_info, return_value_int32);
    convert(return_vector, return_value_int32);
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_UINT64: {
    std::vector<uint64_t> return_value_uint64;
    bool res = ParseDataFromBinaryBlob(data, out, signal_info, return_value_uint64);
    convert(return_vector, return_value_uint64);
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_INT64: {
    std::vector<int64_t> return_value_int64;
    bool res = ParseDataFromBinaryBlob(data, out, signal_info, return_value_int64);
    convert(return_vector, return_value_int64);
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_FLOAT: {
    std::vector<float> return_value_float;
    bool res = ParseDataFromBinaryBlob(data, out, signal_info, return_value_float);
    convert(return_vector, return_value_float);
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_DOUBLE: {
    std::vector<double> return_value_double;
    bool res = ParseDataFromBinaryBlob(data, out, signal_info, return_value_double);
    convert(return_vector, return_value_double);
    return res;
  }

  case next::udex::SignalType::SIGNAL_TYPE_STRUCT: {
    warn(__FILE__, " Signal type struct is not supported yet.");
    return false;
  }
  }

  return false;
}

SignalInfo DataExtractorImpl::GetInfo(const std::string &signal_url) {
  ProfileUdex_FLL;

  auto find_info = signal_info_cache_.find(signal_url);
  if (find_info != signal_info_cache_.end()) {
    return find_info->second;
  }

  auto topic = signal_explorer_->GetPackageUrl(signal_url);
  if (topic.size() == 0) {
    debug(__FILE__, "got empty reply from GetPackageUrl for url '{0}'", signal_url);
    return {};
  }

  bool is_available = false;
  auto signal_description = signal_explorer_->getSignalDescription(topic[0], is_available);
  std::string schema(signal_description.binary_data_description.begin(),
                     signal_description.binary_data_description.end());

  Signal signal;
  SignalInfo signal_info;

  if (core_lib_parser_.GetSignalInfo(topic[0], signal_url, schema, signal_description.basic_info.format_type, signal)) {
    signal_info.array_size = signal.data_[signal_url].array_length_;
    signal_info.signal_size = signal.data_[signal_url].payload_size_;
    signal_info.signal_type = signal.data_[signal_url].sig_type_;
    signal_info.offset = signal.data_[signal_url].start_offset_;

    signal_info_cache_[signal_url] = signal_info;
  }

  return signal_info;
}

unsigned short DataExtractorImpl::GetExtractionQueueMaxSize() { return extraction_queue_max_size_; }

void DataExtractorImpl::SetExtractionVersion(const std::string &port_name, const size_t port_size,
                                             const std::string &version, const std::string &data_type_name) {
  SetExtractionVersion(port_name, port_size, ConvertVersion(version), data_type_name);
}

void DataExtractorImpl::SetExtractionVersion(const std::string &port_name, const size_t port_size,
                                             const uint32_t version, const std::string &data_type_name) {
  StructInfo struct_info{port_name, port_size, version, data_type_name};

  struct_info_map_.emplace(std::make_pair(port_name, struct_info));

  if (udex_topic_queue_mp_.find(port_name) != udex_topic_queue_mp_.end()) {
    // if the queue already exists, forward to queue
    udex_topic_queue_mp_[port_name]->setStructInfo(&struct_info_map_.at(port_name));
  }
}

uint32_t DataExtractorImpl::ConvertVersion(const std::string &version) {
  const auto vv = next::sdk::string::split(version, ".");

  if (vv.size() != 3 && vv.size() != 4) {
    throw std::invalid_argument("the given version string is not of format major.minor.patch[.bugfix]");
  }

  // BugFix byte is actually never used and it ignored, all the other bytes are actually shifted to the right
  // and patch is where the Bufgix should be. Resulting version bytes: 0 major minor patch
  if (std::stoul(vv[0]) > 255 || std::stoul(vv[1]) > 255 || std::stoul(vv[2]) > 255) {
    warn(__FILE__, "Version byte > 255, version will be truncated");
  }

  uint8_t major = static_cast<uint8_t>(std::stoul(vv[0]));
  uint8_t minor = static_cast<uint8_t>(std::stoul(vv[1]));
  uint8_t patch = static_cast<uint8_t>(std::stoul(vv[2]));
  uint32_t vers = (uint32_t)((major << 16) + (minor << 8) + patch);

  return vers;
}

TimestampedDataPacket DataExtractorImpl::InsertEmptyDataPacket(const std::string &topic_name, const std::size_t size) {
  ProfileUdex_OV;
  UdexDataPackage tmp;
  void *buff_data = malloc(size);
  tmp.Data = buff_data;
  if (tmp.Data == nullptr) {
    error(__FILE__, "Could not allocate memory. Could not insert empty data packet inside queue for topic: {0}",
          topic_name);
    return TimestampedDataPacket{};
  }
  memset(buff_data, 0, size);
  tmp.info.format_type = next::sdk::types::PackageFormatType::PACKAGE_FORMAT_TYPE_UNKNOWN;
  tmp.info.Time = 0;
  tmp.Size = size;

  PushData(topic_name, &tmp);

  free(buff_data);

  // TODO-SIMEN-9755 make sure queue Pull is blocked
  std::list<std::shared_ptr<TimestampedDataPacket>> queue = udex_topic_queue_mp_[topic_name]->PullQueue();

  if (queue.size() > 0) {
    return *(queue.back());
  }
  // TODO: SIMEN-9755 make sure queue is set up when a empty packages needs to be inserted
  error(__FILE__, "Could not insert empty data packet inside queue for topic: {0}", topic_name);
  return TimestampedDataPacket{nullptr, 0, 0};
}

} // namespace extractor
} // namespace udex
} // namespace next

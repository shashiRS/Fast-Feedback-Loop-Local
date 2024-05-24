/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */

#include "udex_queue.h"

#include <next/sdk/logger/logger.hpp>

namespace next {
namespace udex {

std::string adapter_dll_ = "";

UDex_Queue::UDex_Queue(IConverter *converter, const std::string &schema, const PackageInfo &package_info,
                       const SignalInfo &signal_info, bool blocking_flag, bool drop_extra_input)
    : converter_(converter), schema_(schema), package_info_(package_info), signal_info_(signal_info),
      blocking_flag_(blocking_flag), drop_extra_input_(drop_extra_input) {
  ProfileUdex_OV;
  raw_input_queue_ptr_ = std::make_shared<std::list<std::shared_ptr<Data>>>();

  SetTerminate(false);
  std::thread queue_thred(&UDex_Queue::exec, this);
  queue_exec_thread_ = std::move(queue_thred);
}

UDex_Queue ::~UDex_Queue() {
  SetTerminate(true);
  NotifyAndSetProcessingRequested(true);
  NotifyOutputQueue();

  // wait for the thread to end working:
  queue_exec_thread_.join();

  // clear the buffering queue:
  {
    std::lock_guard<LockableBase(std::mutex)> lock(input_queue_mtx_);
    if (raw_input_queue_ptr_.get() != nullptr) {
      while (!raw_input_queue_ptr_->empty()) {
        auto entryToRemove = raw_input_queue_ptr_->front();
        raw_input_queue_ptr_->pop_front();
        free(entryToRemove->data_);
        entryToRemove.reset();
      }
    }
    raw_input_queue_ptr_.reset();
  }

  // clear the processing queue:
  {
    std::lock_guard<LockableBase(std::mutex)> lock(processing_queue_mtx_);
    while (!extracted_output_queue_.empty()) {
      auto entryToRemove = extracted_output_queue_.front();
      extracted_output_queue_.pop_front();

      free(entryToRemove->data);
      entryToRemove.reset();
    }
    extracted_output_queue_.clear();
  }

  {
    std::lock_guard<LockableBase(std::mutex)> lock(id_to_callback_mtx_);
    id_to_callback_url_map_.clear();
  }
  debug(__FILE__, "Dtor finished");
}

UDex_Queue::Data::Data(std::string topic, void *data, size_t dataSize, size_t time)
    : topic_(topic), data_(data), dataSize_(dataSize), Time(time) {
  ProfileUdex_FLL;
}

void UDex_Queue::SetTerminate(bool terminate) {
  std::lock_guard<LockableBase(std::mutex)> lk(input_queue_blocker_.mutex);
  terminating_ = terminate;
  terminating_cv_check_ = terminate;
}

void UDex_Queue::NotifyAndSetProcessingRequested(bool proc_requested) {
  {
    std::lock_guard<LockableBase(std::mutex)> lk(input_queue_blocker_.mutex);
    processing_requested_ = proc_requested;
    processing_requested_cv_check_ = proc_requested;
    input_queue_blocker_.cv.notify_all();
  }
  if (proc_requested) {
    std::lock_guard<LockableBase(std::mutex)> cv_bool_lock(pull_queue_blocker_.mutex);
    pull_queue_blocker_.spurious_trigger_safeguard_ = false;
  }
}

void UDex_Queue::run_gia(void *&data, size_t &size) {
  ProfileUdexN_DTL("next::udex::UDex_Queue::ConvertUdexPackage converter_");
  // GIA
  if (converter_) { // if we have a converter, this is ECU data that can be adapted
    auto converter_type_name = converter_->GetTypeName();

    uint32_t version = 0;
    memcpy(&version, data, 4);

    // input
    CVersionInfoImpl version_in(version);
    unsigned long size_in = (unsigned long)size; // size of the previous extracted data

    if (struct_info_) {
      // output
      CVersionInfoImpl version_out(struct_info_->version);
      unsigned long size_out = (unsigned long)struct_info_->url_size;

      bool compatible = converter_->AreCompatible(simOPAQUE_t, size_out, version_out, simOPAQUE_t, size_in, &version_in,
                                                  converter_type_name);

      if (compatible) {
        void *adapted_data = nullptr;
        unsigned long adapted_size = 0;
        long converted = converter_->ConvertData(simOPAQUE_t, size_out, version_out, simOPAQUE_t, size_in, &version_in,
                                                 data, &adapted_data, adapted_size);
        // TODO: SIMEN-9755 @Calin please check for memory leaks
        if (converted == 0) // 0 means ok, others are error codes
        {
          data = adapted_data;
          size = adapted_size;
        }
      }
    }
  }
}

void UDex_Queue::prepare_extraction_payload(const next::udex::UdexDataPackage *UdexDataPackage, void *&input,
                                            size_t &size) {
  input = (void *)UdexDataPackage->Data;
  (void)input; // to avoid linux error: parameter �input� set but not used
  size = UdexDataPackage->Size;

  size_t binary_blob_size = UdexDataPackage->Size;
  size_t sdl_signal_size = signal_info_.signal_size;

  if (binary_blob_size > sdl_signal_size) {
    // data size adapting logic - will only apply if number of bytes is below threshold
    // the payload has been padded with extra bytes for some reason and the SDL file does not contain those extra bytes
    // the padding must be removed
    // if the size difference ( blob - SDL) > defined_value, nothing happens - image usecase
    // if the size difference ( blob - SDL) < defined_value, cut off extra bytes from binary blob - padding usecase
    if (binary_blob_size - sdl_signal_size < signal_sdl_size_diff_threshold_) {
      // cut off (don't use) extra bytes from binary blob
      size = sdl_signal_size;
    }
  }
}

void UDex_Queue::ConvertUdexPackage(const std::string &topic, const next::udex::UdexDataPackage *raw_package) {
  ProfileUdex_DTL;
  size_t outSize;
  void *payload;
  ConvertRawPackageWithCoreLibGia(topic, raw_package, outSize, payload);

  // lock the processing queue while pushing/modifying it:
  {
    std::lock_guard<LockableBase(std::mutex)> lock(processing_queue_mtx_);
    if (extracted_output_queue_.size() >= getQueueMaxSize()) {
      RemoveFrontFromExtractedQueue();
    }
    pushDataToExtractedQueue(outSize, payload, raw_package->info.Time);
  }
  return;
}

void UDex_Queue::ConvertRawPackageWithCoreLibGia(const std::string &topic, const UdexDataPackage *raw_package,
                                                 size_t &outSize, void *&payload) {
  void *extraction_input = nullptr;
  size_t extraction_input_size = 0;

  prepare_extraction_payload(raw_package, extraction_input, extraction_input_size);
  memory_pointer extracted_data = std::make_unique<std::vector<uint8_t>>();
  // process the data:

  core_lib_converter_.ConvertData(topic, extraction_input, extraction_input_size, raw_package->info.Time, schema_,
                                  package_info_, extracted_data);
  void *raw_extracted_data = extracted_data->data();
  outSize = extracted_data->size();

  run_gia(raw_extracted_data, outSize);

  payload = malloc(outSize);
  {
    ProfileUdexN_DTL("next::udex::UDex_Queue::ConvertUdexPackage malloc/memcpy");
    memcpy(payload, raw_extracted_data, outSize);
  }
}

void UDex_Queue::pushDataToExtractedQueue(size_t outSize, void *payload, size_t timestamp) {
  try {
    // push the new processed data to the queue:
    TimestampedDataPacket insert_data;
    insert_data.data = payload;
    insert_data.size = outSize;
    insert_data.timestamp = timestamp;
    extracted_output_queue_.push_back(std::make_shared<TimestampedDataPacket>(insert_data));
  } catch (const std::bad_alloc &e) {
    sdk::logger::error(__FILE__, "Allocation failed: {0}", e.what());

    free(payload);
  }
}

void UDex_Queue::RemoveFrontFromExtractedQueue() {
  std::shared_ptr<TimestampedDataPacket> tempNodeToRemovePtr(nullptr);
  // we reached to the max size, remove the oldest value from the front before pushing the new one to the back:
  tempNodeToRemovePtr = extracted_output_queue_.front();
  extracted_output_queue_.pop_front();
  if (nullptr != tempNodeToRemovePtr) {
    // we have a ptr to an entry to free
    void *extractedDataToRemove = tempNodeToRemovePtr->data;
    // free the allocation for the extracted data:
    free(extractedDataToRemove);
  }
}

void UDex_Queue::CallUpCallbacks() {
  // call callbacks
  if (terminating_ == true) {
    return;
  }
  std::lock_guard locker(id_to_callback_mtx_);
  {
    ProfileUdexN_DTL("next::udex::UDex_Queue::ConvertUdexPackage for loop callback");
    for (auto id_to_callback_url_pair : id_to_callback_url_map_) {
      // auto id = id_to_callback_url_pair.first;
      auto callback_url_pair = id_to_callback_url_pair.second;
      {
        ProfileUdexN_DTL("next::udex::UDex_Queue::ConvertUdexPackage processing callback");
        UdexCallbackT &cb = callback_url_pair.first;
        std::string signal_url = callback_url_pair.second;
        SetProcessingThreadInfo();
        cb(signal_url);
        callback_active_ = false;
      }
    }
  }
}

void UDex_Queue::SetProcessingThreadInfo() {
  processing_thread_id_ = std::this_thread::get_id();
  callback_active_ = true;
}

void UDex_Queue::SetBlockingFlag(const bool blocking_flag) { blocking_flag_ = blocking_flag; }

void UDex_Queue::SetDropExtraInput(const bool drop_input_flag) { drop_extra_input_ = drop_input_flag; }

void UDex_Queue::save_last_raw_package(const next::udex::UdexDataPackage *UdexDataPackage) {
  // in case topic is variable size, adapt to the bigger topic size
  if (UdexDataPackage->Size > saved_raw_package_.Size) {
    saved_raw_package_.Size = UdexDataPackage->Size;
    if (saved_raw_package_.Data != nullptr) {
      free(const_cast<void *>(saved_raw_package_.Data));
    }

    saved_raw_package_.Data = malloc(saved_raw_package_.Size);
    if (nullptr == saved_raw_package_.Data) {
      // error while allocating memory of the inSize.
      return;
    }
  }
  memcpy(const_cast<void *>(saved_raw_package_.Data), UdexDataPackage->Data, UdexDataPackage->Size);
  saved_raw_package_.info.format_type = UdexDataPackage->info.format_type;
  saved_raw_package_.info.Time = UdexDataPackage->info.Time;
  saved_raw_package_.info.PackageUrl = UdexDataPackage->info.PackageUrl;
}

bool UDex_Queue::Push(const std::string &topic, const next::udex::UdexDataPackage *UdexDataPackage) {
  if (terminating_) {
    // the class is terminating...
    return false;
  }

  SetCurrentlyPushingInput(true);
  save_last_raw_package(UdexDataPackage);

#ifdef BUILD_TESTING
  std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms_push));
  std::cout << __FILE__ << " should only be visible while unit testing" << std::endl;
#endif

  if (blocking_flag_) { // just call separate function for now
    // maybe it can be improved by pushing to raw_input_queue, notify and wait until it's processed
    std::lock_guard locker(blocking_mtx);
    ConvertUdexPackage(topic, UdexDataPackage);
    SetCurrentlyPushingInput(false);
    NotifyAndSetProcessingRequested(false);
    NotifyOutputQueue();
    CallUpCallbacks();
    return true;
  }

  PutDataToInputQueue(topic, UdexDataPackage);
  SetCurrentlyPushingInput(false);
  return true;
}

void UDex_Queue::SetCurrentlyPushingInput(bool currently_input_push) {
  currently_pushing_input_ = currently_input_push;
  {
    std::lock_guard<std::mutex> lock_currently_pushing_input(pull_queue_blocker_.mutex);
    currently_pushing_input_cv_check_ = currently_input_push;
  }
}

bool UDex_Queue::PutDataToInputQueue(const std::string &topic, const UdexDataPackage *UdexDataPackage) {
  void *copiedData = malloc(UdexDataPackage->Size);
  if (nullptr == copiedData) {
    // error while allocating memory of the inSize.
    return false;
  }
  memcpy(copiedData, UdexDataPackage->Data, UdexDataPackage->Size);
  try {
    std::shared_ptr<UDex_Queue::Data> data =
        std::make_shared<UDex_Queue::Data>(topic, copiedData, UdexDataPackage->Size, UdexDataPackage->info.Time);
    {
      std::lock_guard<std::mutex> protect_input(input_queue_mtx_);
      raw_input_queue_ptr_->push_back(data);

      // notify the working thread that new data is received.
      NotifyAndSetProcessingRequested(true);
    }
  } catch (const std::bad_alloc &e) {
    sdk::logger::error(__FILE__, "Allocation failed: {0}", e.what());
    free(copiedData);
    return false;
  }
  return true;
}

UdexDataPackage UDex_Queue::GetRawData() { return saved_raw_package_; }

bool UDex_Queue::isDataAvailable() {
  waitForProcessing();
  if (extracted_output_queue_.empty()) {
    return false;
  }
  return true;
}

void UDex_Queue::waitForProcessing() {

  if (terminating_) {
    return;
  }
  // special treatment for callback in execution loop.
  // This enables also to check Data in a callback on the exec thread
  if (CheckOnProcessingThread()) {
    return;
  }
  // see if processing was requested or input is onging
  // ! -> we need to wait until the mutex is grabbed by exec() !
  if (processing_requested_ || currently_pushing_input_) {
    // now wait
    std::unique_lock<LockableBase(std::mutex)> lock(pull_queue_blocker_.mutex);
    // wait if 1) pull_queue is locked (no processing started), 2) processing is requested 3) input is pushed
    pull_queue_blocker_.cv.wait(lock, [&]() {
      return pull_queue_blocker_.spurious_trigger_safeguard_ && !processing_requested_cv_check_ &&
             !currently_pushing_input_cv_check_;
    });
  }
  // mutex is grabed by exec() if needed now
  std::lock_guard<LockableBase(std::mutex)> lock_processing(processing_ongoing_mtx);
}

bool UDex_Queue::CheckOnProcessingThread() {
  if (callback_active_) {
    if (processing_thread_id_ == std::this_thread::get_id()) {
      return true;
    }
  }
  return false;
}

std::list<std::shared_ptr<TimestampedDataPacket>> UDex_Queue::PullQueue() {

  waitForProcessing();
  std::lock_guard<LockableBase(std::mutex)> lock(processing_queue_mtx_);
  if (extracted_output_queue_.empty()) {
    // no data available yet.
    return std::list<std::shared_ptr<TimestampedDataPacket>>();
  }

  provided_elements_ = extracted_output_queue_.size();

  return extracted_output_queue_;
}

void UDex_Queue::RemoveElements(const size_t nr_of_elements_to_remove) {
  ProfileUdex_DTL;
  std::lock_guard<LockableBase(std::mutex)> lock(processing_queue_mtx_);

  for (size_t cnt = 0; cnt < nr_of_elements_to_remove; cnt++) {
    if (!extracted_output_queue_.empty()) {
      auto entryToRemove = extracted_output_queue_.front();
      extracted_output_queue_.pop_front();

      free(entryToRemove->data);
      entryToRemove.reset();
    }
  }
}

// pop based on array<index> - prio 2

void UDex_Queue::ClearQueue() {
  ProfileUdex_DTL;
  std::lock_guard<LockableBase(std::mutex)> lock(processing_queue_mtx_);

  for (size_t cnt = 0; cnt < provided_elements_; cnt++) {
    if (!extracted_output_queue_.empty()) {
      auto entryToRemove = extracted_output_queue_.front();
      extracted_output_queue_.pop_front();

      free(entryToRemove->data);
      entryToRemove.reset();
    }
  }
}

void UDex_Queue::exec() {

  while (terminating_.load() == false) {
    // wait here until until being notified about new data received or pull is called.

#ifdef BUILD_TESTING
    std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms_exec));
    std::cout << __FILE__ << " should only be visible while unit testing" << std::endl;
#endif

    bool no_new_data_available;
    {
      std::lock_guard<std::mutex> protect_input(input_queue_mtx_);
      no_new_data_available = raw_input_queue_ptr_ == nullptr || raw_input_queue_ptr_->empty();
    }
    if (no_new_data_available) {
      NotifyOutputQueue(); //< no new data avialbe. Trigger OutputQueue
      std::unique_lock<LockableBase(std::mutex)> cv_lock(input_queue_blocker_.mutex);
      input_queue_blocker_.cv.wait(cv_lock, [this] { return terminating_cv_check_ || processing_requested_cv_check_; });
      continue;
    }
    // grab the mutex before giving access in waitForProcessing()
    std::lock_guard<LockableBase(std::mutex)> lock(processing_ongoing_mtx);

    // read out Queue quickly to make sure input is not blocked for long
    std::shared_ptr<std::list<std::shared_ptr<UDex_Queue::Data>>> temp_queue_ptr = ReadInputQueue();

    // no data received:
    if (nullptr == temp_queue_ptr) {
      continue;
    }

    // start processing the new data:
    while (!temp_queue_ptr->empty()) {
      auto data = temp_queue_ptr->front();
      temp_queue_ptr->pop_front();
      auto time_converted = data->Time;
      UdexDataPackage data_package = {
          data->data_,
          data->dataSize_,
          {time_converted, package_info_.format_type, package_info_.processor_required, data->topic_.c_str()}};
      ConvertUdexPackage(data->topic_, &data_package);
      CallUpCallbacks();
      free(data->data_);
    }
  }
}

void UDex_Queue::NotifyOutputQueue() {
  {
    std::lock_guard<LockableBase(std::mutex)> cv_bool_lock(pull_queue_blocker_.mutex);
    pull_queue_blocker_.spurious_trigger_safeguard_ = true;
    pull_queue_blocker_.cv.notify_all();
  }
}

std::shared_ptr<std::list<std::shared_ptr<UDex_Queue::Data>>> UDex_Queue::ReadInputQueue() {
  std::lock_guard<std::mutex> protect_input(input_queue_mtx_);
  std::shared_ptr<std::list<std::shared_ptr<Data>>> temp_queue_ptr(nullptr);
  try {
    if (drop_extra_input_) { // only take the last element from the buffering input queue and drop the rest
      temp_queue_ptr = std::make_shared<std::list<std::shared_ptr<Data>>>();
      auto last = raw_input_queue_ptr_->back();
      temp_queue_ptr->push_back(last);
      raw_input_queue_ptr_->pop_back();
      for (auto it : *raw_input_queue_ptr_) {
        free(it->data_);
      }
      raw_input_queue_ptr_.reset();
    } else { // move the buffering queue to a temp queue, so that the queue's mutex is unlocked as soon as possible.
      temp_queue_ptr = std::move(raw_input_queue_ptr_);
    }

    // initialize again.
    raw_input_queue_ptr_ = std::make_shared<std::list<std::shared_ptr<Data>>>();
  } catch (const std::bad_alloc &e) {
    sdk::logger::error(__FILE__, "Allocation failed: {0}", e.what());
  }

  NotifyAndSetProcessingRequested(false);
  return temp_queue_ptr;
}

void UDex_Queue::setQueueMaxSize(unsigned short queueMaxSize) { queue_max_size_ = queueMaxSize; }

unsigned short UDex_Queue::getQueueMaxSize() { return queue_max_size_; }

void UDex_Queue::setQueueCallbacks(
    std::map<size_t, std::pair<next::udex::UdexCallbackT, std::string>> &id_to_callback_url_map) {
  ProfileUdex_FLL;
  std::lock_guard locker(id_to_callback_mtx_);
  id_to_callback_url_map_ = id_to_callback_url_map;
}

void UDex_Queue::setStructInfo(StructInfo *struct_info) { struct_info_ = struct_info; }

next::sdk::types::PackageFormatType UDex_Queue::getFormatType() { return package_info_.format_type; }

void UDex_Queue::setPackageInfo(const PackageInfo &package_info) { package_info_ = package_info; }

PackageInfo UDex_Queue::getPackageInfo() { return package_info_; }

void UDex_Queue::setSchema(const std::string &schema) { schema_ = schema; }

void UDex_Queue::setSizeDiffThreshold(const size_t diff) { signal_sdl_size_diff_threshold_ = diff; }

} // namespace udex
} // namespace next

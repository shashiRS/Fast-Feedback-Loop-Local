/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */

#ifndef UDEX_UDEX_QUEUE_H_
#define UDEX_UDEX_QUEUE_H_

#include <condition_variable>
#include <future>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>

#include <next/sdk/profiler/profile_tags.hpp>
#include <next/udex/data_types.hpp>

#include "../signal_storage/core_lib_wrap_data_converter.hpp"
#include "gia/gia_interfaces.h"
#include "gia/version_info.hpp"

namespace next {
namespace udex {

struct StructInfo {
  std::string url;
  size_t url_size;
  uint32_t version;
  std::string converter;
};

class UDex_Queue {
public:
  UDex_Queue(IConverter *converter, const std::string &schema, const PackageInfo &package_info,
             const SignalInfo &signal_info, bool blocking_flag, bool drop_extra_input);

  virtual ~UDex_Queue();

  struct Data {
    Data(std::string topic, void *data, size_t dataSize, size_t time);
    std::string topic_;
    void *data_;
    size_t dataSize_;
    size_t Time; //!< network time stamp -> read from ecal
  };

  void SetBlockingFlag(const bool blocking_flag);
  void SetDropExtraInput(const bool drop_input_flag);

  bool Push(const std::string &topic, const next::udex::UdexDataPackage *UdexDataPackage);

  UdexDataPackage GetRawData();

  bool isDataAvailable();

  std::list<std::shared_ptr<TimestampedDataPacket>> PullQueue();

  void RemoveElements(const size_t nr_of_elements_to_remove);

  void ClearQueue();

  void setQueueMaxSize(unsigned short queueMaxSize);

  unsigned short getQueueMaxSize();

  void setQueueCallbacks(std::map<size_t, std::pair<next::udex::UdexCallbackT, std::string>> &id_to_callback_url_map);

  void setStructInfo(StructInfo *struct_info);

  next::sdk::types::PackageFormatType getFormatType();

  void setPackageInfo(const PackageInfo &package_info);

  PackageInfo getPackageInfo();

  void setSchema(const std::string &desc);

  void setSizeDiffThreshold(const size_t diff);

private:
  void SetTerminate(bool terminate);
  void NotifyAndSetProcessingRequested(bool proc_requested);
  bool CheckOnProcessingThread();
  void SetProcessingThreadInfo();
  void CallUpCallbacks();
  bool PutDataToInputQueue(const std::string &topic, const UdexDataPackage *UdexDataPackage);
  void RemoveFrontFromExtractedQueue();
  void pushDataToExtractedQueue(size_t outSize, void *payload, size_t timestamp);
  void ConvertRawPackageWithCoreLibGia(const std::string &topic, const UdexDataPackage *raw_package, size_t &outSize,
                                       void *&payload);
  std::shared_ptr<std::list<std::shared_ptr<UDex_Queue::Data>>> ReadInputQueue();
  void waitForProcessing();
  void NotifyOutputQueue();
  void SetCurrentlyPushingInput(bool currently_input_push);

  struct ConditionalWait {
    TracyLockable(std::mutex, mutex);
    std::condition_variable cv;
    bool spurious_trigger_safeguard_ = false;
  };

  void exec();
  void ConvertUdexPackage(const std::string &topic, const next::udex::UdexDataPackage *raw_package);
  void save_last_raw_package(const next::udex::UdexDataPackage *UdexDataPackage);
  void run_gia(void *&extractedData, size_t &outSize);
  void prepare_extraction_payload(const next::udex::UdexDataPackage *UdexDataPackage, void *&input, size_t &size);

  // this is a shrd_ptr of the queue that buffers the new data temporarily
  std::shared_ptr<std::list<std::shared_ptr<UDex_Queue::Data>>> raw_input_queue_ptr_;
  // this queue stores the data that is already processed, along with the data size and timestamp
  std::list<std::shared_ptr<TimestampedDataPacket>> extracted_output_queue_;
  // this thread should work on the data to process it asynchronously.
  std::thread queue_exec_thread_;
  // this condition_variable should be used to notify the working thread when there is a new data received.

  ConditionalWait input_queue_blocker_;
  ConditionalWait pull_queue_blocker_;

  // this mutex should be used to protect the queue that stores the proccessed data.
  TracyLockable(std::mutex, processing_queue_mtx_);
  TracyLockable(std::mutex, input_queue_mtx_);
  TracyLockable(std::mutex, id_to_callback_mtx_);

  TracyLockable(std::mutex, processing_ongoing_mtx);
  TracyLockable(std::mutex, blocking_mtx);

  std::thread::id processing_thread_id_ = {};
  std::atomic<bool> callback_active_ = false;

  std::atomic<bool> terminating_ = false;

  bool terminating_cv_check_ = false;
  bool processing_requested_cv_check_ = false;
  bool currently_pushing_input_cv_check_ = false;

  // save the number of elements pulled, use this for clearing only what was provided already
  size_t provided_elements_ = 0;

  std::map<size_t, std::pair<next::udex::UdexCallbackT, std::string>> id_to_callback_url_map_;

  StructInfo *struct_info_ = nullptr;

  IConverter *converter_ = nullptr;

  std::string schema_ = "";

  PackageInfo package_info_{"", 0, 0, 0, next::sdk::types::PackageFormatType::PACKAGE_FORMAT_TYPE_UNKNOWN, false};

  SignalInfo signal_info_;

  size_t signal_sdl_size_diff_threshold_ = 0;

  unsigned short queue_max_size_ = 20;

  bool blocking_flag_ = false;

  CoreLibWrap::DataConverter core_lib_converter_;

  std::atomic<bool> processing_requested_ = false;
  std::atomic<bool> currently_pushing_input_ = false;
  std::atomic<bool> drop_extra_input_ = false;

  next::udex::UdexDataPackage saved_raw_package_{nullptr, 0, {}};

public:
  friend class UDexQueueTester;
  // ########################################## special treatment for tests
  // ##############################################
#ifdef BUILD_TESTING
  unsigned short sleep_ms_exec = 1;
  unsigned short sleep_ms_push = 1;
#endif
};

} // namespace udex
} // namespace next

#endif // UDEX_UDEX_DATA_EXTRACTOR_H_

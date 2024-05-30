/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */

#ifndef NEXT_UDEX_DATA_EXTRACTOR_IMPL_H
#define NEXT_UDEX_DATA_EXTRACTOR_IMPL_H

// rearrange include because including boost/asio.hpp will lead to WinSock.h already included error

#include <next/udex/data_types.hpp>
#include <next/udex/explorer/signal_explorer.hpp>
#include <next/udex/extractor/data_extractor.hpp>

#include "../signal_storage/core_lib_wrap_data_descriptor_parser.hpp"

#include "gia/gia_dll_handler.h"

#include "sync_algo_loader.h"
#include "udex_queue.h"

namespace next {
namespace udex {
namespace extractor {

class DataExtractorImpl {

public:
  DataExtractorImpl() = delete;

  DataExtractorImpl(const std::string &adapter_dll, unsigned short extractionQueueMaxSize);

  virtual ~DataExtractorImpl();

  // input functions
  void PushData(const std::string &topic_name, const next::udex::UdexDataPackage *data);

  // output functions
  UdexDataPackage GetRawData(const std::string &topic_name);

  bool isDataAvailable(const std::string &topic_name);

  TimestampedDataPacket GetExtractedData(const std::string &signal_url);

  std::list<std::shared_ptr<std::tuple<void *, size_t>>> GetExtractedQueue(const std::string &topic_name);
  std::list<std::shared_ptr<TimestampedDataPacket>> GetExtractedQueueWithTimestamp(const std::string &topic_name);
  void ClearExtractedQueue(const std::string &topic_name);
  void RemoveElementsFromQueue(const std::string &topic_name, const size_t nr_of_elements_to_remove);

  bool GetValue(const std::string &signal_url, const next::udex::SignalType type, dataType &return_value,
                const SignalInfo *signal_info);
  bool GetVectorValue(const std::string &signal_url, const next::udex::SignalType type,
                      std::vector<dataType> &return_vector, const SignalInfo *signal_info);

  bool GetValueAndInfo(const std::string &signal_url, next::udex::SignalInfo &signal_info,
                       std::vector<dataType> &return_vector);
  SignalInfo GetInfo(const std::string &signal_url);

  unsigned short GetExtractionQueueMaxSize();

  size_t connectCallback(next::udex::UdexCallbackT cb, std::string signal_url);
  void disconnectCallback(size_t id);

  std::map<size_t, CallbackInfo> getCallbacksInfo();

  void SetExtractionVersion(const std::string &port_name, const size_t port_size, const std::string &version,
                            const std::string &data_type_name);

  void SetExtractionVersion(const std::string &port_name, const size_t port_size, const uint32_t version,
                            const std::string &data_type_name);

  uint32_t ConvertVersion(const std::string &version);

  TimestampedDataPacket GetSyncExtractedData(const std::string &signal_url, const std::vector<SyncValue> &sync_values);
  void SetSyncAlgo(std::string signal_url, SyncAlgoCfg sync_cfg);

  //! TODO a mechanism for specific urls being not changed is needed. For now all specific values from
  //! SetBlockingExtractonForUrl are overriden
  void SetBlockingExtraction(const bool blocking_flag);

  //! sets blocking for specific URLs does not prevent forcefull override of SetBlockingExtractoin
  void SetBlockingExtractionForUrl(const std::string &url, const bool blocking_flag);

  // this sets default and for currently existing queues, should it affect existing queues?
  // it can be done topic default and url based similar to the blocking_flag
  void SetDropExtraInput(const bool drop_extra_input);

  void setSizeDiffThreshold(const size_t diff);
  void setDefaultSync(std::string signal_url);

  TimestampedDataPacket InsertEmptyDataPacket(const std::string &topic_name, const std::size_t size);

private:
  size_t callback_connection_id_ = 0;
  std::map<std::string, std::map<size_t, std::pair<next::udex::UdexCallbackT, std::string>>>
      map_callbacks_and_urls_to_topics_;
  //-------------------
  // std::map<topic_name,std::map<callback_connection_id, pair<CallbackFunction, signal_url>>>
  //
  // subscribe (topic1, extractor1) -> extractor1.connectCallback(cb, signalURL1)
  //                                   extractor1.connectCallback(cb, signalURL2)
  //
  // subscribe (topic2, extractor2) -> extractor2.connectCallback(cb1, signalURL)
  //                                   extractor2.connectCallback(cb2, signalURL)
  //
  // out of topic1 i am only interested in these specific URLs and will call the paired callback.
  // user can use same callback for all URLs and make a difference by signal_url
  // or call different callback functions for the same signal_url.

  // use a map of the topic and a shrd_ptr of UDex_Queue which is responsible about buffering/processing the data
  // the map maps each instance of UDex_Queue to each topic.
  std::map<std::string, std::shared_ptr<UDex_Queue>> udex_topic_queue_mp_;

  std::map<size_t, CallbackInfo> callbacks_info_;

  std::map<std::string, SignalInfo> signal_info_cache_;

  GiaDllHandler gia_dll_handler_;
  IAdapter *adapter_ptr_ = nullptr;

  std::map<std::string, StructInfo> struct_info_map_;

  std::map<std::string, bool> blocking_flag_map_;
  bool blocking_flag_ = false;
  bool drop_extra_input_ = false;

  std::unique_ptr<explorer::SignalExplorer> signal_explorer_ = std::make_unique<explorer::SignalExplorer>();

  CoreLibWrap::DataDescriptionParser core_lib_parser_;

  std::map<std::string, TimestampedDataPacket> topic_buffer_;

  std::map<std::string, std::shared_ptr<SyncAlgoLoader>> sync_algo_for_signal_;

  size_t signal_sdl_size_diff_threshold_ = 0;

  std::recursive_mutex block_extraction_queue_mutex_;

  unsigned short extraction_queue_max_size_;
};

} // namespace extractor
} // namespace udex
} // namespace next

#endif // NEXT_UDEX_DATA_EXTRACTOR_IMPL_H

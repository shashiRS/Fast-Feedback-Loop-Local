/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_signal_extractor_impl.hpp
 * @brief    implements the signal extractor for the plugin
 **/

#ifndef NEXT_BRIDGESDK_SIGNAL_EXTRACTOR_IMPL_H_
#define NEXT_BRIDGESDK_SIGNAL_EXTRACTOR_IMPL_H_

#include <next/bridgesdk/plugin_addons/plugin_signal_extractor.hpp>
#include <next/udex/data_types.hpp>

namespace next {
namespace bridgesdk {
namespace plugin_addons {

class PluginSignalExtractorImpl {

private:
  PluginSignalExtractorImpl() = delete;

public:
  PluginSignalExtractorImpl(next::bridgesdk::plugin::Plugin *base_plugin);
  virtual ~PluginSignalExtractorImpl() = default;

  next::bridgesdk::plugin::Plugin *base_plugin_;

  // implementation wrappers for all the functions in the extractor interface DataExtractor
  UdexDataPackage GetRawData(const std::string &topic_name);

  bool isDataAvailable(const std::string &topic_name);

  bool GetValue(const std::string &signal_url, const SignalType type, dataType &return_value, SignalInfo *signal_info);

  TypeConversionInfo GetValueWithTypeChecking(const std::string &signal_url, const SignalType type,
                                              dataType &return_value, SignalInfo *signal_info = nullptr);

  bool GetVectorValue(const std::string &signal_url, const SignalType type, std::vector<dataType> &return_vector,
                      SignalInfo *signal_info);

  bool GetValueAndInfo(const std::string &signal_url, SignalInfo &signal_info, std::vector<dataType> &return_vector);

  SignalInfo GetInfo(const std::string &signal_url);

  void ClearExtractedQueue(const std::string &topic_name);

  void *GetExtractedData(const std::string &topic_name, size_t &out_size, time_t &time);

  std::list<std::shared_ptr<next::bridgesdk::TimestampedDataPacket>> GetExtractedQueue(const std::string &topic_name);
};

} // namespace plugin_addons
} // namespace bridgesdk
} // namespace next

#endif // NEXT_BRIDGESDK_SIGNAL_EXTRACTOR_IMPL_H_

/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_signal_extractor_mock.hpp
 * @brief    utility addon to output data
 **/

#ifndef NEXT_BRIDGES_PLUGIN_SIGNALEXTRACTOR_MOCK_H
#define NEXT_BRIDGES_PLUGIN_SIGNALEXTRACTOR_MOCK_H

#include <next/udex/helper_data_extractor.h>

#include <next/bridgesdk/plugin_addons/plugin_signal_extractor.hpp>

namespace next {
namespace test {

class PluginSignalExtractorMock : public next::bridgesdk::plugin_addons::PluginSignalExtractor {

public:
  PluginSignalExtractorMock(next::bridgesdk::plugin::Plugin *plugin) : PluginSignalExtractor(plugin) {}
  virtual ~PluginSignalExtractorMock() = default;

  virtual next::bridgesdk::plugin_addons::UdexDataPackage GetRawData(const std::string &) override {
    return next::bridgesdk::plugin_addons::UdexDataPackage{};
  }

  virtual bool isDataAvailable(const std::string &) override { return false; }

  virtual void *GetExtractedData(const std::string &, size_t &, bridgesdk::time_t &) override { return nullptr; }

  virtual std::list<std::shared_ptr<next::bridgesdk::TimestampedDataPacket>>
  GetExtractedQueue(const std::string &) override {
    return {};
  }
  virtual void ClearExtractedQueue(const std::string &) override {}

  virtual next::bridgesdk::plugin_addons::TypeConversionInfo
  GetValueWithTypeChecking(const std::string &signal_url, const next::bridgesdk::plugin_addons::SignalType type,
                           next::bridgesdk::plugin_addons::dataType &return_value,
                           next::bridgesdk::plugin_addons::SignalInfo *signal_info = nullptr) override {
    GetValue(signal_url, type, return_value, signal_info);
    return next::bridgesdk::plugin_addons::TypeConversionInfo::FIT;
  }

  virtual bool GetValue(const std::string &signal_url, const next::bridgesdk::plugin_addons::SignalType type,
                        next::bridgesdk::plugin_addons::dataType &return_value,
                        next::bridgesdk::plugin_addons::SignalInfo * = nullptr) override {

    auto signal_info_converter = [](next::udex::SignalInfo &sig_info_udex,
                                    const next::bridgesdk::plugin_addons::SignalInfo &sig_info_bridge) {
      sig_info_udex.array_size = sig_info_bridge.array_size;
      sig_info_udex.offset = sig_info_bridge.offset;
      sig_info_udex.signal_size = sig_info_bridge.signal_size;
      sig_info_udex.signal_type = static_cast<next::udex::SignalType>(sig_info_bridge.signal_type);
    };

    switch (type) {
    case next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_BOOL: {
      bool return_value_bool{false};
      next::udex::SignalInfo info;
      signal_info_converter(info, sig_info_[signal_url]);
      bool res = next::udex::extractor::ParseDataFromBinaryBlob(message_.data, message_.size, info, return_value_bool);
      return_value = return_value_bool;
      return res;
    }

    case next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_CHAR: {
      char return_value_char{'\0'};
      next::udex::SignalInfo info;
      signal_info_converter(info, sig_info_[signal_url]);
      bool res = next::udex::extractor::ParseDataFromBinaryBlob(message_.data, message_.size, info, return_value_char);
      return_value = return_value_char;
      return res;
    }

    case next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT8: {
      uint8_t return_value_uint8{0u};
      next::udex::SignalInfo info;
      signal_info_converter(info, sig_info_[signal_url]);
      bool res = next::udex::extractor::ParseDataFromBinaryBlob(message_.data, message_.size, info, return_value_uint8);
      return_value = return_value_uint8;
      return res;
    }

    case next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT8: {
      int8_t return_value_int8{0};
      next::udex::SignalInfo info;
      signal_info_converter(info, sig_info_[signal_url]);
      bool res = next::udex::extractor::ParseDataFromBinaryBlob(message_.data, message_.size, info, return_value_int8);
      return_value = return_value_int8;
      return res;
    }

    case next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT16: {
      uint16_t return_value_uint16{0u};
      next::udex::SignalInfo info;
      signal_info_converter(info, sig_info_[signal_url]);
      bool res =
          next::udex::extractor::ParseDataFromBinaryBlob(message_.data, message_.size, info, return_value_uint16);
      return_value = return_value_uint16;
      return res;
    }

    case next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT16: {
      int16_t return_value_int16{0};
      next::udex::SignalInfo info;
      signal_info_converter(info, sig_info_[signal_url]);
      bool res = next::udex::extractor::ParseDataFromBinaryBlob(message_.data, message_.size, info, return_value_int16);
      return_value = return_value_int16;
      return res;
    }

    case next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT32: {
      uint32_t return_value_uint32{0u};
      next::udex::SignalInfo info;
      signal_info_converter(info, sig_info_[signal_url]);
      bool res =
          next::udex::extractor::ParseDataFromBinaryBlob(message_.data, message_.size, info, return_value_uint32);
      return_value = return_value_uint32;
      return res;
    }

    case next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT32: {
      int32_t return_value_uint32{0};
      next::udex::SignalInfo info;
      signal_info_converter(info, sig_info_[signal_url]);
      bool res =
          next::udex::extractor::ParseDataFromBinaryBlob(message_.data, message_.size, info, return_value_uint32);
      return_value = return_value_uint32;
      return res;
    }

    case next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT64: {
      uint64_t return_value_uint64{0u};
      next::udex::SignalInfo info;
      signal_info_converter(info, sig_info_[signal_url]);
      bool res =
          next::udex::extractor::ParseDataFromBinaryBlob(message_.data, message_.size, info, return_value_uint64);
      return_value = return_value_uint64;
      return res;
    }

    case next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT64: {
      int64_t return_value_int64{0};
      next::udex::SignalInfo info;
      signal_info_converter(info, sig_info_[signal_url]);
      bool res = next::udex::extractor::ParseDataFromBinaryBlob(message_.data, message_.size, info, return_value_int64);
      return_value = return_value_int64;
      return res;
    }

    case next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_FLOAT: {
      float return_value_float{0.f};
      next::udex::SignalInfo info;
      signal_info_converter(info, sig_info_[signal_url]);
      bool res = next::udex::extractor::ParseDataFromBinaryBlob(message_.data, message_.size, info, return_value_float);
      return_value = return_value_float;
      return res;
    }

    case next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_DOUBLE: {
      double return_value_double{0.};
      next::udex::SignalInfo info;
      signal_info_converter(info, sig_info_[signal_url]);
      bool res =
          next::udex::extractor::ParseDataFromBinaryBlob(message_.data, message_.size, info, return_value_double);
      return_value = return_value_double;
      return res;
    }

    case next::bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_STRUCT: {
      // not implemented in the extractor, so no timplemented in the mock
      return false;
    }
    }

    return false;
  }

  virtual bool GetVectorValue(const std::string &, const next::bridgesdk::plugin_addons::SignalType,
                              std::vector<next::bridgesdk::plugin_addons::dataType> &,
                              next::bridgesdk::plugin_addons::SignalInfo * = nullptr) override {
    return false;
  }

  virtual bool GetValueAndInfo(const std::string &, next::bridgesdk::plugin_addons::SignalInfo &,
                               std::vector<next::bridgesdk::plugin_addons::dataType> &) override {
    return false;
  }

  virtual next::bridgesdk::plugin_addons::SignalInfo GetInfo(const std::string &signal_url) override {
    auto sig_inf = sig_info_.find(signal_url);
    if (sig_inf != std::end(sig_info_)) {
      return sig_info_[signal_url];
    }
    return {};
  }

  virtual float getValueAsFloat() { return 0.F; };

  void setSignalInfo(std::unordered_map<std::string, next::bridgesdk::plugin_addons::SignalInfo> sig_info) {
    sig_info_ = sig_info;
  }

  void updatePayload(const std::vector<uint8_t> &payload) {
    payload_ = payload;
    message_.channel_name = "test";
    message_.data = reinterpret_cast<const void *>(payload.data());
    message_.size = payload_.size();
    message_.time = 10;
  }

private:
  std::unordered_map<std::string, next::bridgesdk::plugin_addons::SignalInfo> sig_info_{};
  next::bridgesdk::ChannelMessagePointers message_{};
  std::vector<uint8_t> payload_{};
}; // PluginSignalExtractorMock

} // namespace test
} // namespace next

#endif // NEXT_BRIDGES_PLUGIN_SIGNALEXTRACTOR_MOCK_H

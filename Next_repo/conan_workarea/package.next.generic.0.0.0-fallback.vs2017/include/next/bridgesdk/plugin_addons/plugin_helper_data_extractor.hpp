/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_helper_data_extractor.hpp
 * @brief    small functionality set to be used to extract data. \todo Transfer to UDEX
 *
 **/

#ifndef NEXT_BRIDGESDK_PLUGIN_HELPER_DATA_EXTRACTOR_H_
#define NEXT_BRIDGESDK_PLUGIN_HELPER_DATA_EXTRACTOR_H_

#include <next/bridgesdk/datatypes.h>
#include <next/bridgesdk/plugin.h>
#include "plugin_signal_extractor.hpp"

#include <cstdint>
#include <string>

namespace next {
namespace bridgesdk {
namespace plugin_addons {

/*\brief
 *
 * Test if Template from plugin user matches to signaltype with help of cpp_type analogon
 *
 */

template <typename T>
static inline bool simpleTypeConsistencyCheck(const std::string &cpp_type) {
  if (cpp_type.compare(typeid(T).name()) == 0) {
    return true;
  }
  return false;
}

template <typename T>
bool transformValueToTemplateType(const bridgesdk::plugin_addons::RawSignalInfo &info, const char *binary_pointer,
                                  T &return_value) {
  if (sizeof(T) < info.payload_size_) {
    return false;
  }
  // TODO check sign, return false if we would lose the sign

  const std::byte *ptr = reinterpret_cast<const std::byte *>(binary_pointer) + info.signal_offset;

  switch (info.sig_type_) {
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_CHAR:
    return_value = T(*reinterpret_cast<const char *>(ptr));
    break;
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT8:
    return_value = T(*reinterpret_cast<const uint8_t *>(ptr));
    break;
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT8:
    return_value = T(*reinterpret_cast<const int8_t *>(ptr));
    break;
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT16:
    return_value = T(*reinterpret_cast<const uint16_t *>(ptr));
    break;
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT16:
    return_value = T(*reinterpret_cast<const int16_t *>(ptr));
    break;
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT32:
    return_value = T(*reinterpret_cast<const uint32_t *>(ptr));
    break;
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT32:
    return_value = T(*reinterpret_cast<const int32_t *>(ptr));
    break;
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT64:
    return_value = T(*reinterpret_cast<const uint64_t *>(ptr));
    break;
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT64:
    return_value = T(*reinterpret_cast<const int64_t *>(ptr));
    break;
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_FLOAT:
    return_value = T(*reinterpret_cast<const float *>(ptr));
    break;
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_DOUBLE:
    return_value = T(*reinterpret_cast<const double *>(ptr));
    break;
  default:
    return false;
  }
  return true;
}

static inline std::string GetCppTypeFromSignalType(bridgesdk::plugin_addons::SignalType type) {

  switch (type) {
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_CHAR:
    return typeid(char).name();
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT8:
    return typeid(uint8_t).name();
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT8:
    return typeid(int8_t).name();
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT16:
    return typeid(uint16_t).name();
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT16:
    return typeid(int16_t).name();
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT32:
    return typeid(uint32_t).name();
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT32:
    return typeid(int32_t).name();
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_UINT64:
    return typeid(uint64_t).name();
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_INT64:
    return typeid(int64_t).name();
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_FLOAT:
    return typeid(float).name();
  case bridgesdk::plugin_addons::SignalType::SIGNAL_TYPE_DOUBLE:
    return typeid(double).name();
  default:
    return "none";
  }
}

template <typename T>
static inline bool CheckTypeTemplateAndSignalMatching(bridgesdk::plugin_addons::SignalType sig_type) {
  if (simpleTypeConsistencyCheck<T>(GetCppTypeFromSignalType(sig_type)) == true) {
    return true;
  }
  return false;
}

template <typename T>
static inline bool ParseDataFromBinaryBlob(const next::bridgesdk::ChannelMessagePointers *raw_data,
                                           const bridgesdk::plugin_addons::Signals &signal_list,
                                           const std::string &signal_name, std::vector<T> &return_vector) {
  static_assert(std::is_floating_point<T>::value || std::is_integral<T>::value,
                "only floating point and integral types are allowed (e.g. float, double, short, unsigned int...)");

  if (!raw_data || !raw_data->data) {
    sdk::logger::warn(__FILE__, "[Extractor] nullptr given for input");
    return false;
  }

  // extract valid signal from list
  const auto found_signal = signal_list.raw_signal_info.find(signal_name);
  if (found_signal == signal_list.raw_signal_info.end()) {
    return false;
  }
  const bridgesdk::plugin_addons::RawSignalInfo &info = found_signal->second;

  if (info.signal_offset + info.array_length_ * info.payload_size_ > raw_data->size) {
    sdk::logger::warn(__FILE__, "[Extractor] Message size invalid");
    return false;
  }

  T single_value{0};
  const char *binary_pointer = static_cast<const char *>(raw_data->data);
  if (CheckTypeTemplateAndSignalMatching<T>(info.sig_type_) == true) {
    if (info.payload_size_ != sizeof(T)) {
      sdk::logger::warn(__FILE__, "[Extractor] Type invalid");
      return false;
    }
    const T *ptr = reinterpret_cast<const T *>(binary_pointer + info.signal_offset);
    return_vector.clear();
    return_vector.reserve(info.array_length_);
    return_vector.insert(return_vector.end(), ptr, ptr + info.array_length_);
  } else {
    for (size_t i_value = 0; i_value < info.array_length_; i_value++) {
      if (transformValueToTemplateType(info, binary_pointer, single_value)) {
        return_vector.push_back(single_value);
      } else {
        sdk::logger::warn(__FILE__, "[Extractor] Type invalid");
        return false;
      }
      binary_pointer += info.payload_size_;
    }
  }
  return true;
}

template <typename T>
static inline bool ParseDataFromBinaryBlob(const next::bridgesdk::ChannelMessagePointers *raw_data,
                                           const bridgesdk::plugin_addons::Signals &signal_list,
                                           const std::string &signal_name, T &return_value) {
  static_assert(std::is_floating_point<T>::value || std::is_integral<T>::value,
                "only floating point and integral types are allowed (e.g. float, double, short, unsigned int...)");

  if (!raw_data || !raw_data->data) {
    sdk::logger::warn(__FILE__, "[Extractor] nullptr given for input");
    return false;
  }

  // extract valid signal from list
  auto found_signal = signal_list.raw_signal_info.find(signal_name);
  if (found_signal == signal_list.raw_signal_info.end()) {
    return false;
  }
  bridgesdk::plugin_addons::RawSignalInfo info = found_signal->second;

  char *binary_pointer = (char *)raw_data->data;

  // return if value not valid due to array
  if (info.array_length_ != 1) {
    sdk::logger::warn(__FILE__, "[Extractor] Message size invalid");
    return false;
  }
  // check if memory is valid
  if (info.signal_offset + info.payload_size_ > raw_data->size) {
    sdk::logger::warn(__FILE__, "[Extractor] Message size invalid");
    return false;
  }

  if (CheckTypeTemplateAndSignalMatching<T>(info.sig_type_) == true) {
    T *return_value_ptr = (T *)(binary_pointer + info.signal_offset);
    return_value = return_value_ptr[0];
  } else {
    if (sizeof(return_value) < info.payload_size_) {
      return false;
    }

    if (true == transformValueToTemplateType(info, binary_pointer, return_value)) {
      return true;
    }

    // \todo make big endian, small endian magic here!

    sdk::logger::debug(__FILE__, "[Extractor] Type invalid");
    return false;
  }
  return true;
} // namespace plugin_addons

} // namespace plugin_addons
} // namespace bridgesdk
} // namespace next

#endif // NEXT_BRIDGESDK_PLUGIN_HELPER_DATA_EXTRACTOR_H_

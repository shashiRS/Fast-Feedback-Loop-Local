/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
/**
 * @file     helper_data_extractor.h
 * @brief    small functionality set to be used to extract data
 *
 **/

#ifndef HELPER_DATA_EXTRACTOR_H
#define HELPER_DATA_EXTRACTOR_H

#include <cstdint>
#include <string>

#ifndef NO_USE_LOGGER_NAMESPACE
#define UNDEF_NO_USE_LOGGER_NAMESPACE
#endif
#define NO_USE_LOGGER_NAMESPACE
#include <next/sdk/logger/logger.hpp>
#ifdef UNDEF_NO_USE_LOGGER_NAMESPACE
#undef NO_USE_LOGGER_NAMESPACE
#undef UNDEF_NO_USE_LOGGER_NAMESPACE
#endif

#include <next/udex/data_types.hpp>
#include <next/udex/type_casting.hpp>

namespace next {
namespace udex {
namespace extractor {

template <typename T>
bool transformValueToTemplateType(const SignalInfo &info, const char *binary_pointer, T &return_value) {
  if (sizeof(T) < info.signal_size) {
    return false;
  }
  const std::byte *ptr = reinterpret_cast<const std::byte *>(binary_pointer) + info.offset;

  switch (info.signal_type) {
  case SignalType::SIGNAL_TYPE_CHAR:
    return_value = T(*reinterpret_cast<const char *>(ptr));
    break;
  case SignalType::SIGNAL_TYPE_UINT8:
    return_value = T(*reinterpret_cast<const uint8_t *>(ptr));
    break;
  case SignalType::SIGNAL_TYPE_INT8:
    return_value = T(*reinterpret_cast<const int8_t *>(ptr));
    break;
  case SignalType::SIGNAL_TYPE_UINT16:
    return_value = T(*reinterpret_cast<const uint16_t *>(ptr));
    break;
  case SignalType::SIGNAL_TYPE_INT16:
    return_value = T(*reinterpret_cast<const int16_t *>(ptr));
    break;
  case SignalType::SIGNAL_TYPE_UINT32:
    return_value = T(*reinterpret_cast<const uint32_t *>(ptr));
    break;
  case SignalType::SIGNAL_TYPE_INT32:
    return_value = T(*reinterpret_cast<const int32_t *>(ptr));
    break;
  case SignalType::SIGNAL_TYPE_UINT64:
    return_value = T(*reinterpret_cast<const uint64_t *>(ptr));
    break;
  case SignalType::SIGNAL_TYPE_INT64:
    return_value = T(*reinterpret_cast<const int64_t *>(ptr));
    break;
  case SignalType::SIGNAL_TYPE_FLOAT:
    return_value = T(*reinterpret_cast<const float *>(ptr));
    break;
  case SignalType::SIGNAL_TYPE_DOUBLE:
    return_value = T(*reinterpret_cast<const double *>(ptr));
    break;
  default:
    return false;
  }
  return true;
}

template <typename T>
static inline bool ParseDataFromBinaryBlob(const void *raw_data, const size_t raw_data_size, const SignalInfo &info,
                                           std::vector<T> &return_vector) {
  static_assert(std::is_floating_point<T>::value || std::is_integral<T>::value,
                "only floating point and integral types are allowed (e.g. float, double, short, unsigned int...)");

  if (!raw_data) {
    next::sdk::logger::info(__FILE__, "[Extractor] nullptr given for input");
    return false;
  }

  if (info.offset + info.array_size * info.signal_size > raw_data_size) {
    next::sdk::logger::info(__FILE__, "[Extractor] Message size invalid");
    return false;
  }

  T single_value{0};
  const char *binary_pointer = static_cast<const char *>(raw_data);
  if (casting::CheckTypeConsistency<T>(casting::GetTypeFromSignalType(info.signal_type)) == true) {
    if (info.signal_size != sizeof(T)) {
      next::sdk::logger::info(__FILE__, "[Extractor] Type invalid");
      return false;
    }
    const T *ptr = reinterpret_cast<const T *>(binary_pointer + info.offset);
    return_vector.clear();
    return_vector.reserve(info.array_size);
    return_vector.insert(return_vector.end(), ptr, ptr + info.array_size);
  } else {
    for (size_t i_value = 0; i_value < info.array_size; i_value++) {
      if (transformValueToTemplateType(info, binary_pointer, single_value)) {
        return_vector.push_back(single_value);
      } else {
        next::sdk::logger::info(__FILE__, "[Extractor] Type invalid");
        return false;
      }
      binary_pointer += info.signal_size;
    }
  }
  return true;
}

template <typename T>
static inline bool ParseDataFromBinaryBlob(const void *raw_data, const size_t raw_data_size, const SignalInfo &info,
                                           T &return_value) {
  static_assert(std::is_floating_point<T>::value || std::is_integral<T>::value,
                "only floating point and integral types are allowed (e.g. float, double, short, unsigned int...)");

  if (!raw_data) {
    next::sdk::logger::info(__FILE__, "[Extractor] nullptr given for input");
    return false;
  }

  const char *binary_pointer = static_cast<const char *>(raw_data);

  // return if value not valid due to array
  if (info.array_size != 1) {
    next::sdk::logger::info(__FILE__, "[Extractor] Message size invalid");
    return false;
  }
  // check if memory is valid
  if (info.offset + info.signal_size > raw_data_size) {
    next::sdk::logger::info(__FILE__, "[Extractor] Message size invalid");
    return false;
  }

  if (casting::CheckTypeConsistency<T>(casting::GetTypeFromSignalType(info.signal_type)) == true) {
    T *return_value_ptr = (T *)(binary_pointer + info.offset);
    return_value = return_value_ptr[0];
  } else {

    if (sizeof(return_value) < info.signal_size) {
      return false;
    }

    if (!transformValueToTemplateType(info, binary_pointer, return_value)) {
      return false;
    }

    // \todo make big endian, small endian magic here!

    // info("[Extractor] Type invalid");
    // return true;
  }
  return true;
}

} // namespace extractor
} // namespace udex
} // namespace next

#endif // HELPER_DATA_EXTRACTOR_H

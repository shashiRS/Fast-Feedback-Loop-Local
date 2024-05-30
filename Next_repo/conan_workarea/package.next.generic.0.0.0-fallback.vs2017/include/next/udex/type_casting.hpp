/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
/*
 * @file     type_casting.hpp
 * @brief    helper functions to cast values with help of SignalTypes
 * functinos
 */
#ifndef NEXT_UDEX_TYPE_CASTING_HPP
#define NEXT_UDEX_TYPE_CASTING_HPP

#include <cstddef>

#include "data_types.hpp"

namespace next {
namespace udex {
namespace casting {

template <typename T>
inline bool CheckTypeConsistency(const std::string &cpp_type) {
  if (cpp_type.compare(typeid(T).name()) == 0) {
    return true;
  }
  return false;
}

inline std::string GetTypeFromSignalType(const SignalType type) {
  switch (type) {
  case SignalType::SIGNAL_TYPE_CHAR:
    return typeid(char).name();
  case SignalType::SIGNAL_TYPE_UINT8:
    return typeid(uint8_t).name();
  case SignalType::SIGNAL_TYPE_INT8:
    return typeid(int8_t).name();
  case SignalType::SIGNAL_TYPE_UINT16:
    return typeid(uint16_t).name();
  case SignalType::SIGNAL_TYPE_INT16:
    return typeid(int16_t).name();
  case SignalType::SIGNAL_TYPE_UINT32:
    return typeid(uint32_t).name();
  case SignalType::SIGNAL_TYPE_INT32:
    return typeid(int32_t).name();
  case SignalType::SIGNAL_TYPE_UINT64:
    return typeid(uint64_t).name();
  case SignalType::SIGNAL_TYPE_INT64:
    return typeid(int64_t).name();
  case SignalType::SIGNAL_TYPE_FLOAT:
    return typeid(float).name();
  case SignalType::SIGNAL_TYPE_DOUBLE:
    return typeid(double).name();
  default:
    return "none";
  }
}

template <typename T>
inline void castValueFromSignalToRequestType(const SignalType &type, const void *binary_pointer, T &return_value) {
  const std::byte *ptr = reinterpret_cast<const std::byte *>(binary_pointer);
  switch (type) {
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
    break;
  }
}

template <typename T>
inline void castValue(const SignalType type, const void *address, const size_t max_size, T &return_value) {
  if (CheckTypeConsistency<T>(GetTypeFromSignalType(type)) == true) {
    if (max_size < sizeof(T)) {
      return_value = 0;
      return;
    }
    T *return_value_ptr = (T *)(address);
    return_value = return_value_ptr[0];
    return;
  }
  castValueFromSignalToRequestType(type, address, return_value);
  return;
}

} // namespace casting
} // namespace udex
} // namespace next
#endif // NEXT_UDEX_TYPE_CASTING_HPP

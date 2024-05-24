/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     data_types.hpp
 * @brief    Common data types used within next
 *
 */

#ifndef NEXT_DATA_TYPES_H_
#define NEXT_DATA_TYPES_H_

#include <string>
#include <unordered_map>

namespace next {

struct SyncSignalPortMapping {
  std::string syncref_signal_url;
  std::string synchronized_port_member_name;
};

struct SyncMapping {
  SyncSignalPortMapping signal_url_timestamp;
  SyncSignalPortMapping signal_url_syncid;
};

struct Trigger {
  std::string node_name;
  std::string methode_name;
};

enum ErrorCode {
  SUCCESS = 0,
  EXEC_FAILED = 1,
  SYNC_FAILED = 2,
  FW_FAILED = 3,
  PARSE_RESPONSE_FAIL = 4,
  UNKNOWN_FAILED = 5
};

struct TriggerReturn {
  enum ErrorCode error;
  int SigState;
  std::string error_message;
};

const std::unordered_map<std::string, ErrorCode> err_str_to_code = {
    {"SUCCESS", ErrorCode::SUCCESS},
    {"EXEC_FAILED", ErrorCode::EXEC_FAILED},
    {"SYNC_FAILED", ErrorCode::SYNC_FAILED},
    {"FW_FAILED", ErrorCode::FW_FAILED},
    {"PARSE_RESPONSE_FAIL", ErrorCode::PARSE_RESPONSE_FAIL},
    {"UNKNOWN_FAILED", ErrorCode::UNKNOWN_FAILED}};

inline ErrorCode getErrorCodeFromString(const std::string &err) {
  auto it = err_str_to_code.find(err);
  if (it != err_str_to_code.end())
    return it->second;
  return ErrorCode::UNKNOWN_FAILED;
}

enum DataAvailabilityMode { ALWAYS_TRUE, DATA_BASED, NOT_SPECIFIED };

inline DataAvailabilityMode getDataAvilabilityEnum(std::string format_str) {
  if (format_str == "ALWAYS_TRUE") {
    return DataAvailabilityMode::ALWAYS_TRUE;
  } else if (format_str == "DATA_BASED") {
    return DataAvailabilityMode::DATA_BASED;
  }

  return DataAvailabilityMode::NOT_SPECIFIED;
}
} // namespace next

#endif // NEXT_DATA_TYPES_H_

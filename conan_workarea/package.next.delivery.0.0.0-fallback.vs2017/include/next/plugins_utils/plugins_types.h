/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugins_types.h
 * @brief    general types being used within some plugins not to be transfered
 *           to the generic plugin interface
 **/

#ifndef NEXT_PLUGINS_UTILS_PLUGINS_TYPES_H_
#define NEXT_PLUGINS_UTILS_PLUGINS_TYPES_H_

#include <string>
#include <vector>

namespace next {
namespace plugins_utils {

struct SensorConfig {
  std::string sensor_url;
  std::string sensor_name;
  std::string data_view_name;
  std::string data_cache_name;
  std::vector<size_t> subscription_id;
  uint32_t sensor_id;
};

//! compare operator for @SensorConfig
inline bool operator==(const SensorConfig &lhs, const SensorConfig &rhs) {
  return (lhs.sensor_url == rhs.sensor_url && lhs.sensor_name == rhs.sensor_name &&
          lhs.data_view_name == rhs.data_view_name && lhs.data_cache_name == rhs.data_cache_name &&
          lhs.sensor_id == rhs.sensor_id && lhs.subscription_id == rhs.subscription_id);
}

} // namespace plugins_utils
} // namespace next

#endif // NEXT_PLUGINS_UTILS_PLUGINS_TYPES_H_

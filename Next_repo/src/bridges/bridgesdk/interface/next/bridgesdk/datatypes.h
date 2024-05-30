/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     datatypes.h
 * @brief    Common datatypes within next_bridges SDK
 *
 *
 **/

#ifndef NEXT_BRIDGESDK_DATATYPES_H_
#define NEXT_BRIDGESDK_DATATYPES_H_

#include <functional>
#include <map>
#include <string>

namespace next {
namespace bridgesdk {

//! time in microseconds
typedef uint64_t time_t;

struct TimestampedDataPacket {
  void *data{nullptr};
  size_t size;
  time_t timestamp;
};

//! Message for transmitting data
/*!
 *  This struct is carying the data of a data channel. The data is referenced
 *  by pointers to avoid copying them when passing the message.
 */
struct ChannelMessagePointers {
  const char *channel_name;
  const void *data;
  size_t size;
  time_t time;
};

//!< Subscription Type
enum SubscriptionType : std::int8_t { DIRECT, RINGBUFFER, BLOCKING };

//! type definiation for calbacks
using pluginCallbackT = std::function<void(const std::string, const ChannelMessagePointers *)>;

struct SignalUrl {
  std::string url_name;
  bool leaf;
};

// Plugin Description Datatypes
//! Plugin possible COnfig types
enum class ConfigType : std::uint8_t {
  EDITABLE,          //! config can be changed (mutable)
  FIXED,             //! config is fixed and can't be changed (immutable)
  SELECTABLE,        //! config is part of a selection list
  SELECTABLE_FIXED,  //! config is part of a selection list
  MULTISELECT,       //! config is part of a selection list
  MULTISELECT_FIXED, //! config is part of a selection list
};

//! struct that contains one single param value, this will be added in
struct PluginConfigParam {
  ConfigType config_type;
  std::string value;
};
//! compare operator for @PluginConfigParam
inline bool operator==(const PluginConfigParam &lhs, const PluginConfigParam &rhs) {
  return (lhs.config_type == rhs.config_type && lhs.value == rhs.value);
}

using plugin_config_t = std::multimap<std::string, PluginConfigParam>;

namespace plugin {
struct CoordinateSystemTransform {
  float translationX{0.0F};
  float translationY{0.0F};
  float translationZ{0.0F};
  float rotationX{0.0F};
  float rotationY{0.0F};
  float rotationZ{0.0F};
};

//! contains all info needed by the GUI to show the Sensor
struct SensorInfoForGUI {
  std::string name;
  std::string stream;
  std::string color;
  std::string type;
  std::string referenceCoordinateSystem;
  std::string flatbuffer_bfbs_path;   // path to the binary flatbuffer schema
  std::string flatbuffer_bfbs_base64; // Base64 encoded binary flatbuffer schema
  std::string schemaName;             // flatbuffer schema name
  CoordinateSystemTransform coordinateSystem;
};

inline bool operator==(const SensorInfoForGUI &lhs, const SensorInfoForGUI &rhs) {
  if (lhs.name == rhs.name && lhs.stream == rhs.stream && lhs.color == rhs.color && lhs.type == rhs.type &&
      lhs.referenceCoordinateSystem == rhs.referenceCoordinateSystem) {
    return true;
  }
  return false;
}

struct SensorInfoForFbs {
  std::string data_view;
  std::string cache_name;
  uint32_t sensor_id;
  time_t timestamp_microsecond = 0; //!< current timestamp of the datapackage
};

struct FlatbufferPackage {
  const uint8_t *fbs_binary;
  size_t fbs_size;
};

} // namespace plugin
} // namespace bridgesdk
} // namespace next

#endif // NEXT_BRIDGESDK_DATATYPES_H_

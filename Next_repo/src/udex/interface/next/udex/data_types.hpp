/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */

#ifndef UDEX_DATA_TYPES_HPP
#define UDEX_DATA_TYPES_HPP

#include <cstring>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <variant>

#include <next/sdk/types/package_data_types.hpp>

namespace next {
namespace udex {

/*! Describes the kind of package which is send over. Used for improved speed or sorting of packages
 *
 */
enum PackageMode {
  PACKAGE_MODE_NOTSET = -1,
  PACKAGE_MODE_SDL,    //< sdl / ecu / c-like flat memory layout data
  PACKAGE_MODE_CAN,    //< can data with respective space compressed layout
  PACKAGE_MODE_FIBEX,  //< fibex / ethernet data
  PACKAGE_MODE_BINARY, //< raw binary data with no detailed description
  PACKAGE_MODE_DEBUG   //< debug data with with reflection based description
};

/*! Meta Information per package. Changes per send datapackage
 *
 */
struct MetaInfo {
  // usefull data
  uint64_t mts_timestamp; //< mts_timestamp to synchronizie simluation
  next::sdk::types::PackageFormatType format_type{
      sdk::types::PACKAGE_FORMAT_TYPE_UNKNOWN}; //!< format_type used for data extraction
  uint8_t cycle_state;                          //!< cycle_state might be used later for triggering

  // needed for intermediate solution with backward compatibility
  bool dynamic_package = true;     //!< DynamicPublisher package
  bool processor_required = false; //!< post-processing of package required

  // dummy or TBD
  int package_id;                         //!< package id for extraction
  PackageMode mode = PACKAGE_MODE_NOTSET; //!< package mode -> can be checked for testing agains the description
};

/*! Basic information about a Signal description
 * Requires parent_url to be linked within url based simulation context
 * Mode is required to enable correct SignalDescription handling
 */
struct BasicInformation {
  // usefull data
  std::string parent_url = ""; //< base url of package
  std::string data_source_name = "";
  std::string view_name = "";
  std::string group_name = "";
  next::sdk::types::PackageFormatType format_type{
      sdk::types::PACKAGE_FORMAT_TYPE_UNKNOWN}; //< format_type used for data extraction
  uint16_t source_id = 0;
  uint32_t instance_id = 0;
  uint32_t cycle_id = 0;
  uint64_t vaddr = 0;

  // dummy or TBD
  PackageMode mode = PACKAGE_MODE_NOTSET; //< package mode
  int dummy = 0;
};

/*! Signal Description contains the binary format to decode the binary blob as well as some additional information
 * to enable a network wide interpretation.
 * Signal Description is serialized into a string on a base64 approach for binary data and serialized with JSON
 *
 * 1) basic_info is some meta data needed for extraction
 * 2) binary_data_descriptoin is for example the Xtype Schema file from mts_core_lib
 *
 */
struct SignalDescription {
  BasicInformation basic_info; //< basic description
  std::vector<char>
      binary_data_description; //< binary representation which gets encoded into base64 for transport layer
};

struct UdexDataPackageInfo {
  uint64_t Time; //!< network time stamp -> does not need to match recording timestamp
  next::sdk::types::PackageFormatType format_type{
      sdk::types::PACKAGE_FORMAT_TYPE_UNKNOWN}; //< format_type used for data extraction
  bool processor_required = false;              //!< requires a post processor to extract data
  std::string PackageUrl;                       //!< package url which is the top level url of the receive binary blob
};

//! network data package defintion
struct UdexDataPackage {
  const void *Data; //!< actual raw memory location of package
  size_t Size;      //!< size of raw memory
  UdexDataPackageInfo info;
};

/*! Basic information about a "UdexDataPackage"
 */
struct PackageInfo {
  std::string data_source_name = "";
  uint16_t source_id;
  uint32_t instance_id;
  uint32_t cycle_id;
  next::sdk::types::PackageFormatType format_type;
  bool processor_required = false;
};

constexpr const size_t kMaxPayloadSize = 255;
constexpr const size_t kMaxNumOfMsg = 1000;

enum class SignalType : std::uint8_t {
  SIGNAL_TYPE_BOOL,
  SIGNAL_TYPE_CHAR,
  SIGNAL_TYPE_UINT8,
  SIGNAL_TYPE_INT8,
  SIGNAL_TYPE_UINT16,
  SIGNAL_TYPE_INT16,
  SIGNAL_TYPE_UINT32,
  SIGNAL_TYPE_INT32,
  SIGNAL_TYPE_UINT64,
  SIGNAL_TYPE_INT64,
  SIGNAL_TYPE_FLOAT,
  SIGNAL_TYPE_DOUBLE,
  SIGNAL_TYPE_STRUCT
};

const std::map<SignalType, size_t> any_value_size_map = {
    {SignalType::SIGNAL_TYPE_CHAR, sizeof(char)},     {SignalType::SIGNAL_TYPE_UINT8, sizeof(uint8_t)},
    {SignalType::SIGNAL_TYPE_INT8, sizeof(int8_t)},   {SignalType::SIGNAL_TYPE_UINT16, sizeof(uint16_t)},
    {SignalType::SIGNAL_TYPE_INT16, sizeof(int16_t)}, {SignalType::SIGNAL_TYPE_UINT32, sizeof(uint32_t)},
    {SignalType::SIGNAL_TYPE_INT32, sizeof(int32_t)}, {SignalType::SIGNAL_TYPE_UINT64, sizeof(uint64_t)},
    {SignalType::SIGNAL_TYPE_INT64, sizeof(int64_t)}, {SignalType::SIGNAL_TYPE_FLOAT, sizeof(float)},
    {SignalType::SIGNAL_TYPE_DOUBLE, sizeof(double)}, {SignalType::SIGNAL_TYPE_BOOL, sizeof(bool)}};

struct PayloadType {
  size_t payload_size_;
  SignalType sig_type_;
  size_t array_length_;
  size_t start_offset_;
};

struct Signal {
  std::unordered_map<std::string, PayloadType> data_{};
};

const std::unordered_map<int, SignalType> data_type_mapping_ = {
    {16397, SignalType::SIGNAL_TYPE_CHAR},  {16515, SignalType::SIGNAL_TYPE_UINT8},
    {16386, SignalType::SIGNAL_TYPE_INT8},  {16517, SignalType::SIGNAL_TYPE_UINT16},
    {16388, SignalType::SIGNAL_TYPE_INT16}, {16519, SignalType::SIGNAL_TYPE_UINT32},
    {16390, SignalType::SIGNAL_TYPE_INT32}, {16521, SignalType::SIGNAL_TYPE_UINT64},
    {16392, SignalType::SIGNAL_TYPE_INT64}, {16394, SignalType::SIGNAL_TYPE_FLOAT},
    {16395, SignalType::SIGNAL_TYPE_DOUBLE}};

struct ChildInfo {
  std::string name;
  size_t array_lenght{};
  int child_count{};
  inline bool operator==(const ChildInfo &rhs) {
    return name == rhs.name && child_count == rhs.child_count && array_lenght == rhs.array_lenght;
  }
  inline bool operator<(const ChildInfo &rhs) {
    return name < rhs.name;
  } // sort only by names to get a clean duplication removal based on name
};

typedef std::variant<bool, char, int8_t, uint8_t, int16_t, uint16_t, int32_t, uint32_t, int64_t, uint64_t, float,
                     double>
    dataType;

struct SignalInfo {
  SignalType signal_type = SignalType::SIGNAL_TYPE_STRUCT;
  size_t signal_size = 0;
  size_t array_size = 0;
  size_t offset = 0;
};

struct SyncValue {
  SignalType signal_type;
  dataType signal_value;
};

struct TimestampedDataPacket {
  void *data;
  size_t size;
  size_t timestamp;
};

using UdexCallbackT = std::function<void(const std::string)>;
using SortingAlgorithm = std::function<TimestampedDataPacket(
    std::list<std::shared_ptr<TimestampedDataPacket>> &local_list, bool &extrac_succesfull, std::string url,
    long long value, SignalInfo signal_info)>;

const unsigned short EXTRACTION_QUEUE_DEFAULT_SIZE = 100;

enum SyncAlgoType {
  GET_LAST_ELEMENT,
  FIND_EXACT_VALUE,
  FIND_EXACT_HEADER_TIMESTAMP,
  FIND_NEAREST_VALUE,
  DLL,
  WRONG_SYNC_ALGO_SPECIFIER
};

inline SyncAlgoType getSyncAlgoTypeEnum(std::string format_str) {
  if (format_str == "GET_LAST_ELEMENT") {
    return SyncAlgoType::GET_LAST_ELEMENT;
  } else if (format_str == "FIND_EXACT_VALUE") {
    return SyncAlgoType::FIND_EXACT_VALUE;
  } else if (format_str == "FIND_NEAREST_VALUE") {
    return SyncAlgoType::FIND_NEAREST_VALUE;
  } else if (format_str == "FIND_EXACT_HEADER_TIMESTAMP") {
    return SyncAlgoType::FIND_EXACT_HEADER_TIMESTAMP;
  }

  return SyncAlgoType::WRONG_SYNC_ALGO_SPECIFIER;
}

struct SyncAlgoCfg {
  SyncAlgoType algorythm_type = SyncAlgoType::GET_LAST_ELEMENT;
  std::string sync_signal_url = "";
  std::string composition_sync_signal_url = "";
};

struct CallbackInfo {
  next::udex::UdexCallbackT callback_function;
  std::string url;
  std::string topic;
};
//! type definition for callbacks
using statusCallbackT =
    std::function<void(const std::string &executed_function, const std::string &description, const bool areIndexesUsed,
                       const size_t currentIndex, const size_t maxIndex)>;
} // namespace udex
} // namespace next

enum DescriptionType {
  DESCRIPTION_TYPE_SDL,
  DESCRIPTION_TYPE_CDL,
  DESCRIPTION_TYPE_DBC,
  DESCRIPTION_TYPE_SWC,
  DESCRIPTION_TYPE_FIBEX,
  DESCRIPTION_TYPE_GPS_SDL,
  DESCRIPTION_TYPE_SW_CONTAINER,
  DESCRIPTION_TYPE_REFERENCE_CAMERA_SDL,
  DESCRIPTION_TYPE_ARXML
};

#endif // UDEX_DATA_TYPES_HPP

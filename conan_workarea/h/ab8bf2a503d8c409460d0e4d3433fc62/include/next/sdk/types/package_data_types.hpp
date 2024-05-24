/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     package_data_types.hpp
 * @brief    defines the data package structures within the NEXT simulation framework.
 *           Used by Next-UDex, Next-Player
 */

#ifndef NEXT_DATA_TYPES_HPP
#define NEXT_DATA_TYPES_HPP

#include <functional>
#include <memory>
#include <string>

namespace next {
namespace sdk {
namespace types {

/*! @brief Data format type
 *
 */
enum DataDescriptionFormatType {
  DESCRIPTION_FORMAT_TYPE_UNKNOWN = -1,
  DESCRIPTION_FORMAT_TYPE_SDL = 0,
  DESCRIPTION_FORMAT_TYPE_CDL = 1,
  DESCRIPTION_FORMAT_TYPE_DBC = 2,
  DESCRIPTION_FORMAT_TYPE_SWC = 3,
  DESCRIPTION_FORMAT_TYPE_FIBEX = 4,
  DESCRIPTION_FORMAT_TYPE_GPS_SDL = 5,
  DESCRIPTION_FORMAT_TYPE_SW_CONTAINER = 6,
  DESCRIPTION_FORMAT_TYPE_REFERENCE_CAMERA_SDL = 7,
  DESCRIPTION_FORMAT_TYPE_HW_DATA = 8,
  DESCRIPTION_FORMAT_TYPE_ARXML = 9
};

enum PackageFormatType {
  PACKAGE_FORMAT_TYPE_UNKNOWN = -1,
  PACKAGE_FORMAT_TYPE_TIMEBASE,
  PACKAGE_FORMAT_TYPE_GPS_GENERIC,
  PACKAGE_FORMAT_TYPE_GPS_NMEA,
  PACKAGE_FORMAT_TYPE_CAN,
  PACKAGE_FORMAT_TYPE_FLEXRAY,
  PACKAGE_FORMAT_TYPE_REFCAM,
  PACKAGE_FORMAT_TYPE_ETH,
  PACKAGE_FORMAT_TYPE_IBEO,
  PACKAGE_FORMAT_TYPE_CT4,
  PACKAGE_FORMAT_TYPE_XCP,
  PACKAGE_FORMAT_TYPE_RTRAGE,
  PACKAGE_FORMAT_TYPE_MTA,
  PACKAGE_FORMAT_TYPE_MTA_SW,
  PACKAGE_FORMAT_TYPE_MTA_HW
};

class IPackage {

public:
  struct PackageMetaInfo {
    uint16_t source_id = 0;
    uint32_t instance_number = 0;
    uint32_t cycle_id = 0;
    uint64_t virtual_address = 0;
    uint16_t service_id = 0;
    uint32_t method_id = 0;
    inline bool operator==(const PackageMetaInfo &other) const {
      return this->source_id == other.source_id && this->instance_number == other.instance_number &&
             this->cycle_id == other.cycle_id && this->virtual_address == other.virtual_address &&
             this->service_id == other.service_id && this->method_id == other.method_id;
    }
  };

  struct PackageHeader {
    size_t timestamp;
    PackageFormatType format;
    uint32_t cycle_id;
    uint8_t cycle_state;
  };

  struct Payload {
    size_t size;
    const unsigned char *mem_pointer;
  };

  /*! This is an enum for types of the package meta information. */
  enum PackageMetaType {
    PACKAGE_META_TYPE_NAME = 0, /*!< The meta information is defined by the package name. */
    PACKAGE_META_TYPE_INFO =
        1, /*!< The meta information is defined by the meta info package (sourceId, instanceId, ...) */
    PACKAGE_META_TYPE_VADDR = 2 /*!< The meta information is defined by the virtual address. */
  };

  using package_ptr = std::unique_ptr<void *, std::function<void(void *)>>;

public:
  IPackage(package_ptr ptr) { internal_package_ = std::move(ptr); };
  virtual ~IPackage() = default;

  IPackage(const IPackage &other) = delete;
  IPackage(IPackage &&other) = delete;

  virtual bool PayloadAvailable() = 0;
  virtual const Payload GetPayload() = 0;
  virtual size_t GetSize() = 0;

  virtual const PackageHeader &GetPackageHeader() = 0;

  virtual PackageMetaType GetMetaType() const = 0;

  virtual const PackageMetaInfo &GetMetaInfo() = 0;
  virtual const std::string &GetPackageName() const = 0;
  virtual size_t GetPackageVaddr() const = 0;

  virtual bool ResetPayloads() = 0;
  bool processor_required_ = false;

protected:
  package_ptr internal_package_;
};

template <class packageObject>
IPackage::package_ptr takePackageOwnership(packageObject &object, std::function<void(packageObject *)> deleter) {
  return IPackage::package_ptr(reinterpret_cast<void **>(&object), [deleter](void *toremove) {
    packageObject *castremoval = (packageObject *)toremove;
    deleter(castremoval);
  });
}

const std::unordered_map<DataDescriptionFormatType, std::string> data_description_format_type_to_string = {
    {DESCRIPTION_FORMAT_TYPE_UNKNOWN, ""},
    {DESCRIPTION_FORMAT_TYPE_SDL, "sdl"},
    {DESCRIPTION_FORMAT_TYPE_CDL, "cdl"},
    {DESCRIPTION_FORMAT_TYPE_DBC, "dbc"},
    {DESCRIPTION_FORMAT_TYPE_SWC, "swc"},
    {DESCRIPTION_FORMAT_TYPE_FIBEX, "fibex"},
    {DESCRIPTION_FORMAT_TYPE_ARXML, "arxml"},
    {DESCRIPTION_FORMAT_TYPE_GPS_SDL, "gps-sdl"},
    {DESCRIPTION_FORMAT_TYPE_SW_CONTAINER, "sw_container"},
    {DESCRIPTION_FORMAT_TYPE_REFERENCE_CAMERA_SDL, "reference-camera-sdl"},
    {DESCRIPTION_FORMAT_TYPE_HW_DATA, "hw_data"}};
const std::unordered_map<std::string, DataDescriptionFormatType> data_description_format_string_to_type = {
    {"", DESCRIPTION_FORMAT_TYPE_UNKNOWN},
    {"sdl", DESCRIPTION_FORMAT_TYPE_SDL},
    {"cdl", DESCRIPTION_FORMAT_TYPE_CDL},
    {"dbc", DESCRIPTION_FORMAT_TYPE_DBC},
    {"swc", DESCRIPTION_FORMAT_TYPE_SWC},
    {"fibex", DESCRIPTION_FORMAT_TYPE_FIBEX},
    {"arxml", DESCRIPTION_FORMAT_TYPE_ARXML},
    {"gps-sdl", DESCRIPTION_FORMAT_TYPE_GPS_SDL},
    {"sw_container", DESCRIPTION_FORMAT_TYPE_SW_CONTAINER},
    {"reference-camera-sdl", DESCRIPTION_FORMAT_TYPE_REFERENCE_CAMERA_SDL},
    {"hw_data", DESCRIPTION_FORMAT_TYPE_HW_DATA}};
inline DataDescriptionFormatType getDataDescriptionFormatEnum(std::string type_string) {
  auto found_element = next::sdk::types::data_description_format_string_to_type.find(type_string);
  if (found_element != next::sdk::types::data_description_format_string_to_type.end()) {
    return found_element->second;
  }
  return next::sdk::types::DESCRIPTION_FORMAT_TYPE_UNKNOWN;
}
inline std::string getDataDescriptionFormatString(DataDescriptionFormatType data_format) {
  auto string_list_element = data_description_format_type_to_string.find(data_format);
  if (string_list_element != data_description_format_type_to_string.end()) {
    return string_list_element->second;
  } else {
    return "";
  }
}

const std::unordered_map<PackageFormatType, std::string> package_format_type_to_string = {
    {PACKAGE_FORMAT_TYPE_UNKNOWN, ""},
    {PACKAGE_FORMAT_TYPE_TIMEBASE, "mts.timebase"},
    {PACKAGE_FORMAT_TYPE_GPS_GENERIC, "mts.gps.generic"},
    {PACKAGE_FORMAT_TYPE_GPS_NMEA, "mts.gps.nmea"},
    {PACKAGE_FORMAT_TYPE_CAN, "mts.can"},
    {PACKAGE_FORMAT_TYPE_FLEXRAY, "mts.flexray"},
    {PACKAGE_FORMAT_TYPE_REFCAM, "mts.refcam"},
    {PACKAGE_FORMAT_TYPE_ETH, "mts.eth"},
    {PACKAGE_FORMAT_TYPE_IBEO, "mts.ibeo"},
    {PACKAGE_FORMAT_TYPE_CT4, "mts.ct4"},
    {PACKAGE_FORMAT_TYPE_XCP, "mts.xcp"},
    {PACKAGE_FORMAT_TYPE_RTRAGE, "mts.rtrange"},
    {PACKAGE_FORMAT_TYPE_MTA, "mts.mta"},
    {PACKAGE_FORMAT_TYPE_MTA_SW, "mts.mta.sw"},
    {PACKAGE_FORMAT_TYPE_MTA_HW, "mts.mta.hw"}};
const std::unordered_map<std::string, PackageFormatType> package_format_string_to_format = {
    {"", PACKAGE_FORMAT_TYPE_UNKNOWN},
    {"mts.timebase", PACKAGE_FORMAT_TYPE_TIMEBASE},
    {"mts.gps.generic", PACKAGE_FORMAT_TYPE_GPS_GENERIC},
    {"mts.gps.nmea", PACKAGE_FORMAT_TYPE_GPS_NMEA},
    {"mts.can", PACKAGE_FORMAT_TYPE_CAN},
    {"mts.flexray", PACKAGE_FORMAT_TYPE_FLEXRAY},
    {"mts.refcam", PACKAGE_FORMAT_TYPE_REFCAM},
    {"mts.eth", PACKAGE_FORMAT_TYPE_ETH},
    {"mts.ibeo", PACKAGE_FORMAT_TYPE_IBEO},
    {"mts.ct4", PACKAGE_FORMAT_TYPE_CT4},
    {"mts.xcp", PACKAGE_FORMAT_TYPE_XCP},
    {"mts.rtrange", PACKAGE_FORMAT_TYPE_RTRAGE},
    {"mts.mta", PACKAGE_FORMAT_TYPE_MTA},
    {"mts.mta.sw", PACKAGE_FORMAT_TYPE_MTA_SW},
    {"mts.mta.hw", PACKAGE_FORMAT_TYPE_MTA_HW},
    {"mts.eth.rtrange", PACKAGE_FORMAT_TYPE_RTRAGE}};
inline PackageFormatType getPackageFormatTypeEnum(std::string format_str) {
  auto string_list_element = package_format_string_to_format.find(format_str);
  if (string_list_element != package_format_string_to_format.end()) {
    return string_list_element->second;
  } else {
    return PACKAGE_FORMAT_TYPE_UNKNOWN;
  }
}
inline std::string getPackageFormatTypeString(PackageFormatType format_type) {
  auto string_list_element = package_format_type_to_string.find(format_type);
  if (string_list_element != package_format_type_to_string.end()) {
    return string_list_element->second;
  } else {
    return "";
  }
}

} // namespace types
} // namespace sdk
} // namespace next

#endif // NEXT_DATA_TYPES_HPP

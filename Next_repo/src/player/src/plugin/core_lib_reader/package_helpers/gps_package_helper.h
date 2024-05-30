/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
#ifndef NEXT_HW_PACKAGE_HELPER_H
#define NEXT_HW_PACKAGE_HELPER_H

#include <mts/toolbox/gps/gps.hpp>
#include "next/sdk/types/package_data_types.hpp"

#include "../core_lib_datatypes.h"

class GpsPackage {
public:
  static next::sdk::types::IPackage::PackageMetaInfo CreateMetaInfo(mts::extensibility::package &package,
                                                                    size_t &size) {

    next::sdk::types::IPackage::PackageMetaInfo info;
    info.instance_number = package->instance_number;
    info.source_id = package->source_id;
    info.cycle_id = package->cycle_id;
    info.virtual_address = 0;
    try {
      auto nmea_packet = mts::toolbox::gps::get_nmea_packet<mts::extensibility::package>(&package);
      auto nmea_payload = mts::toolbox::gps::get_payload<mts::extensibility::package>(&package);
      if (nmea_packet == nullptr) {
        return info;
      }
      info.virtual_address = (size_t)((nmea_packet->packet_type + 0x100) * 0x1000);
      size = std::get<1>(nmea_payload);
    } catch (std::runtime_error &error) {
      size = 0u;
      next::sdk::logger::error(
          __FILE__,
          "Failed to get payload info from GPS package. Source ID: {0}, Instance Number: {1}, Cycle ID: {2}, "
          "TimeStamp: {3}, Error: {4}",
          package->source_id, package->instance_number, package->cycle_id, package->reference_timestamp, error.what());
    }
    return info;
  }
  static next::sdk::types::IPackage::Payload GetPackagePayload(const mts::extensibility::package &package) {
    next::sdk::types::IPackage::Payload mem{0, nullptr};
    try {
      auto resolved_tuple = mts::toolbox::gps::get_payload<mts::extensibility::package>(&package);
      mem.mem_pointer = std::get<0>(resolved_tuple);
      mem.size = std::get<1>(resolved_tuple);
    } catch (std::runtime_error &error) {
      next::sdk::logger::error(
          __FILE__,
          "Failed to get payload info from GPS package. Source ID: {0}, Instance Number: {1}, Cycle ID: {2}, "
          "TimeStamp: {3}, Error: {4}",
          package->source_id, package->instance_number, package->cycle_id, package->reference_timestamp, error.what());
    }
    return mem;
  }
};

#endif // NEXT_HW_PACKAGE_HELPER_H

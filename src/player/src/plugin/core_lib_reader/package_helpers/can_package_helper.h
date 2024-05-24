/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
#ifndef NEXT_CAN_PACKAGE_HELPER_H
#define NEXT_CAN_PACKAGE_HELPER_H

#include "next/sdk/types/package_data_types.hpp"

#include <mts/toolbox/can/can_types.h>
#include <mts/toolbox/can/can.hpp>

#include "../core_lib_datatypes.h"

class CanPackage {
public:
  static next::sdk::types::IPackage::PackageMetaInfo CreateMetaInfo(mts::extensibility::package &package,
                                                                    size_t &size) {
    using namespace mts::toolbox::can;
    next::sdk::types::IPackage::PackageMetaInfo info;
    info.instance_number = package->instance_number;
    info.source_id = package->source_id;
    info.virtual_address = 0;
    try {
      auto [payload, payload_length] = mts::toolbox::can::get_payload<mts::extensibility::package>(&package);
      info.cycle_id = package->cycle_id;
      size = payload_length;
      static_cast<void>(payload);
    } catch (std::runtime_error &error) {
      next::sdk::logger::error(
          __FILE__,
          "Failed to get payload info from CAN package. Source ID: {0}, Instance Number: {1}, Message ID: {2}, "
          "TimeStamp: {3}, Error: {4}",
          package->source_id, package->instance_number, package->cycle_id, package->reference_timestamp, error.what());
      info.cycle_id = 0;
      size = 0;
    }
    return info;
  }
  static next::sdk::types::IPackage::Payload GetPackagePayload(const mts::extensibility::package &package) {
    next::sdk::types::IPackage::Payload mem{0, nullptr};
    try {
      auto [payload, payload_length] = mts::toolbox::can::get_payload<mts::extensibility::package>(&package);
      mem.mem_pointer = payload;
      mem.size = payload_length;
    } catch (std::runtime_error &error) {
      next::sdk::logger::error(
          __FILE__,
          "Failed to get payload info from CAN package. Source ID: {0}, Instance Number: {1}, Message ID: {2}, "
          "TimeStamp: {3}, Error: {4}",
          package->source_id, package->instance_number, package->cycle_id, package->reference_timestamp, error.what());
    }
    return mem;
  }
};

#endif // NEXT_CAN_PACKAGE_HELPER_H

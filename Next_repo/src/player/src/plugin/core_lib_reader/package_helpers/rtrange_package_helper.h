/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
#ifndef NEXT_RTRANGE_PACKAGE_HELPER_H
#define NEXT_RTRANGE_PACKAGE_HELPER_H

#include "next/sdk/types/package_data_types.hpp"

#include <mts/extensibility/package.hpp>
#include <mts/toolbox/rt_range/rt_range.hpp>

#include "../core_lib_datatypes.h"

// TODO: make this working
class RtrangePackage {
public:
  static next::sdk::types::IPackage::PackageMetaInfo CreateMetaInfo(mts::extensibility::package &package,
                                                                    size_t &size) {
    using namespace mts::toolbox::rt_range;
    next::sdk::types::IPackage::PackageMetaInfo info;
    info.instance_number = package->instance_number;
    info.source_id = package->source_id;
    info.cycle_id = package->cycle_id;
    info.virtual_address = 0;
    try {
      auto rt_range_package_header = get_package_header<mts::extensibility::package>(&package);
      // Validate package content.
      info.virtual_address = rt_range_package_header->address;
      switch (rt_range_package_header->address) {
      case udex_virtual_address_rcom: {
        /*Validate and get the RCOM package*/
        auto rcom_package = get_rcom_packet<mts::extensibility::package>(&package);
        size = sizeof(*rcom_package);
        break;
      }
      case udex_virtual_address_ncom_hunter:
      case udex_virtual_address_ncom_target_01:
      case udex_virtual_address_ncom_target_02:
      case udex_virtual_address_ncom_target_03:
      case udex_virtual_address_ncom_target_04: {
        /*Validate and get the NCOM package*/
        auto ncom_package = get_ncom_packet<mts::extensibility::package>(&package);
        size = sizeof(*ncom_package);
        break;
      }
      default:
        return info;
      }
    } catch (std::runtime_error &error) {
      size = 0u;
      next::sdk::logger::error(
          __FILE__,
          "Failed to get payload info from RT-Range package. Source ID: {0}, Instance Number: {1}, Cycle ID: {2}, "
          "TimeStamp: {3}, Error: {4}",
          package->source_id, package->instance_number, package->cycle_id, package->reference_timestamp, error.what());
    }
    return info;
  }
  static next::sdk::types::IPackage::Payload GetPackagePayload(const mts::extensibility::package &package) {
    using namespace mts::toolbox::rt_range;
    next::sdk::types::IPackage::Payload mem{0, nullptr};
    try {
      auto rt_range_package_header = get_package_header<mts::extensibility::package>(&package);
      // Validate generic package content.
      switch (rt_range_package_header->address) {
      case udex_virtual_address_rcom: {
        /*Validate and get the RCOM package*/
        auto rcom_package = get_rcom_packet<mts::extensibility::package>(&package);
        mem.size = sizeof(*rcom_package);
        mem.mem_pointer = reinterpret_cast<const unsigned char *>(rcom_package);
        break;
      }
      case udex_virtual_address_ncom_hunter:
      case udex_virtual_address_ncom_target_01:
      case udex_virtual_address_ncom_target_02:
      case udex_virtual_address_ncom_target_03:
      case udex_virtual_address_ncom_target_04: {
        /*Validate and get the NCOM package*/
        auto ncom_package = get_ncom_packet<mts::extensibility::package>(&package);
        mem.size = sizeof(*ncom_package);
        mem.mem_pointer = reinterpret_cast<const unsigned char *>(ncom_package);
        break;
      }
      default:
        return mem;
      }
    } catch (std::runtime_error &error) {
      next::sdk::logger::error(
          __FILE__,
          "Failed to get payload info from RT-Range package. Source ID: {0}, Instance Number: {1}, Cycle ID: {2}, "
          "TimeStamp: {3}, Error: {4}",
          package->source_id, package->instance_number, package->cycle_id, package->reference_timestamp, error.what());
    }
    return mem;
  }
};

#endif // NEXT_RTRANGE_PACKAGE_HELPER_H

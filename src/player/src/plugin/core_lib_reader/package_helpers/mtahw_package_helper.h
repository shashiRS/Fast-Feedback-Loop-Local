/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
#ifndef NEXT_MTAHW_PACKAGE_HELPER_H
#define NEXT_MTAHW_PACKAGE_HELPER_H

#include <vector>

#include <mts/extensibility/sources.h>
#include <mts/extensibility/package.hpp>
#include <mts/toolbox/ecu/ecu.hpp>

#include <next/sdk/types/package_data_types.hpp>

#include "../core_lib_datatypes.h"
#include "package_interface.hpp"

class MtaHwPackage : public IPackageHelper {
public:
  MtaHwPackage(uint16_t source_id, uint32_t instance_number, uint32_t cycle_id,
               const std::unordered_map<size_t, CoreLibProcessor> &proc_info)
      : IPackageHelper() {
    CheckProcessorRequired(source_id, instance_number, cycle_id, 0, proc_info);
  }
  MtaHwPackage() = delete;

  void SetUp(mts::extensibility::package &package, size_t &size) override {
    if (processor_required_) {
      ProcessorSetUp(package, size);
    } else {
      package_available_ = true;
    }
  }

  next::sdk::types::IPackage::PackageMetaInfo GetMetaInfo(mts::extensibility::package &package, size_t &size) override {

    next::sdk::types::IPackage::PackageMetaInfo first_package_info;
    first_package_info.instance_number = package->instance_number;
    first_package_info.cycle_id = package->cycle_id;
    first_package_info.source_id = package->source_id;
    first_package_info.virtual_address = 0;

    try {
      auto [payload, payload_length] = mts::toolbox::ecu::get_payload<mts::extensibility::package>(&package);
      package_payload_address_ = payload;
      size_ = payload_length;
      size = payload_length; // total_package_size
    } catch (std::runtime_error &error) {
      size = 0;
      next::sdk::logger::error(__FILE__,
                               "Failed to get payload info from MTA Hardware package. Source ID: {0}, Instance "
                               "Number: {1}, Cycle ID: {2}, "
                               "TimeStamp: {3}, Error: {4}",
                               package->source_id, package->instance_number, package->cycle_id,
                               package->reference_timestamp, error.what());
    }
    return first_package_info;
  }

  next::sdk::types::IPackage::Payload GetPayload(const mts::extensibility::package &package) override {
    if (processor_required_) {
      return ProcessorPackage::ProcessorGetPackagePayload(package);
    }
    if (!package_available_) {
      return {};
    }
    next::sdk::types::IPackage::Payload mem{0, nullptr};
    mem.size = size_;
    mem.mem_pointer = package_payload_address_;

    package_available_ = false;

    return mem;
  }

private:
  size_t size_ = 0;
  const uint8_t *package_payload_address_{nullptr};
};

#endif // NEXT_MTAHW_PACKAGE_HELPER_H

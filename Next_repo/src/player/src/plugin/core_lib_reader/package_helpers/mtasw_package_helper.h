/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
#ifndef NEXT_MTASW_PACKAGE_HELPER_H
#define NEXT_MTASW_PACKAGE_HELPER_H

#include "next/sdk/types/package_data_types.hpp"
#include "next/sdk/types/package_hash.hpp"

#include <mts/toolbox/ecu/ecu_types.h>
#include <mts/toolbox/ecu/ecu.hpp>

#include "../core_lib_datatypes.h"

class MtaSwPackage : public IPackageHelper {
public:
  MtaSwPackage(const std::unordered_map<size_t, uint32_t> &hash_to_size_map,
               const std::unordered_map<size_t, uint64_t> &hash_to_next_vaddr_map,
               const std::unordered_map<size_t, CoreLibProcessor> &proc_info,
               const mts::extensibility::package &package)
      : IPackageHelper(), hash_to_size_map_(hash_to_size_map), hash_to_next_vaddr_map_(hash_to_next_vaddr_map) {
    auto *software_package_header = reinterpret_cast<const mts_ecu_software_header_t *>(package->data());
    if (software_package_header == nullptr) {
      return;
    }
    auto package_vaddr = software_package_header->address;
    CheckProcessorRequired(package->source_id, package->instance_number, package->cycle_id, package_vaddr, proc_info);
  }

  next::sdk::types::IPackage::PackageMetaType GetMetaType() override {
    if (!processor_required_) {
      return next::sdk::types::IPackage::PACKAGE_META_TYPE_INFO;
    }
    if (ProcessorPayloadAvailable()) {
      return next::sdk::types::IPackage::PACKAGE_META_TYPE_NAME;
    }
    return next::sdk::types::IPackage::PACKAGE_META_TYPE_INFO;
  }

  void SetUp(mts::extensibility::package &package, size_t &size) override {
    if (processor_required_) {
      ProcessorSetUp(package, size);
    }

    auto *software_package_header = reinterpret_cast<const mts_ecu_software_header_t *>(package->data());
    if (software_package_header == nullptr) {
      return;
    }
    auto package_vaddr = software_package_header->address;

    bool valid_payload = SetupPackageInfo(package, package_vaddr);
    if (!valid_payload) {
      size = 0;
      package_available_ = false;
    }

    next::sdk::types::IPackage::PackageMetaInfo package_info;
    package_info.instance_number = package->instance_number;
    package_info.cycle_id = package->cycle_id;
    package_info.source_id = package->source_id;
    package_info.virtual_address = package_vaddr;
    package_info.method_id = 0;
    package_info.service_id = 0;
    size_t package_start_hash = next::sdk::types::PackageHash::hashMetaInfo(package_info);

    auto size_package = software_package_header->control_length.length;
    size = size_package;

    uint32_t size_sdl;
    if (!GetSize(package_start_hash, size_sdl)) {
      size_sdl = size_package;
    }
    size_sdl = adaptBytePaddingTo8Byte(size_sdl);

    // Package is smaller or equal to the sdl size -> send out all available data
    if (size_sdl >= size_package) {
      sw_payloads_.push_back({{size_package, package_payload_address_}, package_vaddr});
      package_setup = true;
    } else {
      sw_payloads_.push_back({{size_sdl, package_payload_address_}, package_vaddr});
      SetupPackageGroups(package_start_hash, package_vaddr);
    }
    package_setup = true;
  }

  next::sdk::types::IPackage::PackageMetaInfo GetMetaInfo(mts::extensibility::package &package, size_t &) override {
    next::sdk::types::IPackage::PackageMetaInfo package_info;
    package_info.instance_number = package->instance_number;
    package_info.cycle_id = package->cycle_id;
    package_info.source_id = package->source_id;
    package_info.method_id = 0;
    package_info.service_id = 0;

    // package was set up
    if (PackageIsSetup()) {
      ExtractVaddrFromList(package_info);
      return package_info;
    }
    return package_info;
  }

  next::sdk::types::IPackage::Payload GetPayload(const mts::extensibility::package &package) override {
    if (processor_required_) {
      if (ProcessorPayloadAvailable()) {
        return ProcessorPackage::ProcessorGetPackagePayload(package);
      }
    }
    next::sdk::types::IPackage::Payload mem{0, nullptr};
    if (current_index_ < sw_payloads_.size()) {
      size_t temp_index = current_index_;
      current_index_++;
      return sw_payloads_[temp_index].payload;
    }
    return mem;
  }

  bool PayloadAvailable() override {
    if (processor_required_) {
      if (ProcessorPayloadAvailable()) {
        return true;
      }
    }
    return current_index_ < sw_payloads_.size();
  }

  bool ProcessorRequired() override {
    if (processor_required_) {
      if (ProcessorPayloadAvailable()) {
        return true;
      }
    }
    return false;
  }

  bool ResetPayload() override {
    if (processor_required_) {
      ProcessorResetPayload();
    }
    current_index_ = 0;
    return true;
  }

private:
  const std::unordered_map<size_t, uint32_t> &hash_to_size_map_;
  const std::unordered_map<size_t, uint64_t> &hash_to_next_vaddr_map_;

  const uint8_t *package_payload_address_{};
  size_t package_total_size_ = 0;
  uint16_t package_source_id = 0;
  uint32_t package_instance_number = 0;
  uint32_t package_cycle_id = 0;

  struct SubGroup {
    next::sdk::types::IPackage::Payload payload;
    size_t virtual_address;
  };
  std::vector<SubGroup> sw_payloads_;
  size_t current_index_ = 0;
  bool package_setup = false;

private:
  uint32_t adaptBytePaddingTo8Byte(uint32_t size_from_sdl) {
    uint32_t result = size_from_sdl % 8;
    if (result != 0) {
      // TODO: check if need to align the size from the sdl to 8 bytes
      return size_from_sdl + 8 - result;
    }
    return size_from_sdl;
  }

  bool SetupPackageInfo(mts::extensibility::package &package, uint64_t vaddr) {

    // setupPackage
    try {
      if ((package->cycle_state & mts::extensibility::cycle_body) != 0 ||
          (package->cycle_state == mts::extensibility::no_cycle)) {
        auto software_package_header =
            mts::toolbox::ecu::get_software_package_header<mts::extensibility::package>(&package);
        if (std::get<0>(software_package_header) != nullptr) {
          std::tuple<const uint8_t *, std::size_t> resolved_tuple =
              mts::toolbox::ecu::get_payload<mts::extensibility::package>(&package);
          package_payload_address_ = std::get<0>(resolved_tuple);
          package_total_size_ = std::get<1>(resolved_tuple);
        }
      } else {
        package_payload_address_ = nullptr;
        package_total_size_ = 0;
      }
    } catch (std::exception &e) {
      if (std::string("Data not available.") != e.what()) {
        error(__FILE__, "reading MTA SW package failed: c_id:{0}, s_id{1}, i_nr{2}, vaddr:{3} error:{4}",
              package->cycle_id, package->source_id, package->instance_number, vaddr, e.what());
      }
    }
    package_source_id = package->source_id;
    package_instance_number = package->instance_number;
    package_cycle_id = package->cycle_id;
    return package_total_size_ != 0;
  }

  bool GetNextVaddr(size_t hash, size_t &next_vaddr) {
    auto find_it = hash_to_next_vaddr_map_.find(hash);
    if (find_it != hash_to_next_vaddr_map_.end()) {
      next_vaddr = find_it->second;
      return true;
    } else {
      next_vaddr = 0;
      return false;
    }
  }
  bool GetSize(size_t hash, uint32_t &size) {
    auto find_it = hash_to_size_map_.find(hash);
    if (find_it != hash_to_size_map_.end()) {
      size = find_it->second;
      return true;
    } else {
      size = 0;
      return false;
    }
  }

  void correctLastPayload() {
    if (sw_payloads_.size()) {
      size_t last_index = sw_payloads_.size() - 1;
      sw_payloads_[last_index].payload.size = package_total_size_;
    }
  }

  bool HandleNextVaddrIsPackageAddress(size_t address_difference) {
    if (address_difference == 0) {
      correctLastPayload();
      return false;
    }
    return true;
  }

  void HandleSizeIfSinglePayload(size_t size) {
    if (sw_payloads_.size() == 1) {
      sw_payloads_[0].payload.size = size;
    }
  }

  void SetupPackageGroups(size_t package_start_hash, size_t package_vaddr) {
    size_t current_hash = package_start_hash;
    do {
      uint64_t next_vaddr = 0;
      if (!GetNextVaddr(current_hash, next_vaddr)) {
        break;
      }

      uint64_t diff = next_vaddr - package_vaddr;
      if (!HandleNextVaddrIsPackageAddress(diff)) {
        break;
      }

      if (diff >= package_total_size_) {
        HandleSizeIfSinglePayload(package_total_size_);
        break;
      }

      current_hash = next::sdk::types::PackageHash::hashMetaInfo(
          {package_source_id, package_instance_number, package_cycle_id, next_vaddr, 0, 0});

      uint32_t hash_size = 0;
      if (!GetSize(current_hash, hash_size)) {
        break;
      }

      sw_payloads_.push_back({{hash_size, package_payload_address_ + diff}, next_vaddr});

    } while (1);
  }

  inline bool PackageIsSetup() { return package_setup; }
  inline void ExtractVaddrFromList(next::sdk::types::IPackage::PackageMetaInfo &info) {
    if (current_index_ < sw_payloads_.size()) {
      info.virtual_address = sw_payloads_[current_index_].virtual_address;
    } else if (sw_payloads_.size() != 0) {
      info.virtual_address = sw_payloads_[0].virtual_address;
    } else {
      info.virtual_address = 0;
    }
  }
};

#endif // NEXT_MTASW_PACKAGE_HELPER_H

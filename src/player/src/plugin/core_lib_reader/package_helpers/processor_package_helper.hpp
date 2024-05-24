/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */

#ifndef NEXT_PROCESSOR_PACKAGE_HELPER_HPP
#define NEXT_PROCESSOR_PACKAGE_HELPER_HPP

#include <next/sdk/types/package_data_types.hpp>

#include "../core_lib_datatypes.h"
const std::string default_package_name_ = "undefined_package";

class ProcessorPackage {
public:
  ProcessorPackage() : processor_required_(false) {}
  virtual ~ProcessorPackage(){};

  bool CheckProcessorRequired(uint16_t source_id, uint32_t instance_number, uint32_t cycle_id, uint64_t vaddr,
                              const std::unordered_map<size_t, CoreLibProcessor> &proc_info) {
    auto hash = next::sdk::types::PackageHash::hashMetaInfo({source_id, instance_number, cycle_id, vaddr, 0, 0});
    auto found_proc = proc_info.find(hash);
    if (found_proc != proc_info.end()) {
      processor_required_ = true;
      processor_info_ = (CoreLibProcessor *)&(found_proc->second);
    }
    return processor_required_;
  }

  void ProcessorSetUp(mts::extensibility::package &package, size_t &size) {
    size = package->size;
    package_payload_address_ = reinterpret_cast<const uint8_t *>(package->data());

    if (processor_required_ && (processor_info_ != nullptr)) { // default hardware package
      for (const auto &info : processor_info_->publish_urls) {
        payloads_.push_back({{size, package_payload_address_}, info});
      }
    }
  }

  next::sdk::types::IPackage::Payload ProcessorGetPackagePayload(const mts::extensibility::package &) {
    next::sdk::types::IPackage::Payload mem{0, nullptr};
    if (current_index_ < payloads_.size()) {
      size_t temp_index = current_index_;
      current_index_++;
      return payloads_[temp_index].payload;
    }
    return mem;
  }

  bool ProcessorPayloadAvailable() { return current_index_ < payloads_.size(); }

  inline const std::string &ProcessorGetName() {
    if (current_index_ < payloads_.size()) {
      return payloads_[current_index_].package_name;
    } else {
      return default_package_name_;
    }
  }

  bool ProcessorResetPayload() {
    current_index_ = 0;
    return true;
  }

protected:
  bool processor_required_ = false;

private:
  const uint8_t *package_payload_address_{nullptr};

  struct SubGroup {
    next::sdk::types::IPackage::Payload payload;
    std::string package_name;
  };
  std::vector<SubGroup> payloads_;
  size_t current_index_ = 0;

  CoreLibProcessor *processor_info_;
};

#endif // NEXT_PROCESSOR_PACKAGE_HELPER_HPP

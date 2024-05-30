/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
#ifndef NEXT_ETHERNET_PACKAGE_HELPER_H
#define NEXT_ETHERNET_PACKAGE_HELPER_H

#include "next/sdk/types/package_data_types.hpp"
#include "next/sdk/types/package_hash.hpp"

#include <mts/extensibility/package.hpp>
#include <mts/toolbox/ethernet/ethernet.hpp>
using namespace mts::toolbox::ethernet;

#include "../core_lib_datatypes.h"
class EthernetPackage {
public:
  struct serialization_info {
    serialization_technology serialization_technology_type;
    uint8_t interface_version;
    uint32_t protocol_version;
  };
  using serialization_info_map = std::unordered_map<size_t, serialization_info>;
  EthernetPackage(const serialization_info_map &service_info) : service_info_(service_info) {}
  next::sdk::types::IPackage::PackageMetaInfo CreateMetaInfo(mts::extensibility::package &package, size_t &size) {
    next::sdk::types::IPackage::PackageMetaInfo first_package_info;
    first_package_info.instance_number = package->instance_number;
    first_package_info.source_id = package->source_id;
    first_package_info.cycle_id = 0;
    first_package_info.virtual_address = 0;

    // package was set up
    if (PackageIsSetup()) {
      ExtractServiceIDAndMethodIDFromList(first_package_info);
      return first_package_info;
    }

    SetupPackageInfo(package);

    auto payload = package_payload_address_;
    auto remaining_payload_size = package_total_size_;
    size = package_total_size_;
    const mts_ethernet_autosar_soad_header_t *header = nullptr;

    // Traverse each application package (SOMEIP or AUTOSAR package) and get the required info
    while (remaining_payload_size >= sizeof(mts_ethernet_autosar_soad_header_t)) {
      bool unsupported_protocol = false;
      /* Get the service ID and method ID
       * mts_autosar_soad_header_t is considered here since all the required fields present in it and it is similar
       * to SOME-IP header except some fields at the end.
       */
      header = reinterpret_cast<const mts_ethernet_autosar_soad_header_t *>(payload);
      auto length = mts::runtime::endian_reverse<uint32_t>(header->length);
      if (length > remaining_payload_size)
        break;
      uint16_t service_id = mts::runtime::endian_reverse<uint16_t>(header->service_id);
      uint32_t method_id = mts::runtime::endian_reverse<uint16_t>(header->method_id);
      auto itr = service_info_.find(static_cast<uint16_t>(service_id));
      if (itr != service_info_.end()) {
        switch (itr->second.serialization_technology_type) {
        case serialization_technology::autosar: {
          remaining_payload_size -= sizeof(mts_ethernet_autosar_soad_header_t);
          payload += sizeof(mts_ethernet_autosar_soad_header_t);
        } break;
        case serialization_technology::some_ip: {
          auto *some_ip_header = reinterpret_cast<const mts_ethernet_some_ip_header_t *>(payload);
          if (some_ip_header->interface_version != itr->second.interface_version &&
              some_ip_header->protocol_version != itr->second.protocol_version) {
            unsupported_protocol = true;
            break;
          }
          remaining_payload_size -= sizeof(mts_ethernet_some_ip_header_t);
          payload += sizeof(mts_ethernet_some_ip_header_t);
          length -= 8; /*subtract 8 bytes which are part of SOME/IP header*/
        } break;
        default:
          unsupported_protocol = true;
          break; // Not Supported
        }
        if (unsupported_protocol)
          break;
      } else {
        break;
      }
      auto current_hash = next::sdk::types::PackageHash::hashMetaInfo(
          {package_source_id_, package_instance_number_, 0, 0, service_id, method_id});
      payloads_.push_back({{length, payload}, service_id, method_id, current_hash});
      remaining_payload_size -= length;
      payload += length;
    }

    ExtractServiceIDAndMethodIDFromList(first_package_info);
    package_setup = true;
    return first_package_info;
  }

  next::sdk::types::IPackage::Payload GetPackagePayload(const mts::extensibility::package &) {
    next::sdk::types::IPackage::Payload mem{0, nullptr};
    if (current_index_ < payloads_.size()) {
      size_t temp_index = current_index_;
      current_index_++;
      return payloads_[temp_index].payload;
    }
    return mem;
  }

  bool ResetPayload() {
    current_index_ = 0;
    return true;
  }

  bool PayloadAvailable() { return current_index_ < payloads_.size(); }

  static bool HasPayload(mts::extensibility::package &package) {
    try {
      auto has_payload = mts::toolbox::ethernet::has_payload<mts::extensibility::package>(&package);
      return has_payload;
    } catch (std::exception &e) {
      error("Exception thrown while validating Ethernet package: {}", e.what());
    }
    return false;
  }

private:
  const serialization_info_map &service_info_;

  const uint8_t *package_payload_address_;
  size_t package_total_size_ = 0;
  uint16_t package_source_id_ = 0;
  uint32_t package_instance_number_ = 0;

  struct SubGroup {
    next::sdk::types::IPackage::Payload payload;
    uint16_t service_id;
    uint32_t methoid_id;
    size_t hash;
  };
  std::vector<SubGroup> payloads_;
  size_t current_index_ = 0;
  bool package_setup = false;

private:
  void SetupPackageInfo(mts::extensibility::package &package) {
    // setupPackage
    try {
      auto [payload, payload_length] =
          mts::toolbox::ethernet::get_protocol_payload<mts::extensibility::package>(&package);
      package_payload_address_ = payload;
      package_total_size_ = payload_length;
    } catch (std::exception &e) {
      error("Exception thrown while buffering recording packages: {}", e.what());
    }
    package_source_id_ = package->source_id;
    package_instance_number_ = package->instance_number;
  }

  inline bool PackageIsSetup() { return package_setup; }

  inline void ExtractServiceIDAndMethodIDFromList(next::sdk::types::IPackage::PackageMetaInfo &info) {
    if (current_index_ < payloads_.size()) {
      info.service_id = payloads_[current_index_].service_id;
      info.method_id = payloads_[current_index_].methoid_id;
    } else if (payloads_.size() != 0) {
      info.service_id = payloads_[0].service_id;
      info.method_id = payloads_[0].methoid_id;
    } else {
      info.service_id = 0;
      info.method_id = 0;
    }
  }
};

#endif // NEXT_ETHERNET_PACKAGE_HELPER_H

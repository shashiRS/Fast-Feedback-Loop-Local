/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
#ifndef NEXT_CORE_LIB_PACKAGE_H
#define NEXT_CORE_LIB_PACKAGE_H

#include <mts/extensibility/package.hpp>
#include <mts/extensibility/raw_package.hpp>

#include <next/sdk/sdk.hpp>
#include <next/sdk/types/package_data_types.hpp>

#include "core_lib_datatypes.h"
#include "package_helpers/can_package_helper.h"
#include "package_helpers/default_package_helper.h"
#include "package_helpers/ethernet_package_helper.h"
#include "package_helpers/flexray_package_helper.h"
#include "package_helpers/gps_package_helper.h"
#include "package_helpers/mtahw_package_helper.h"
#include "package_helpers/mtasw_package_helper.h"
#include "package_helpers/rtrange_package_helper.h"

static inline std::unordered_map<size_t, uint32_t> dummy_size_map;
static inline std::unordered_map<size_t, uint64_t> dummy_vaddr_map;
static inline EthernetPackage::serialization_info_map dummy_ethernet_service_info_map;
static inline std::unordered_map<size_t, CoreLibProcessor> dummy_processor_map;

class CoreLibPackage : public next::sdk::types::IPackage {
public:
  CoreLibPackage() = delete;

  // constructor for testing purpose
  CoreLibPackage(mts::extensibility::package &&pkg)
      : next::sdk::types::IPackage(
            std::move(next::sdk::types::takePackageOwnership<mts::extensibility::package>(pkg, [](void *) {}))),
        size_(0), package_(std::move(pkg)), hash_to_size_map_(dummy_size_map), hash_to_prev_vaddr_map_(dummy_vaddr_map),
        ethernet_service_info_(dummy_ethernet_service_info_map), processor_map_(dummy_processor_map) {
    SetupPackage();
  }

  // actual constructor
  CoreLibPackage(mts::extensibility::package &&pkg, const std::unordered_map<size_t, uint32_t> &hash_to_size_map,
                 const std::unordered_map<size_t, uint64_t> &has_to_next_vaddr_map,
                 EthernetPackage::serialization_info_map &ethernet_service_info,
                 const std::unordered_map<size_t, CoreLibProcessor> &processor_map)
      : next::sdk::types::IPackage(
            std::move(next::sdk::types::takePackageOwnership<mts::extensibility::package>(pkg, [](void *) {}))),
        size_(0), package_(std::move(pkg)), hash_to_size_map_(hash_to_size_map),
        hash_to_prev_vaddr_map_(has_to_next_vaddr_map), ethernet_service_info_(ethernet_service_info),
        processor_map_(processor_map) {
    SetupPackage();
  }

  // constructor for testing purpose
  CoreLibPackage(mts::extensibility::package &&pkg, EthernetPackage::serialization_info_map &ethernet_service_info)
      : next::sdk::types::IPackage(
            std::move(next::sdk::types::takePackageOwnership<mts::extensibility::package>(pkg, [](void *) {}))),
        size_(0), package_(std::move(pkg)), hash_to_size_map_(dummy_size_map), hash_to_prev_vaddr_map_(dummy_vaddr_map),
        ethernet_service_info_(ethernet_service_info), processor_map_(dummy_processor_map) {
    SetupPackage();
  }

  // constructor for testing purpose
  CoreLibPackage(mts::extensibility::package &&pkg, const std::unordered_map<size_t, uint32_t> &hash_to_size_map,
                 const std::unordered_map<size_t, uint64_t> &has_to_next_vaddr_map)
      : next::sdk::types::IPackage(
            std::move(next::sdk::types::takePackageOwnership<mts::extensibility::package>(pkg, [](void *) {}))),
        size_(0), package_(std::move(pkg)), hash_to_size_map_(hash_to_size_map),
        hash_to_prev_vaddr_map_(has_to_next_vaddr_map), ethernet_service_info_(dummy_ethernet_service_info_map),
        processor_map_(dummy_processor_map) {
    SetupPackage();
  }

  virtual ~CoreLibPackage() {}

  bool operator<(const CoreLibPackage &rhs) const {
    if (GetCorePackage()->reference_timestamp < rhs.GetCorePackage()->reference_timestamp)
      return true;
    return false;
  }

  void SetupPackage() {
    next::sdk::types::PackageFormatType format = next::sdk::types::getPackageFormatTypeEnum(package_->format_type);

    header_.timestamp = get_timestamp();
    header_.cycle_state = get_cycle_state();
    header_.cycle_id = get_cycle();
    header_.format = format;
    package_name_ = "";

    format_ = format;
    type_ = PACKAGE_META_TYPE_INFO;
    if (format == next::sdk::types::PACKAGE_FORMAT_TYPE_MTA_SW) {
      mta_package_ =
          std::make_shared<MtaSwPackage>(hash_to_size_map_, hash_to_prev_vaddr_map_, processor_map_, package_);
      processor_required_ = mta_package_->ProcessorRequired();
      mta_package_->SetUp(package_, size_);
      type_ = mta_package_->GetMetaType();
      package_valid_ = true;
    } else if (format == next::sdk::types::PACKAGE_FORMAT_TYPE_MTA_HW) {
      mta_hw_package_ =
          std::make_shared<MtaHwPackage>(get_source(), package_->instance_number, header_.cycle_id, processor_map_);
      processor_required_ = mta_hw_package_->ProcessorRequired();
      mta_hw_package_->SetUp(package_, size_);
      type_ = mta_hw_package_->GetMetaType();
      package_valid_ = true;
    } else if (format == next::sdk::types::PACKAGE_FORMAT_TYPE_CAN) {
      info_ = CanPackage::CreateMetaInfo(package_, size_);
      package_valid_ = true;
    } else if (format == next::sdk::types::PACKAGE_FORMAT_TYPE_REFCAM) {
      info_ = DefaultPackage::CreateMetaInfo(package_, size_);
      package_valid_ = true;
    } else if (format == next::sdk::types::PACKAGE_FORMAT_TYPE_GPS_GENERIC) {
      info_ = GpsPackage::CreateMetaInfo(package_, size_);
      package_valid_ = true;
    } else if (format == next::sdk::types::PACKAGE_FORMAT_TYPE_RTRAGE) {
      info_ = RtrangePackage::CreateMetaInfo(package_, size_);
      package_valid_ = true;
    } else if (format == next::sdk::types::PACKAGE_FORMAT_TYPE_ETH) {
      // Check if this package contains Ethernet based measurement data
      if (EthernetPackage::HasPayload(package_)) {
        ethernet_package_ = std::make_shared<EthernetPackage>(ethernet_service_info_);
        package_valid_ = true;
      } else {
        // Discard any non-measurement related Ethernet data traffic
        package_valid_ = false;
      }
    } else if (format == next::sdk::types::PACKAGE_FORMAT_TYPE_FLEXRAY) {
      info_ = FlexrayPackage::CreateMetaInfo(package_, size_);
      package_valid_ = true;
    } else {
      package_valid_ = false;
    }
    package_available_ = true;
  }

  bool PackageValid() { return package_valid_; }

public:
  virtual bool PayloadAvailable() override {
    if (format_ == next::sdk::types::PACKAGE_FORMAT_TYPE_MTA_SW) {
      if (mta_package_) {
        return mta_package_->PayloadAvailable();
      } else {
        return false;
      }
    } else if (format_ == next::sdk::types::PACKAGE_FORMAT_TYPE_MTA_HW) {
      if (mta_hw_package_) {
        return mta_hw_package_->PayloadAvailable();
      } else {
        return false;
      }
    } else if (format_ == next::sdk::types::PACKAGE_FORMAT_TYPE_ETH) {
      if (ethernet_package_) {
        info_ = ethernet_package_->CreateMetaInfo(package_, size_);
        return ethernet_package_->PayloadAvailable();
      } else {
        return false;
      }
    } else {
      return DefaultPackage::PayloadAvailable(package_available_);
    }
  }

  virtual const Payload GetPayload() override {
    Payload mem{0, nullptr};

    if (format_ == next::sdk::types::PACKAGE_FORMAT_TYPE_MTA_SW) {
      if (mta_package_) {
        processor_required_ = mta_package_->ProcessorRequired();
        mem = mta_package_->GetPayload(GetCorePackage());
      }
    } else if (format_ == next::sdk::types::PACKAGE_FORMAT_TYPE_MTA_HW) {
      if (mta_hw_package_) {
        mem = mta_hw_package_->GetPayload(GetCorePackage());
      }
    } else if (format_ == next::sdk::types::PACKAGE_FORMAT_TYPE_CAN) {
      mem = CanPackage::GetPackagePayload(GetCorePackage());
    } else if (format_ == next::sdk::types::PACKAGE_FORMAT_TYPE_REFCAM) {
      mem = DefaultPackage::GetPackagePayload(GetCorePackage());
    } else if (format_ == next::sdk::types::PACKAGE_FORMAT_TYPE_GPS_GENERIC) {
      mem = GpsPackage::GetPackagePayload(GetCorePackage());
    } else if (format_ == next::sdk::types::PACKAGE_FORMAT_TYPE_RTRAGE) {
      mem = RtrangePackage::GetPackagePayload(GetCorePackage());
    } else if (format_ == next::sdk::types::PACKAGE_FORMAT_TYPE_ETH) {
      if (ethernet_package_) {
        mem = ethernet_package_->GetPackagePayload(GetCorePackage());
      }
    } else if (format_ == next::sdk::types::PACKAGE_FORMAT_TYPE_FLEXRAY) {
      mem = FlexrayPackage::GetPackagePayload(GetCorePackage());
    } else {
    }
    return mem;
  }
  virtual size_t GetSize() override { return size_; }

  virtual bool ResetPayloads() override {
    if (format_ == next::sdk::types::PACKAGE_FORMAT_TYPE_MTA_SW) {
      if (mta_package_) {
        return mta_package_->ResetPayload();
      }
      return true;
    } else if (format_ == next::sdk::types::PACKAGE_FORMAT_TYPE_MTA_HW) {
      if (mta_hw_package_) {
        mta_hw_package_->ResetPayload();
      }
      return true;
    } else if (format_ == next::sdk::types::PACKAGE_FORMAT_TYPE_ETH) {
      if (ethernet_package_) {
        ethernet_package_->ResetPayload();
      }
      return true;
    } else {
      return DefaultPackage::ResetPayload(package_available_);
    }
  }

  virtual PackageMetaType GetMetaType() const override {
    if (format_ == next::sdk::types::PACKAGE_FORMAT_TYPE_MTA_SW) {
      if (mta_package_) {
        return mta_package_->GetMetaType();
      }
    } else if (format_ == next::sdk::types::PACKAGE_FORMAT_TYPE_MTA_HW) {
      if (mta_hw_package_) {
        return mta_hw_package_->GetMetaType();
      }
    }
    return type_;
  }

  virtual const PackageMetaInfo &GetMetaInfo() override {
    if (format_ == next::sdk::types::PACKAGE_FORMAT_TYPE_MTA_SW) {
      if (mta_package_) {
        info_ = mta_package_->GetMetaInfo(package_, size_);
      }
    } else if (format_ == next::sdk::types::PACKAGE_FORMAT_TYPE_MTA_HW) {
      if (mta_hw_package_) {
        info_ = mta_hw_package_->GetMetaInfo(package_, size_);
      }
    } else if (format_ == next::sdk::types::PACKAGE_FORMAT_TYPE_ETH) {
      if (ethernet_package_) {
        info_ = ethernet_package_->CreateMetaInfo(package_, size_);
      }
    }
    return info_;
  }

  virtual const PackageHeader &GetPackageHeader() override { return header_; }

  virtual const std::string &GetPackageName() const override {
    if (format_ == next::sdk::types::PACKAGE_FORMAT_TYPE_MTA_HW) {
      if (mta_hw_package_) {
        return mta_hw_package_->GetName();
      }
    } else if (format_ == next::sdk::types::PACKAGE_FORMAT_TYPE_MTA_SW) {
      if (mta_package_) {
        return mta_package_->GetName();
      }
    }
    return package_name_;
  }

  virtual size_t GetPackageVaddr() const override { return 0; }

private:
  uint64_t get_timestamp() const { return package_->reference_timestamp; }
  uint16_t get_source() const { return package_->source_id; }
  uint32_t get_cycle() const { return package_->cycle_id; }
  uint8_t get_cycle_state() const { return package_->cycle_state; }

  mts::extensibility::package &GetCorePackage() const { return package_; }

  size_t size_;
  PackageMetaType type_;
  mutable mts::extensibility::package package_;
  const std::unordered_map<size_t, uint32_t> &hash_to_size_map_;
  const std::unordered_map<size_t, uint64_t> &hash_to_prev_vaddr_map_;
  const EthernetPackage::serialization_info_map &ethernet_service_info_;
  const std::unordered_map<size_t, CoreLibProcessor> &processor_map_;

  std::shared_ptr<MtaSwPackage> mta_package_;
  std::shared_ptr<EthernetPackage> ethernet_package_;
  std::shared_ptr<MtaHwPackage> mta_hw_package_;

  next::sdk::types::PackageFormatType format_ = next::sdk::types::PACKAGE_FORMAT_TYPE_UNKNOWN;
  bool package_available_ = false;
  bool package_valid_ = false;
  PackageMetaInfo info_ = {};
  PackageHeader header_;
  std::string package_name_ = "";
};

#endif // NEXT_CORE_LIB_PACKAGE_H

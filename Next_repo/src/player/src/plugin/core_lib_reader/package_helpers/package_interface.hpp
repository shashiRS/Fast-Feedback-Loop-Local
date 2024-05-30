/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
#ifndef NEXT_PACKAGE_INTERFACE_HPP
#define NEXT_PACKAGE_INTERFACE_HPP

#include "processor_package_helper.hpp"

class IPackageHelper : public ProcessorPackage {
public:
  IPackageHelper() : ProcessorPackage(){};
  virtual ~IPackageHelper(){};

  virtual next::sdk::types::IPackage::PackageMetaInfo GetMetaInfo(mts::extensibility::package &package,
                                                                  size_t &size) = 0;
  virtual next::sdk::types::IPackage::Payload GetPayload(const mts::extensibility::package &package) = 0;

  virtual void SetUp(mts::extensibility::package &package, size_t &size) = 0;

  virtual next::sdk::types::IPackage::PackageMetaType GetMetaType() {
    return processor_required_ ? next::sdk::types::IPackage::PACKAGE_META_TYPE_NAME
                               : next::sdk::types::IPackage::PACKAGE_META_TYPE_INFO;
  }

  virtual bool PayloadAvailable() {
    if (processor_required_) {
      return ProcessorPayloadAvailable();
    } else {
      return package_available_;
    }
  }
  virtual bool ProcessorRequired() { return processor_required_; }
  virtual bool ResetPayload() {
    if (processor_required_) {
      ProcessorResetPayload();
    } else {
      package_available_ = true;
    }
    return true;
  };

  virtual inline const std::string &GetName() { return ProcessorGetName(); };

protected:
  bool package_available_ = false;
};

#endif // NEXT_PACKAGE_INTERFACE_HPP

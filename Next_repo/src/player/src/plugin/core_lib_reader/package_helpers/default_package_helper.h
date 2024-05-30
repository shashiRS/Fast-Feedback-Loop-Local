/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
#ifndef NEXT_DEFAULT_PACKAGE_HELPER_H
#define NEXT_DEFAULT_PACKAGE_HELPER_H

#include "next/sdk/types/package_data_types.hpp"

#include <mts/extensibility/package.hpp>

#include "../core_lib_datatypes.h"

class DefaultPackage {
public:
  static next::sdk::types::IPackage::PackageMetaInfo CreateMetaInfo(mts::extensibility::package &package,
                                                                    size_t &size) {
    next::sdk::types::IPackage::PackageMetaInfo info;
    info.instance_number = package->instance_number;
    info.source_id = package->source_id;
    info.cycle_id = package->cycle_id;
    info.virtual_address = 0;
    size = package->size;
    return info;
  }
  static next::sdk::types::IPackage::Payload GetPackagePayload(const mts::extensibility::package &package) {
    next::sdk::types::IPackage::Payload mem{0, nullptr};
    mem.mem_pointer = (const unsigned char *)package->data();
    mem.size = package->size;
    return mem;
  }
  static bool PayloadAvailable(bool &package_available) {
    if (package_available) {
      package_available = false;
      return true;
    } else {
      return false;
    }
  }

  static bool ResetPayload(bool &package_available) { return package_available = true; }
};
#endif // NEXT_DEFAULT_PACKAGE_HELPER_H

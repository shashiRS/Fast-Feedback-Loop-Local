/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     version_info.h
 * @brief    Implementation for event subscriber
 *
 **/
#ifndef NEXTSDK_VERSION_INFO_H_
#define NEXTSDK_VERSION_INFO_H_

#include <string.h>
#include <atomic>
#include <map>
#include <memory>

#include "../event_base/base_event_binary.h"
#include "../sdk_config.hpp"

namespace next {
namespace sdk {
namespace events {

struct VersionInfoPackage {
  std::string component_name;
  std::string documentation_link;
  std::string component_version;
};

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4251)
#endif
class DECLSPEC_nextsdk VersionInfo : public BaseEventBinary<VersionInfoPackage> {
#if defined(_MSC_VER)
#pragma warning(pop)
#endif

public:
  VersionInfo(std::string name) : BaseEventBinary<VersionInfoPackage>(name) {}

protected:
  virtual payload_type serializeEventPayload(const VersionInfoPackage &message) const override;
  virtual VersionInfoPackage deserializeEventPayload(const payload_type &buf) const override;
};

} // namespace events
} // namespace sdk
} // namespace next

#endif // NEXTSDK_VERSION_INFO_H_

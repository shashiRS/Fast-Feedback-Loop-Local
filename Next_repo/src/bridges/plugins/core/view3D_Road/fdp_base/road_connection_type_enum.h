// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_ROAD_CONNECTION_TYPE_ENUM_H_
#define FDP_BASE_ROAD_CONNECTION_TYPE_ENUM_H_

#include "eco/deprecation.h"

#include "Platform_Types.h"

// PRQA S 2180 ++
/* date: 2020-03-31, reviewer: PR reviewers of https://github-am.geo.conti.de/ADAS/eco/pull/1100, CCBIssueId:
 * https://jira-adas.zone2.agileci.conti.de/browse/SEP-1137, reason: implicit construction is desired here */

namespace fdp_base {
/// \cond HIDDEN_ENUMS
class roadConnectionTypeEnum {
public:
  typedef uint16 underlying_type;
  enum do_not_use {
    RMF_RCT_CONSTRUCTION_START = 1U,
    RMF_RCT_CONSTRUCTION_END = 2U,
    RMF_RCT_CONSTRUCTION_CONTINUED = 4U,
    RMF_RCT_TUNNEL_START = 8U,
    RMF_RCT_TUNNEL_END = 16U,
    RMF_RCT_TUNNEL_CONTINUED = 32U,
    RMF_RCT_ROUNDABOUT_START = 64U,
    RMF_RCT_ROUNDABOUT_END = 128U,
    RMF_RCT_ROUNDABOUT_CONTINUED = 256U,
    RMF_RCT_CROSSING = 512U,
    RMF_RCT_TOLLBOOTH = 1024U,
    RMF_RCT_ATTRIBUTE_CHANGE = 2048U
  };

  roadConnectionTypeEnum() : value(static_cast<underlying_type>(RMF_RCT_CONSTRUCTION_START)) {}

  roadConnectionTypeEnum(const do_not_use v) : value(static_cast<const underlying_type>(v)) {}

  roadConnectionTypeEnum(const roadConnectionTypeEnum &v) : value(static_cast<const underlying_type>(v)) {}

  DEPRECATED_BEGIN DEPRECATED_END roadConnectionTypeEnum(const underlying_type v) : value(v) {}

  operator underlying_type() const { return value; }

  roadConnectionTypeEnum &operator|=(const do_not_use v) {
    value |= static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(roadConnectionTypeEnum &) operator|=(const underlying_type v) {
    value |= v;
    return *this;
  }

  roadConnectionTypeEnum &operator=(const do_not_use v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  roadConnectionTypeEnum &operator=(const roadConnectionTypeEnum &v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(roadConnectionTypeEnum &) operator=(const underlying_type v) {
    value = v;
    return *this;
  }

  template <typename T>
  bool operator==(const T v) const {
    return value == static_cast<const underlying_type>(v);
  }
  /// \endcond

private:
  underlying_type value;
};
} // namespace fdp_base

// PRQA S 2180 --

#endif // FDP_BASE_ROAD_CONNECTION_TYPE_ENUM_H_

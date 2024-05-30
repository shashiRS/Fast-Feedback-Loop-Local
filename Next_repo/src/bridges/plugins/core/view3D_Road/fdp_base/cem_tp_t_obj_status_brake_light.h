// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_TP_T_OBJ_STATUS_BRAKE_LIGHT_H_
#define FDP_BASE_CEM_TP_T_OBJ_STATUS_BRAKE_LIGHT_H_

#include "eco/deprecation.h"

#include "Platform_Types.h"

// PRQA S 2180 ++
/* date: 2020-03-31, reviewer: PR reviewers of https://github-am.geo.conti.de/ADAS/eco/pull/1100, CCBIssueId:
 * https://jira-adas.zone2.agileci.conti.de/browse/SEP-1137, reason: implicit construction is desired here */

namespace fdp_base {
/// \cond HIDDEN_ENUMS
/// Possible states of brake light detection values: enum { TP_ADD_ATTR_OBJ_BRAKE_LIGHT_UNKNOWN=0
/// ,TP_ADD_ATTR_OBJ_BRAKE_LIGHT_NO_BRAKING=1,TP_ADD_ATTR_OBJ_BRAKE_LIGHT_REGULAR_BRAKING=2
/// ,TP_ADD_ATTR_OBJ_BRAKE_LIGHT_HEAVY_BREAKING=3,TP_ADD_ATTR_OBJ_BRAKE_LIGHT_SNA=255 ,}
/// @range{0,255}
class CEM_TP_t_ObjStatusBrakeLight {
public:
  typedef uint8 underlying_type;
  enum do_not_use {
    TP_ADD_ATTR_OBJ_BRAKE_LIGHT_UNKNOWN = 0U,
    TP_ADD_ATTR_OBJ_BRAKE_LIGHT_NO_BRAKING = 1U,
    TP_ADD_ATTR_OBJ_BRAKE_LIGHT_REGULAR_BRAKING = 2U,
    TP_ADD_ATTR_OBJ_BRAKE_LIGHT_HEAVY_BREAKING = 3U,
    TP_ADD_ATTR_OBJ_BRAKE_LIGHT_SNA = 255U
  };

  CEM_TP_t_ObjStatusBrakeLight() : value(static_cast<underlying_type>(TP_ADD_ATTR_OBJ_BRAKE_LIGHT_UNKNOWN)) {}

  CEM_TP_t_ObjStatusBrakeLight(const do_not_use v) : value(static_cast<const underlying_type>(v)) {}

  CEM_TP_t_ObjStatusBrakeLight(const CEM_TP_t_ObjStatusBrakeLight &v) : value(static_cast<const underlying_type>(v)) {}

  DEPRECATED_BEGIN DEPRECATED_END CEM_TP_t_ObjStatusBrakeLight(const underlying_type v) : value(v) {}

  operator underlying_type() const { return value; }

  CEM_TP_t_ObjStatusBrakeLight &operator|=(const do_not_use v) {
    value |= static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(CEM_TP_t_ObjStatusBrakeLight &) operator|=(const underlying_type v) {
    value |= v;
    return *this;
  }

  CEM_TP_t_ObjStatusBrakeLight &operator=(const do_not_use v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  CEM_TP_t_ObjStatusBrakeLight &operator=(const CEM_TP_t_ObjStatusBrakeLight &v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(CEM_TP_t_ObjStatusBrakeLight &) operator=(const underlying_type v) {
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

#endif // FDP_BASE_CEM_TP_T_OBJ_STATUS_BRAKE_LIGHT_H_

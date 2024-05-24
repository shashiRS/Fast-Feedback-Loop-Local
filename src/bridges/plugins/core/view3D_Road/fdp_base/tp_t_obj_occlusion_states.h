// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_TP_T_OBJ_OCCLUSION_STATES_H_
#define FDP_BASE_TP_T_OBJ_OCCLUSION_STATES_H_

#include "eco/deprecation.h"

#include "Platform_Types.h"

// PRQA S 2180 ++
/* date: 2020-03-31, reviewer: PR reviewers of https://github-am.geo.conti.de/ADAS/eco/pull/1100, CCBIssueId:
 * https://jira-adas.zone2.agileci.conti.de/browse/SEP-1137, reason: implicit construction is desired here */

namespace fdp_base {
/// \cond HIDDEN_ENUMS
/// Describes if/how Object is covered by closer objects in sensing direction. (TP_OBJ_ATTRIBUTE_OCCLUSION) values: enum
/// { TP_OBJECT_OCCL_NONE=0 ,TP_OBJECT_OCCL_UNKNOWN=1,TP_OBJECT_OCCL_PARTLY=2,TP_OBJECT_OCCL_FULL=3
/// ,TP_OBJECT_OCCL_MAX_DIFF_TYPES=4,}
/// @range{0,4}
class TP_t_ObjOcclusionStates {
public:
  typedef uint8 underlying_type;
  enum do_not_use {
    TP_OBJECT_OCCL_NONE = 0U,
    TP_OBJECT_OCCL_UNKNOWN = 1U,
    TP_OBJECT_OCCL_PARTLY = 2U,
    TP_OBJECT_OCCL_FULL = 3U,
    TP_OBJECT_OCCL_MAX_DIFF_TYPES = 4U
  };

  TP_t_ObjOcclusionStates() : value(static_cast<underlying_type>(TP_OBJECT_OCCL_NONE)) {}

  TP_t_ObjOcclusionStates(const do_not_use v) : value(static_cast<const underlying_type>(v)) {}

  TP_t_ObjOcclusionStates(const TP_t_ObjOcclusionStates &v) : value(static_cast<const underlying_type>(v)) {}

  DEPRECATED_BEGIN DEPRECATED_END TP_t_ObjOcclusionStates(const underlying_type v) : value(v) {}

  operator underlying_type() const { return value; }

  TP_t_ObjOcclusionStates &operator|=(const do_not_use v) {
    value |= static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(TP_t_ObjOcclusionStates &) operator|=(const underlying_type v) {
    value |= v;
    return *this;
  }

  TP_t_ObjOcclusionStates &operator=(const do_not_use v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  TP_t_ObjOcclusionStates &operator=(const TP_t_ObjOcclusionStates &v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(TP_t_ObjOcclusionStates &) operator=(const underlying_type v) {
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

#endif // FDP_BASE_TP_T_OBJ_OCCLUSION_STATES_H_

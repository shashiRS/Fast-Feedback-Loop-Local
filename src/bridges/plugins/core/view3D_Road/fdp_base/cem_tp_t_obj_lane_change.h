// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_TP_T_OBJ_LANE_CHANGE_H_
#define FDP_BASE_CEM_TP_T_OBJ_LANE_CHANGE_H_

#include "eco/deprecation.h"

#include "Platform_Types.h"

// PRQA S 2180 ++
/* date: 2020-03-31, reviewer: PR reviewers of https://github-am.geo.conti.de/ADAS/eco/pull/1100, CCBIssueId:
 * https://jira-adas.zone2.agileci.conti.de/browse/SEP-1137, reason: implicit construction is desired here */

namespace fdp_base {
/// \cond HIDDEN_ENUMS
/// Object lane change maneuver in respect to the objects associated lane. values: enum {
/// TP_ADD_ATTR_OBJ_LANE_CHANGE_NONE=0,TP_ADD_ATTR_OBJ_LANE_CHANGE_CUT_OUT_RIGHT=1
/// ,TP_ADD_ATTR_OBJ_LANE_CHANGE_CUT_OUT_LEFT=2,TP_ADD_ATTR_OBJ_LANE_CHANGE_CUT_IN_RIGHT=3
/// ,TP_ADD_ATTR_OBJ_LANE_CHANGE_CUT_IN_LEFT=4,}
/// @range{0,4}
class CEM_TP_t_ObjLaneChange {
public:
  typedef uint8 underlying_type;
  enum do_not_use {
    TP_ADD_ATTR_OBJ_LANE_CHANGE_NONE = 0U,
    TP_ADD_ATTR_OBJ_LANE_CHANGE_CUT_OUT_RIGHT = 1U,
    TP_ADD_ATTR_OBJ_LANE_CHANGE_CUT_OUT_LEFT = 2U,
    TP_ADD_ATTR_OBJ_LANE_CHANGE_CUT_IN_RIGHT = 3U,
    TP_ADD_ATTR_OBJ_LANE_CHANGE_CUT_IN_LEFT = 4U
  };

  CEM_TP_t_ObjLaneChange() : value(static_cast<underlying_type>(TP_ADD_ATTR_OBJ_LANE_CHANGE_NONE)) {}

  CEM_TP_t_ObjLaneChange(const do_not_use v) : value(static_cast<const underlying_type>(v)) {}

  CEM_TP_t_ObjLaneChange(const CEM_TP_t_ObjLaneChange &v) : value(static_cast<const underlying_type>(v)) {}

  DEPRECATED_BEGIN DEPRECATED_END CEM_TP_t_ObjLaneChange(const underlying_type v) : value(v) {}

  operator underlying_type() const { return value; }

  CEM_TP_t_ObjLaneChange &operator|=(const do_not_use v) {
    value |= static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(CEM_TP_t_ObjLaneChange &) operator|=(const underlying_type v) {
    value |= v;
    return *this;
  }

  CEM_TP_t_ObjLaneChange &operator=(const do_not_use v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  CEM_TP_t_ObjLaneChange &operator=(const CEM_TP_t_ObjLaneChange &v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(CEM_TP_t_ObjLaneChange &) operator=(const underlying_type v) {
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

#endif // FDP_BASE_CEM_TP_T_OBJ_LANE_CHANGE_H_

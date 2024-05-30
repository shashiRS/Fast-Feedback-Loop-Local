// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_TP_T_OBJ_ASSOCIATED_LANE_H_
#define FDP_BASE_CEM_TP_T_OBJ_ASSOCIATED_LANE_H_

#include "eco/deprecation.h"

#include "Platform_Types.h"

// PRQA S 2180 ++
/* date: 2020-03-31, reviewer: PR reviewers of https://github-am.geo.conti.de/ADAS/eco/pull/1100, CCBIssueId:
 * https://jira-adas.zone2.agileci.conti.de/browse/SEP-1137, reason: implicit construction is desired here */

namespace fdp_base {
/// \cond HIDDEN_ENUMS
/// Mapping of the object to a lane values: enum { TP_ADD_ATTR_OBJ_ASSOC_LANE_UNKNOWN=0
/// ,TP_ADD_ATTR_OBJ_ASSOC_LANE_EGO=1,TP_ADD_ATTR_OBJ_ASSOC_LANE_LEFT=2
/// ,TP_ADD_ATTR_OBJ_ASSOC_LANE_RIGHT=3,TP_ADD_ATTR_OBJ_ASSOC_LANE_OUTSIDE_LEFT=4
/// ,TP_ADD_ATTR_OBJ_ASSOC_LANE_OUTSIDE_RIGHT=5,TP_ADD_ATTR_OBJ_ASSOC_LANE_LEFT_OUTSIDE_OF_ROAD_BORDERS=6
/// ,TP_ADD_ATTR_OBJ_ASSOC_LANE_RIGHT_OUTSIDE_OF_ROAD_BORDERS=7,TP_ADD_ATTR_OBJ_ASSOC_LANE_SNA=255 ,}
/// @range{0,255}
class CEM_TP_t_ObjAssociatedLane {
public:
  typedef uint8 underlying_type;
  enum do_not_use {
    TP_ADD_ATTR_OBJ_ASSOC_LANE_UNKNOWN = 0U,
    TP_ADD_ATTR_OBJ_ASSOC_LANE_EGO = 1U,
    TP_ADD_ATTR_OBJ_ASSOC_LANE_LEFT = 2U,
    TP_ADD_ATTR_OBJ_ASSOC_LANE_RIGHT = 3U,
    TP_ADD_ATTR_OBJ_ASSOC_LANE_OUTSIDE_LEFT = 4U,
    TP_ADD_ATTR_OBJ_ASSOC_LANE_OUTSIDE_RIGHT = 5U,
    TP_ADD_ATTR_OBJ_ASSOC_LANE_LEFT_OUTSIDE_OF_ROAD_BORDERS = 6U,
    TP_ADD_ATTR_OBJ_ASSOC_LANE_RIGHT_OUTSIDE_OF_ROAD_BORDERS = 7U,
    TP_ADD_ATTR_OBJ_ASSOC_LANE_SNA = 255U
  };

  CEM_TP_t_ObjAssociatedLane() : value(static_cast<underlying_type>(TP_ADD_ATTR_OBJ_ASSOC_LANE_UNKNOWN)) {}

  CEM_TP_t_ObjAssociatedLane(const do_not_use v) : value(static_cast<const underlying_type>(v)) {}

  CEM_TP_t_ObjAssociatedLane(const CEM_TP_t_ObjAssociatedLane &v) : value(static_cast<const underlying_type>(v)) {}

  DEPRECATED_BEGIN DEPRECATED_END CEM_TP_t_ObjAssociatedLane(const underlying_type v) : value(v) {}

  operator underlying_type() const { return value; }

  CEM_TP_t_ObjAssociatedLane &operator|=(const do_not_use v) {
    value |= static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(CEM_TP_t_ObjAssociatedLane &) operator|=(const underlying_type v) {
    value |= v;
    return *this;
  }

  CEM_TP_t_ObjAssociatedLane &operator=(const do_not_use v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  CEM_TP_t_ObjAssociatedLane &operator=(const CEM_TP_t_ObjAssociatedLane &v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(CEM_TP_t_ObjAssociatedLane &) operator=(const underlying_type v) {
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

#endif // FDP_BASE_CEM_TP_T_OBJ_ASSOCIATED_LANE_H_

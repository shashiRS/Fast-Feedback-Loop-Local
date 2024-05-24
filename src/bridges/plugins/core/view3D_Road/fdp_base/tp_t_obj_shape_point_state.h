// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_TP_T_OBJ_SHAPE_POINT_STATE_H_
#define FDP_BASE_TP_T_OBJ_SHAPE_POINT_STATE_H_

#include "eco/deprecation.h"

#include "Platform_Types.h"

// PRQA S 2180 ++
/* date: 2020-03-31, reviewer: PR reviewers of https://github-am.geo.conti.de/ADAS/eco/pull/1100, CCBIssueId:
 * https://jira-adas.zone2.agileci.conti.de/browse/SEP-1137, reason: implicit construction is desired here */

namespace fdp_base {
/// \cond HIDDEN_ENUMS
/// State of shape point at the same index in the aShapePointCoordinates array. INVALID: Shape point does not exist.
/// EDGE_MEASURED: Shapepoint exists and is measured by the EM. This marks a real corner of the bounding shape of the
/// object. EDGE_ASSUMED: Shape point exists but is not measured by the EM. The position of the shape point is generated
/// out of model knowledge. It is not verified by a measurement. VISIBILITY_EDGE: Shape point exists but is not a real
/// corner of the object. It is just the boundary up to what an object is visible to the sensor. Typically used when we
/// only see a part of the object and know it. values: enum { TP_OBJECT_SHAPE_POINT_STATE_INVALID=0
/// ,TP_OBJECT_SHAPE_POINT_STATE_EDGE_MEASURED=1,TP_OBJECT_SHAPE_POINT_STATE_EDGE_ASSUMED=2
/// ,TP_OBJECT_SHAPE_POINT_STATE_VISIBILITY_EDGE=3,TP_OBJECT_SHAPE_POINT_STATE_MAX_DIFF_TYPES=4 ,}
/// @range{0,4}
class TP_t_ObjShapePointState {
public:
  typedef uint8 underlying_type;
  enum do_not_use {
    TP_OBJECT_SHAPE_POINT_STATE_INVALID = 0U,
    TP_OBJECT_SHAPE_POINT_STATE_EDGE_MEASURED = 1U,
    TP_OBJECT_SHAPE_POINT_STATE_EDGE_ASSUMED = 2U,
    TP_OBJECT_SHAPE_POINT_STATE_VISIBILITY_EDGE = 3U,
    TP_OBJECT_SHAPE_POINT_STATE_MAX_DIFF_TYPES = 4U
  };

  TP_t_ObjShapePointState() : value(static_cast<underlying_type>(TP_OBJECT_SHAPE_POINT_STATE_INVALID)) {}

  TP_t_ObjShapePointState(const do_not_use v) : value(static_cast<const underlying_type>(v)) {}

  TP_t_ObjShapePointState(const TP_t_ObjShapePointState &v) : value(static_cast<const underlying_type>(v)) {}

  DEPRECATED_BEGIN DEPRECATED_END TP_t_ObjShapePointState(const underlying_type v) : value(v) {}

  operator underlying_type() const { return value; }

  TP_t_ObjShapePointState &operator|=(const do_not_use v) {
    value |= static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(TP_t_ObjShapePointState &) operator|=(const underlying_type v) {
    value |= v;
    return *this;
  }

  TP_t_ObjShapePointState &operator=(const do_not_use v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  TP_t_ObjShapePointState &operator=(const TP_t_ObjShapePointState &v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(TP_t_ObjShapePointState &) operator=(const underlying_type v) {
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

#endif // FDP_BASE_TP_T_OBJ_SHAPE_POINT_STATE_H_

// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_TP_T_OBJ_ACTIVE_FLAG_H_
#define FDP_BASE_CEM_TP_T_OBJ_ACTIVE_FLAG_H_

#include "eco/deprecation.h"

#include "Platform_Types.h"

// PRQA S 2180 ++
/* date: 2020-03-31, reviewer: PR reviewers of https://github-am.geo.conti.de/ADAS/eco/pull/1100, CCBIssueId:
 * https://jira-adas.zone2.agileci.conti.de/browse/SEP-1137, reason: implicit construction is desired here */

namespace fdp_base {
/// \cond HIDDEN_ENUMS
/// Active flags for each individual object class. Specifies if an object is intended to be delivered on the customer
/// bus values: enum { TP_ADD_CLASSIFICATION_OBJ_AF_NONE=0,TP_ADD_CLASSIFICATION_OBJ_AF_PEDESTRIAN=1
/// ,TP_ADD_CLASSIFICATION_OBJ_AF_BICYCLE=2,TP_ADD_CLASSIFICATION_OBJ_AF_MOTORBIKE=4
/// ,TP_ADD_CLASSIFICATION_OBJ_AF_CAR=8,TP_ADD_CLASSIFICATION_OBJ_AF_TRUCK=16
/// ,TP_ADD_CLASSIFICATION_OBJ_AF_OTHER_VEHICLE=32,TP_ADD_CLASSIFICATION_OBJ_AF_ROAD_BOUNDARY=64
/// ,TP_ADD_CLASSIFICATION_OBJ_AF_MASK=127,}
/// @range{0,127}
class CEM_TP_t_ObjActiveFlag {
public:
  typedef uint8 underlying_type;
  enum do_not_use {
    TP_ADD_CLASSIFICATION_OBJ_AF_NONE = 0U,
    TP_ADD_CLASSIFICATION_OBJ_AF_PEDESTRIAN = 1U,
    TP_ADD_CLASSIFICATION_OBJ_AF_BICYCLE = 2U,
    TP_ADD_CLASSIFICATION_OBJ_AF_MOTORBIKE = 4U,
    TP_ADD_CLASSIFICATION_OBJ_AF_CAR = 8U,
    TP_ADD_CLASSIFICATION_OBJ_AF_TRUCK = 16U,
    TP_ADD_CLASSIFICATION_OBJ_AF_OTHER_VEHICLE = 32U,
    TP_ADD_CLASSIFICATION_OBJ_AF_ROAD_BOUNDARY = 64U,
    TP_ADD_CLASSIFICATION_OBJ_AF_MASK = 127U
  };

  CEM_TP_t_ObjActiveFlag() : value(static_cast<underlying_type>(TP_ADD_CLASSIFICATION_OBJ_AF_NONE)) {}

  CEM_TP_t_ObjActiveFlag(const do_not_use v) : value(static_cast<const underlying_type>(v)) {}

  CEM_TP_t_ObjActiveFlag(const CEM_TP_t_ObjActiveFlag &v) : value(static_cast<const underlying_type>(v)) {}

  DEPRECATED_BEGIN DEPRECATED_END CEM_TP_t_ObjActiveFlag(const underlying_type v) : value(v) {}

  operator underlying_type() const { return value; }

  CEM_TP_t_ObjActiveFlag &operator|=(const do_not_use v) {
    value |= static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(CEM_TP_t_ObjActiveFlag &) operator|=(const underlying_type v) {
    value |= v;
    return *this;
  }

  CEM_TP_t_ObjActiveFlag &operator=(const do_not_use v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  CEM_TP_t_ObjActiveFlag &operator=(const CEM_TP_t_ObjActiveFlag &v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(CEM_TP_t_ObjActiveFlag &) operator=(const underlying_type v) {
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

#endif // FDP_BASE_CEM_TP_T_OBJ_ACTIVE_FLAG_H_

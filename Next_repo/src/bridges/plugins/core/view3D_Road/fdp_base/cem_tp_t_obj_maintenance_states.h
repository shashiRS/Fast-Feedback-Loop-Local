// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_TP_T_OBJ_MAINTENANCE_STATES_H_
#define FDP_BASE_CEM_TP_T_OBJ_MAINTENANCE_STATES_H_

#include "eco/deprecation.h"

#include "Platform_Types.h"

// PRQA S 2180 ++
/* date: 2020-03-31, reviewer: PR reviewers of https://github-am.geo.conti.de/ADAS/eco/pull/1100, CCBIssueId:
 * https://jira-adas.zone2.agileci.conti.de/browse/SEP-1137, reason: implicit construction is desired here */

namespace fdp_base {
/// \cond HIDDEN_ENUMS
/// Indicate the adminstration state of object values: enum { TP_OBJECT_MT_STATE_DELETED=0
/// ,TP_OBJECT_MT_STATE_NEW=1,TP_OBJECT_MT_STATE_MEASURED=2,TP_OBJECT_MT_STATE_PREDICTED=3
/// ,TP_OBJECT_MT_STATE_INACTIVE=4,TP_OBJECT_MT_STATE_MAX_DIFF_TYPES=5 ,}
/// @range{0,5}
class CEM_TP_t_ObjMaintenanceStates {
public:
  typedef uint8 underlying_type;
  enum do_not_use {
    TP_OBJECT_MT_STATE_DELETED = 0U,
    TP_OBJECT_MT_STATE_NEW = 1U,
    TP_OBJECT_MT_STATE_MEASURED = 2U,
    TP_OBJECT_MT_STATE_PREDICTED = 3U,
    TP_OBJECT_MT_STATE_INACTIVE = 4U,
    TP_OBJECT_MT_STATE_MAX_DIFF_TYPES = 5U
  };

  CEM_TP_t_ObjMaintenanceStates() : value(static_cast<underlying_type>(TP_OBJECT_MT_STATE_DELETED)) {}

  CEM_TP_t_ObjMaintenanceStates(const do_not_use v) : value(static_cast<const underlying_type>(v)) {}

  CEM_TP_t_ObjMaintenanceStates(const CEM_TP_t_ObjMaintenanceStates &v)
      : value(static_cast<const underlying_type>(v)) {}

  DEPRECATED_BEGIN DEPRECATED_END CEM_TP_t_ObjMaintenanceStates(const underlying_type v) : value(v) {}

  operator underlying_type() const { return value; }

  CEM_TP_t_ObjMaintenanceStates &operator|=(const do_not_use v) {
    value |= static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(CEM_TP_t_ObjMaintenanceStates &) operator|=(const underlying_type v) {
    value |= v;
    return *this;
  }

  CEM_TP_t_ObjMaintenanceStates &operator=(const do_not_use v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  CEM_TP_t_ObjMaintenanceStates &operator=(const CEM_TP_t_ObjMaintenanceStates &v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(CEM_TP_t_ObjMaintenanceStates &) operator=(const underlying_type v) {
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

#endif // FDP_BASE_CEM_TP_T_OBJ_MAINTENANCE_STATES_H_

// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_FAILURE_STATE_T_H_
#define FDP_BASE_FAILURE_STATE_T_H_

#include "eco/deprecation.h"

#include "Platform_Types.h"

// PRQA S 2180 ++
/* date: 2020-03-31, reviewer: PR reviewers of https://github-am.geo.conti.de/ADAS/eco/pull/1100, CCBIssueId:
 * https://jira-adas.zone2.agileci.conti.de/browse/SEP-1137, reason: implicit construction is desired here */

namespace fdp_base {
/// \cond HIDDEN_ENUMS
/// Failure information in case of failure state CEM_STATE_FAILURE_NONE:There is no failure
/// CEM_STATE_FAILURE_TEMPORARY:Its is a temporary failure CEM_STATE_FAILURE_PERMANENT:It is a permanent failure values:
/// enum { CEM_STATE_FAILURE_NONE=0,CEM_STATE_FAILURE_TEMPORARY=1 ,CEM_STATE_FAILURE_PERMANENT=2,}
/// @range{0,2}
class FailureState_t {
public:
  typedef uint8 underlying_type;
  enum do_not_use { CEM_STATE_FAILURE_NONE = 0U, CEM_STATE_FAILURE_TEMPORARY = 1U, CEM_STATE_FAILURE_PERMANENT = 2U };

  FailureState_t() : value(static_cast<underlying_type>(CEM_STATE_FAILURE_NONE)) {}

  FailureState_t(const do_not_use v) : value(static_cast<const underlying_type>(v)) {}

  FailureState_t(const FailureState_t &v) : value(static_cast<const underlying_type>(v)) {}

  DEPRECATED_BEGIN DEPRECATED_END FailureState_t(const underlying_type v) : value(v) {}

  operator underlying_type() const { return value; }

  FailureState_t &operator|=(const do_not_use v) {
    value |= static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(FailureState_t &) operator|=(const underlying_type v) {
    value |= v;
    return *this;
  }

  FailureState_t &operator=(const do_not_use v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  FailureState_t &operator=(const FailureState_t &v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(FailureState_t &) operator=(const underlying_type v) {
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

#endif // FDP_BASE_FAILURE_STATE_T_H_

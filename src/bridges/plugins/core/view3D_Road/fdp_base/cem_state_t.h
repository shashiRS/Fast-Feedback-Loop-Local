// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_STATE_T_H_
#define FDP_BASE_CEM_STATE_T_H_

#include "eco/deprecation.h"

#include "Platform_Types.h"

// PRQA S 2180 ++
/* date: 2020-03-31, reviewer: PR reviewers of https://github-am.geo.conti.de/ADAS/eco/pull/1100, CCBIssueId:
 * https://jira-adas.zone2.agileci.conti.de/browse/SEP-1137, reason: implicit construction is desired here */

namespace fdp_base {
/// \cond HIDDEN_ENUMS
/// State machine for Cem states CEM_STATE_OFF:Cem state is in off state INIT:Cem state is in init state
/// CEM_STATE_NORMAL:Cem state is in normal state CEM_STATE_FAILURE:Cem state is in failure state values: enum {
/// CEM_STATE_OFF=0,CEM_STATE_INIT=1,CEM_STATE_NORMAL=2 ,CEM_STATE_FAILURE=3,}
/// @range{0,3}
class CemState_t {
public:
  typedef uint8 underlying_type;
  enum do_not_use { CEM_STATE_OFF = 0U, CEM_STATE_INIT = 1U, CEM_STATE_NORMAL = 2U, CEM_STATE_FAILURE = 3U };

  CemState_t() : value(static_cast<underlying_type>(CEM_STATE_OFF)) {}

  CemState_t(const do_not_use v) : value(static_cast<const underlying_type>(v)) {}

  CemState_t(const CemState_t &v) : value(static_cast<const underlying_type>(v)) {}

  DEPRECATED_BEGIN DEPRECATED_END CemState_t(const underlying_type v) : value(v) {}

  operator underlying_type() const { return value; }

  CemState_t &operator|=(const do_not_use v) {
    value |= static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(CemState_t &) operator|=(const underlying_type v) {
    value |= v;
    return *this;
  }

  CemState_t &operator=(const do_not_use v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  CemState_t &operator=(const CemState_t &v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(CemState_t &) operator=(const underlying_type v) {
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

#endif // FDP_BASE_CEM_STATE_T_H_

// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_ALGO_SIGNAL_STATE_T_H_
#define FDP_BASE_ALGO_SIGNAL_STATE_T_H_

#include "eco/deprecation.h"

#include "Platform_Types.h"

// PRQA S 2180 ++
/* date: 2020-03-31, reviewer: PR reviewers of https://github-am.geo.conti.de/ADAS/eco/pull/1100, CCBIssueId:
 * https://jira-adas.zone2.agileci.conti.de/browse/SEP-1137, reason: implicit construction is desired here */

namespace fdp_base {
/// \cond HIDDEN_ENUMS
/// Algo signal state enumeration values: enum { AL_SIG_STATE_INIT=0 ,AL_SIG_STATE_OK=1,AL_SIG_STATE_INVALID=2,}
/// @range{0,2}
class AlgoSignalState_t {
public:
  typedef uint8 underlying_type;
  enum do_not_use { AL_SIG_STATE_INIT = 0U, AL_SIG_STATE_OK = 1U, AL_SIG_STATE_INVALID = 2U };

  AlgoSignalState_t() : value(static_cast<underlying_type>(AL_SIG_STATE_INIT)) {}

  AlgoSignalState_t(const do_not_use v) : value(static_cast<const underlying_type>(v)) {}

  AlgoSignalState_t(const AlgoSignalState_t &v) : value(static_cast<const underlying_type>(v)) {}

  DEPRECATED_BEGIN DEPRECATED_END AlgoSignalState_t(const underlying_type v) : value(v) {}

  operator underlying_type() const { return value; }

  AlgoSignalState_t &operator|=(const do_not_use v) {
    value |= static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(AlgoSignalState_t &) operator|=(const underlying_type v) {
    value |= v;
    return *this;
  }

  AlgoSignalState_t &operator=(const do_not_use v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  AlgoSignalState_t &operator=(const AlgoSignalState_t &v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(AlgoSignalState_t &) operator=(const underlying_type v) {
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

#endif // FDP_BASE_ALGO_SIGNAL_STATE_T_H_

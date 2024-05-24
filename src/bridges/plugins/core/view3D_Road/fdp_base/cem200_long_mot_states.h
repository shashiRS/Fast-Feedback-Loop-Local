// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM200_LONG_MOT_STATES_H_
#define FDP_BASE_CEM200_LONG_MOT_STATES_H_

#include "eco/deprecation.h"

#include "Platform_Types.h"

// PRQA S 2180 ++
/* date: 2020-03-31, reviewer: PR reviewers of https://github-am.geo.conti.de/ADAS/eco/pull/1100, CCBIssueId:
 * https://jira-adas.zone2.agileci.conti.de/browse/SEP-1137, reason: implicit construction is desired here */

namespace fdp_base {
/// \cond HIDDEN_ENUMS
/// Describes the motion state in longitudinal direction the car can take in a Cem200 perspective values: enum {
/// LONG_MOT_STATE_MOVE=0 ,LONG_MOT_STATE_MOVE_FWD=1,LONG_MOT_STATE_MOVE_RWD=2,LONG_MOT_STATE_STDST=3 ,}
/// @range{0,3}
class Cem200LongMotStates {
public:
  typedef uint8 underlying_type;
  enum do_not_use {
    LONG_MOT_STATE_MOVE = 0U,
    LONG_MOT_STATE_MOVE_FWD = 1U,
    LONG_MOT_STATE_MOVE_RWD = 2U,
    LONG_MOT_STATE_STDST = 3U
  };

  Cem200LongMotStates() : value(static_cast<underlying_type>(LONG_MOT_STATE_MOVE)) {}

  Cem200LongMotStates(const do_not_use v) : value(static_cast<const underlying_type>(v)) {}

  Cem200LongMotStates(const Cem200LongMotStates &v) : value(static_cast<const underlying_type>(v)) {}

  DEPRECATED_BEGIN DEPRECATED_END Cem200LongMotStates(const underlying_type v) : value(v) {}

  operator underlying_type() const { return value; }

  Cem200LongMotStates &operator|=(const do_not_use v) {
    value |= static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(Cem200LongMotStates &) operator|=(const underlying_type v) {
    value |= v;
    return *this;
  }

  Cem200LongMotStates &operator=(const do_not_use v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  Cem200LongMotStates &operator=(const Cem200LongMotStates &v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(Cem200LongMotStates &) operator=(const underlying_type v) {
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

#endif // FDP_BASE_CEM200_LONG_MOT_STATES_H_

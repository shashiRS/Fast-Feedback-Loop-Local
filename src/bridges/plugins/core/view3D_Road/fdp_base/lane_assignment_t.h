// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_LANE_ASSIGNMENT_T_H_
#define FDP_BASE_LANE_ASSIGNMENT_T_H_

#include "eco/deprecation.h"

#include "Platform_Types.h"

// PRQA S 2180 ++
/* date: 2020-03-31, reviewer: PR reviewers of https://github-am.geo.conti.de/ADAS/eco/pull/1100, CCBIssueId:
 * https://jira-adas.zone2.agileci.conti.de/browse/SEP-1137, reason: implicit construction is desired here */

namespace fdp_base {
/// \cond HIDDEN_ENUMS
/// Lane assignment Traffic Participants. UNKNOWN_REGION: Region ahead or behind the range of Lane description EGO: Lane
/// within which Ego vehicle is driving L1: Lane immediate left to the Ego lane R1: Lane immediate right to the Ego lane
/// L2: Lane immediate left to L1 lane R2: Lane immediate right to right lane L_OTHERS: Farthest left region adjacent to
/// L2 lane R_OTHERS: Farthest right Region adjacent to R2 lane values: enum { UNKNOWN_REGION=0,EGO=1,L1=2
/// ,R1=3,L2=4,R2=5,L_OTHERS=6,R_OTHERS=7,}
/// @range{0,7}
class LaneAssignment_t {
public:
  typedef uint8 underlying_type;
  enum do_not_use { UNKNOWN_REGION = 0U, EGO = 1U, L1 = 2U, R1 = 3U, L2 = 4U, R2 = 5U, L_OTHERS = 6U, R_OTHERS = 7U };

  LaneAssignment_t() : value(static_cast<underlying_type>(UNKNOWN_REGION)) {}

  LaneAssignment_t(const do_not_use v) : value(static_cast<const underlying_type>(v)) {}

  LaneAssignment_t(const LaneAssignment_t &v) : value(static_cast<const underlying_type>(v)) {}

  DEPRECATED_BEGIN DEPRECATED_END LaneAssignment_t(const underlying_type v) : value(v) {}

  operator underlying_type() const { return value; }

  LaneAssignment_t &operator|=(const do_not_use v) {
    value |= static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(LaneAssignment_t &) operator|=(const underlying_type v) {
    value |= v;
    return *this;
  }

  LaneAssignment_t &operator=(const do_not_use v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  LaneAssignment_t &operator=(const LaneAssignment_t &v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(LaneAssignment_t &) operator=(const underlying_type v) {
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

#endif // FDP_BASE_LANE_ASSIGNMENT_T_H_

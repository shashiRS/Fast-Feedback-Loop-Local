// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_COH_LANE_ASSOC_STATE_T_H_
#define FDP_BASE_COH_LANE_ASSOC_STATE_T_H_

#include "eco/deprecation.h"

#include "Platform_Types.h"

// PRQA S 2180 ++
/* date: 2020-03-31, reviewer: PR reviewers of https://github-am.geo.conti.de/ADAS/eco/pull/1100, CCBIssueId:
 * https://jira-adas.zone2.agileci.conti.de/browse/SEP-1137, reason: implicit construction is desired here */

namespace fdp_base {
/// \cond HIDDEN_ENUMS
/// State of the lane assignment values: enum { COH_ASSOC_STATE_INVALID=0 ,COH_ASSOC_STATE_VALID=1,}
/// @range{0,1}
class CohLaneAssocState_t {
public:
  typedef uint8 underlying_type;
  enum do_not_use { COH_ASSOC_STATE_INVALID = 0U, COH_ASSOC_STATE_VALID = 1U };

  CohLaneAssocState_t() : value(static_cast<underlying_type>(COH_ASSOC_STATE_INVALID)) {}

  CohLaneAssocState_t(const do_not_use v) : value(static_cast<const underlying_type>(v)) {}

  CohLaneAssocState_t(const CohLaneAssocState_t &v) : value(static_cast<const underlying_type>(v)) {}

  DEPRECATED_BEGIN DEPRECATED_END CohLaneAssocState_t(const underlying_type v) : value(v) {}

  operator underlying_type() const { return value; }

  CohLaneAssocState_t &operator|=(const do_not_use v) {
    value |= static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(CohLaneAssocState_t &) operator|=(const underlying_type v) {
    value |= v;
    return *this;
  }

  CohLaneAssocState_t &operator=(const do_not_use v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  CohLaneAssocState_t &operator=(const CohLaneAssocState_t &v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(CohLaneAssocState_t &) operator=(const underlying_type v) {
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

#endif // FDP_BASE_COH_LANE_ASSOC_STATE_T_H_

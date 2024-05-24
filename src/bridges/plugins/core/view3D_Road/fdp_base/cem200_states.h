// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM200_STATES_H_
#define FDP_BASE_CEM200_STATES_H_

#include "eco/deprecation.h"

#include "Platform_Types.h"

// PRQA S 2180 ++
/* date: 2020-03-31, reviewer: PR reviewers of https://github-am.geo.conti.de/ADAS/eco/pull/1100, CCBIssueId:
 * https://jira-adas.zone2.agileci.conti.de/browse/SEP-1137, reason: implicit construction is desired here */

namespace fdp_base {
/// \cond HIDDEN_ENUMS
/// Describes the possible overall states each value can take within Cem200 values: enum {
/// CEM200STATEVALID=0,CEM200STATEINVALID=1 ,CEM200SSTATENOTAVAIL=2,CEM200STATEDECREASED=3,CEM200STATESUBSTITUTE=4
/// ,CEM200STATEINPLAUSIBLE=5,CEM200STATEINIT=15,CEM200STATEMAX=255 ,}
/// @range{0,255}
class Cem200States {
public:
  typedef uint8 underlying_type;
  enum do_not_use {
    CEM200STATEVALID = 0U,
    CEM200STATEINVALID = 1U,
    CEM200SSTATENOTAVAIL = 2U,
    CEM200STATEDECREASED = 3U,
    CEM200STATESUBSTITUTE = 4U,
    CEM200STATEINPLAUSIBLE = 5U,
    CEM200STATEINIT = 15U,
    CEM200STATEMAX = 255U
  };

  Cem200States() : value(static_cast<underlying_type>(CEM200STATEVALID)) {}

  Cem200States(const do_not_use v) : value(static_cast<const underlying_type>(v)) {}

  Cem200States(const Cem200States &v) : value(static_cast<const underlying_type>(v)) {}

  DEPRECATED_BEGIN DEPRECATED_END Cem200States(const underlying_type v) : value(v) {}

  operator underlying_type() const { return value; }

  Cem200States &operator|=(const do_not_use v) {
    value |= static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(Cem200States &) operator|=(const underlying_type v) {
    value |= v;
    return *this;
  }

  Cem200States &operator=(const do_not_use v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  Cem200States &operator=(const Cem200States &v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(Cem200States &) operator=(const underlying_type v) {
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

#endif // FDP_BASE_CEM200_STATES_H_

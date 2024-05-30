// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_MEAS_STATUS_H_
#define FDP_BASE_CEM_MEAS_STATUS_H_

#include "eco/deprecation.h"

#include "Platform_Types.h"

// PRQA S 2180 ++
/* date: 2020-03-31, reviewer: PR reviewers of https://github-am.geo.conti.de/ADAS/eco/pull/1100, CCBIssueId:
 * https://jira-adas.zone2.agileci.conti.de/browse/SEP-1137, reason: implicit construction is desired here */

namespace fdp_base {
/// \cond HIDDEN_ENUMS
/// Measurement status of the Object
/// @range{0,2}
class CEM_MeasStatus {
public:
  typedef uint8 underlying_type;
  enum do_not_use {
    /// Measured
    VERTEX_MEASURED = 0U,
    /// Interpolated
    VERTEX_INTERPOLATED = 1U,
    /// Hypothesis
    VERTEX_HYPOTHESIS = 2U
  };

  CEM_MeasStatus() : value(static_cast<underlying_type>(VERTEX_MEASURED)) {}

  CEM_MeasStatus(const do_not_use v) : value(static_cast<const underlying_type>(v)) {}

  CEM_MeasStatus(const CEM_MeasStatus &v) : value(static_cast<const underlying_type>(v)) {}

  DEPRECATED_BEGIN DEPRECATED_END CEM_MeasStatus(const underlying_type v) : value(v) {}

  operator underlying_type() const { return value; }

  CEM_MeasStatus &operator|=(const do_not_use v) {
    value |= static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(CEM_MeasStatus &) operator|=(const underlying_type v) {
    value |= v;
    return *this;
  }

  CEM_MeasStatus &operator=(const do_not_use v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  CEM_MeasStatus &operator=(const CEM_MeasStatus &v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(CEM_MeasStatus &) operator=(const underlying_type v) {
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

#endif // FDP_BASE_CEM_MEAS_STATUS_H_

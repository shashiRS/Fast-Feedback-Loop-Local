// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_SAFETY_QUALIFIER_ENUM_H_
#define FDP_BASE_SAFETY_QUALIFIER_ENUM_H_

#include "eco/deprecation.h"

#include "Platform_Types.h"

// PRQA S 2180 ++
/* date: 2020-03-31, reviewer: PR reviewers of https://github-am.geo.conti.de/ADAS/eco/pull/1100, CCBIssueId:
 * https://jira-adas.zone2.agileci.conti.de/browse/SEP-1137, reason: implicit construction is desired here */

namespace fdp_base {
/// \cond HIDDEN_ENUMS
class safetyQualifierEnum {
public:
  typedef uint8 underlying_type;
  enum do_not_use { RMF_SAFETY_QM = 0U, RMF_SAFETY_ASIL_B = 1U };

  safetyQualifierEnum() : value(static_cast<underlying_type>(RMF_SAFETY_QM)) {}

  safetyQualifierEnum(const do_not_use v) : value(static_cast<const underlying_type>(v)) {}

  safetyQualifierEnum(const safetyQualifierEnum &v) : value(static_cast<const underlying_type>(v)) {}

  DEPRECATED_BEGIN DEPRECATED_END safetyQualifierEnum(const underlying_type v) : value(v) {}

  operator underlying_type() const { return value; }

  safetyQualifierEnum &operator|=(const do_not_use v) {
    value |= static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(safetyQualifierEnum &) operator|=(const underlying_type v) {
    value |= v;
    return *this;
  }

  safetyQualifierEnum &operator=(const do_not_use v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  safetyQualifierEnum &operator=(const safetyQualifierEnum &v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(safetyQualifierEnum &) operator=(const underlying_type v) {
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

#endif // FDP_BASE_SAFETY_QUALIFIER_ENUM_H_

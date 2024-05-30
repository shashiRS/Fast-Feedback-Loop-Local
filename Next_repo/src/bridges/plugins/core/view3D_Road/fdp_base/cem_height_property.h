// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_HEIGHT_PROPERTY_H_
#define FDP_BASE_CEM_HEIGHT_PROPERTY_H_

#include "eco/deprecation.h"

#include "Platform_Types.h"

// PRQA S 2180 ++
/* date: 2020-03-31, reviewer: PR reviewers of https://github-am.geo.conti.de/ADAS/eco/pull/1100, CCBIssueId:
 * https://jira-adas.zone2.agileci.conti.de/browse/SEP-1137, reason: implicit construction is desired here */

namespace fdp_base {
/// \cond HIDDEN_ENUMS
/// Drivability Information of the Object
/// @range{0,2}
class CEM_HeightProperty {
public:
  typedef uint8 underlying_type;
  enum do_not_use {
    /// Static element is an Obstacle
    STATIC_OBSTACLE = 0U,
    /// Static element can be overdriven
    STATIC_OVERDRIVABLE = 1U,
    /// Static element can be underdriven
    STATIC_UNDERDRIVABLE = 2U
  };

  CEM_HeightProperty() : value(static_cast<underlying_type>(STATIC_OBSTACLE)) {}

  CEM_HeightProperty(const do_not_use v) : value(static_cast<const underlying_type>(v)) {}

  CEM_HeightProperty(const CEM_HeightProperty &v) : value(static_cast<const underlying_type>(v)) {}

  DEPRECATED_BEGIN DEPRECATED_END CEM_HeightProperty(const underlying_type v) : value(v) {}

  operator underlying_type() const { return value; }

  CEM_HeightProperty &operator|=(const do_not_use v) {
    value |= static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(CEM_HeightProperty &) operator|=(const underlying_type v) {
    value |= v;
    return *this;
  }

  CEM_HeightProperty &operator=(const do_not_use v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  CEM_HeightProperty &operator=(const CEM_HeightProperty &v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(CEM_HeightProperty &) operator=(const underlying_type v) {
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

#endif // FDP_BASE_CEM_HEIGHT_PROPERTY_H_

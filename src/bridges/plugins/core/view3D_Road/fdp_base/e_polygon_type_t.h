// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_E_POLYGON_TYPE_T_H_
#define FDP_BASE_E_POLYGON_TYPE_T_H_

#include "eco/deprecation.h"

#include "Platform_Types.h"

// PRQA S 2180 ++
/* date: 2020-03-31, reviewer: PR reviewers of https://github-am.geo.conti.de/ADAS/eco/pull/1100, CCBIssueId:
 * https://jira-adas.zone2.agileci.conti.de/browse/SEP-1137, reason: implicit construction is desired here */

namespace fdp_base {
/// \cond HIDDEN_ENUMS
/// Type of the polygon representing whether the polygon is filled or a hole values: enum {
/// VISIBLE_AREA=1,INVISIBLE_AREA=0,}
/// @range{0,1}
class e_PolygonType_t {
public:
  typedef uint8 underlying_type;
  enum do_not_use { INVISIBLE_AREA = 0U, VISIBLE_AREA = 1U };

  e_PolygonType_t() : value(static_cast<underlying_type>(INVISIBLE_AREA)) {}

  e_PolygonType_t(const do_not_use v) : value(static_cast<const underlying_type>(v)) {}

  e_PolygonType_t(const e_PolygonType_t &v) : value(static_cast<const underlying_type>(v)) {}

  DEPRECATED_BEGIN DEPRECATED_END e_PolygonType_t(const underlying_type v) : value(v) {}

  operator underlying_type() const { return value; }

  e_PolygonType_t &operator|=(const do_not_use v) {
    value |= static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(e_PolygonType_t &) operator|=(const underlying_type v) {
    value |= v;
    return *this;
  }

  e_PolygonType_t &operator=(const do_not_use v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  e_PolygonType_t &operator=(const e_PolygonType_t &v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(e_PolygonType_t &) operator=(const underlying_type v) {
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

#endif // FDP_BASE_E_POLYGON_TYPE_T_H_

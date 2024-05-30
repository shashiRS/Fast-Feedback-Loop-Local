// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_LINEAR_OBJECT_TYPE_ENUM_H_
#define FDP_BASE_LINEAR_OBJECT_TYPE_ENUM_H_

#include "eco/deprecation.h"

#include "Platform_Types.h"

// PRQA S 2180 ++
/* date: 2020-03-31, reviewer: PR reviewers of https://github-am.geo.conti.de/ADAS/eco/pull/1100, CCBIssueId:
 * https://jira-adas.zone2.agileci.conti.de/browse/SEP-1137, reason: implicit construction is desired here */

namespace fdp_base {
/// \cond HIDDEN_ENUMS
class linearObjectTypeEnum {
public:
  typedef uint8 underlying_type;
  enum do_not_use {
    LINOBJTYPE_INVALID = 0U,
    LINOBJTYPE_UNKNOWN = 1U,
    LINOBJTYPE_LANEMARKING = 2U,
    LINOBJTYPE_GUARDRAIL = 3U,
    LINOBJTYPE_VIRTUAL = 4U,
    LINOBJTYPE_CURBSTONE = 5U,
    LINOBJTYPE_ROADEDGE = 6U,
    LINOBJTYPE_WALL = 7U,
    LINOBJTYPE_DELINEATOR = 8U,
    LINOBJTYPE_PYLON = 9U
  };

  linearObjectTypeEnum() : value(static_cast<underlying_type>(LINOBJTYPE_INVALID)) {}

  linearObjectTypeEnum(const do_not_use v) : value(static_cast<const underlying_type>(v)) {}

  linearObjectTypeEnum(const linearObjectTypeEnum &v) : value(static_cast<const underlying_type>(v)) {}

  DEPRECATED_BEGIN DEPRECATED_END linearObjectTypeEnum(const underlying_type v) : value(v) {}

  operator underlying_type() const { return value; }

  linearObjectTypeEnum &operator|=(const do_not_use v) {
    value |= static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(linearObjectTypeEnum &) operator|=(const underlying_type v) {
    value |= v;
    return *this;
  }

  linearObjectTypeEnum &operator=(const do_not_use v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  linearObjectTypeEnum &operator=(const linearObjectTypeEnum &v) {
    value = static_cast<const underlying_type>(v);
    return *this;
  }

  DEPRECATED(linearObjectTypeEnum &) operator=(const underlying_type v) {
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

#endif // FDP_BASE_LINEAR_OBJECT_TYPE_ENUM_H_

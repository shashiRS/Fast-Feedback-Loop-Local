// COMPANY:   Continental Automotive
// COMPONENT: Eco

#ifndef ECO_MEMORY_SIZE_CHECKER_H_
#define ECO_MEMORY_SIZE_CHECKER_H_

// PRQA S 1020 ++
/* date: 2020-03-19, reviewer: PR reviewers of https://github.geo.conti.de/ADAS/CEM200/pull/4988 , reason: macros
 * defined in this file have no direct impact on production code */

// PRQA S 1039 ++
/* date: 2020-03-31, reviewer: PR reviewers of https://github.geo.conti.de/ADAS/CEM200/pull/5125 ,CCBIssueId:
 * https://jira-adas.zone2.agileci.conti.de/browse/SEP-1139, reason: macros defined in this file have no direct impact
 * on production code */

#include "Platform_Types.h"

// tested with Keil "ARM Compiler 6.6.1 Long Term Maintenance" identifying itself with
// VERSION="4.2.1 Compatible Clang 4.0.0 " __clang_version__="4.0.0 "
// Used in project KnorrBremse to compile for MFC525.

/// Concatenate x and y.
#define CONCATENATE(x, y) x##y

/// Concatenate x and y. Concealed functionality.
#define CONCEALED_CONCATINATION(x, y) CONCATENATE(x, y)

namespace eco {
template <sint32 L, sint32 R>
class MemorySizeMismatch;
template <sint32 F>
class MemorySizeMismatch<F, F> {};
} // namespace eco

/// Memory checker validates two input number values.
#define MEMORY_SIZE_CHECKER(Left, Right)                                                                               \
  struct CONCEALED_CONCATINATION(memorySizeCheckerMemorySizeMatches, __LINE__) {                                       \
    eco::MemorySizeMismatch<sizeof(Left), sizeof(Right)> check;                                                        \
  }
/// Const memory checker validates an input struct with a number value.
#define MEMORY_SIZE_CHECKER_CONST(Left, Right)                                                                         \
  struct CONCEALED_CONCATINATION(memorySizeCheckerMemorySizeMatches, __LINE__) {                                       \
    eco::MemorySizeMismatch<(Left), sizeof(Right)> check;                                                              \
  }

// PRQA S 1039 --
// PRQA S 1020 --

#endif // ECO_MEMORY_SIZE_CHECKER_H_

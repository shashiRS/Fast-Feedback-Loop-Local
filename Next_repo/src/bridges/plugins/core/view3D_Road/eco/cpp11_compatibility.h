// COMPANY:   Continental Automotive
// COMPONENT: Eco
#ifndef ECO_CPP11_COMPATIBILITY_H
#define ECO_CPP11_COMPATIBILITY_H

// PRQA S 1020 ++
/* date: 2020-03-19, reviewer: PR reviewers of https://github.geo.conti.de/ADAS/CEM200/pull/4988 , reason: macros
 * defined in this file have no direct impact on production code */

// PRQA S 1039 ++
/* date: 2020-03-31, reviewer: PR reviewers of https://github.geo.conti.de/ADAS/CEM200/pull/5125 ,CCBIssueId:
 * https://jira-adas.zone2.agileci.conti.de/browse/SEP-1139, reason: macros defined in this file have no direct impact
 * on production code */

// PRQA S 5174 ++
// PRQA S 5181 ++
/* date: 2021-05-03, reviewer: PR reviewers of https://github.geo.conti.de/ADAS/CEM200/pull/5988, CCBIssueId:
 * https://jira-adas.zone2.agileci.conti.de/browse/SEP-1183, reason: no use of standard library */
#ifndef NULL
#define NULL 0
#endif
// PRQA S 5181 --
// PRQA S 5174 --

#if (!defined(_MSC_VER) && __cplusplus < 201103L) || (defined(_MSC_VER) && _MSC_VER <= 1900)

// Define c++11 language keywords to ensure compatibility between source code
// that uses keywords introduced in c++11 and older versions
#ifdef nullptr
#undef nullptr
#endif

#define final
#define noexcept
#define override
// PRQA S 1025 ++
/* date: 2021-05-26, reviewer: PR reviewers of https://github.geo.conti.de/ADAS/CEM200/pull/6496, CCBIssueId:
 * https://jira-adas.zone2.agileci.conti.de/browse/SEP-1190, reason: Bug in QAC tooling results in hundreds of L3
 * warnings for the use of NULL */
#define nullptr 0
// PRQA S 1025 --

// tested with Keil "ARM Compiler 6.6.1 Long Term Maintenance" identifying itself with
// VERSION="4.2.1 Compatible Clang 4.0.0 " __clang_version__="4.0.0 "
// Used in project KnorrBremse to compile for MFC525.
// It defines static_assert also for -std=c++03 and -std=c++98

// for all compilers
#ifdef static_assert
#undef static_assert
#endif
#define ASSERT_CONCAT_(a, b) a##b
#define ASSERT_CONCAT(a, b) ASSERT_CONCAT_(a, b)

namespace eco {
template <bool>
class StaticAssert;
template <>
class StaticAssert<true> {};

} // namespace eco

#define static_assert(cond, message)                                                                                   \
  struct ASSERT_CONCAT(StaticAssert_, __LINE__) {                                                                      \
    eco::StaticAssert<(cond)> check;                                                                                   \
  }

// PRQA S 1039 --
// PRQA S 1020 --
#endif // (!defined (_MSC_VER) && __cplusplus < 201103L) || (defined (_MSC_VER) && _MSC_VER <= 1900)

#endif // ECO_CPP11_COMPATIBILITY_H

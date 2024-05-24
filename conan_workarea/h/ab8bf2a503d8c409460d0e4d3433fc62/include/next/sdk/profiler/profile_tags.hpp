/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     profile_tags.hpp
 * @brief    provides tagging for profiling code
 */

#ifndef NEXTSDK_PROFILE_TAGS_H
#define NEXTSDK_PROFILE_TAGS_H

#ifndef TRACY_ENABLE
#define ProfileCore_OV
#define ProfileCoreN_OV(x)
#define ProfileCoreNC_OV(x, y)
#define ProfileCore_DTL
#define ProfileCoreN_DTL(x)
#define ProfileCoreNC_DTL(x, y)
#define ProfileCore_FLL
#define ProfileCoreN_FLL(x)
#define ProfileCoreNC_FLL(x, y)

#define ProfileUdex_OV
#define ProfileUdexN_OV(x)
#define ProfileUdexNC_OV(x, y)
#define ProfileUdex_DTL
#define ProfileUdexN_DTL(x)
#define ProfileUdexNC_DTL(x, y)
#define ProfileUdex_FLL
#define ProfileUdexN_FLL(x)
#define ProfileUdexNC_FLL(x, y)

#define ProfileControl_OV
#define ProfileControlN_OV(x)
#define ProfileControlNC_OV(x, y)
#define ProfileControl_DTL
#define ProfileControlN_DTL(x)
#define ProfileControlNC_DTL(x, y)
#define ProfileControl_FLL
#define ProfileControlN_FLL(x)
#define ProfileControlNC_FLL(x, y)

#define ProfileBridges_OV
#define ProfileBridgesN_OV(x)
#define ProfileBridgesNC_OV(x, y)
#define ProfileBridges_DTL
#define ProfileBridgesN_DTL(x)
#define ProfileBridgesNC_DTL(x, y)
#define ProfileBridges_FLL
#define ProfileBridgesN_FLL(x)
#define ProfileBridgesNC_FLL(x, y)

#define ProfilePlayer_OV
#define ProfilePlayerN_OV(x)
#define ProfilePlayerNC_OV(x, y)
#define ProfilePlayer_DTL
#define ProfilePlayerN_DTL(x)
#define ProfilePlayerNC_DTL(x, y)
#define ProfilePlayer_FLL
#define ProfilePlayerN_FLL(x)
#define ProfilePlayerNC_FLL(x, y)

#define TracyLockable(x, y) x y
#define LockableBase(x) x

#else

#include <tracy/Tracy.hpp>
#include "profiler.hpp"

// Profilinglevel: OVERVIEW
#define ProfileCore_OV                                                                                                 \
  ZoneNamedC(___tracy_scoped_zone, next::sdk::profiler::kProfilerColor_Coresdk,                                        \
             (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::OVERVIEW))
// Profilinglevel: OVERVIEW with Markername
#define ProfileCoreN_OV(name)                                                                                          \
  ZoneNamedNC(___tracy_scoped_zone, name, next::sdk::profiler::kProfilerColor_Coresdk,                                 \
              (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::OVERVIEW))
// Profilinglevel: OVERVIEW with Markername, Color
#define ProfileCoreNC_OV(name, color)                                                                                  \
  ZoneNamedNC(___tracy_scoped_zone, name, color,                                                                       \
              (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::OVERVIEW))
// Profilinglevel: DETAILED
#define ProfileCore_DTL                                                                                                \
  ZoneNamedC(___tracy_scoped_zone, next::sdk::profiler::kProfilerColor_Coresdk1,                                       \
             (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::DETAIL))
// Profilinglevel: DETAILED with Markername
#define ProfileCoreN_DTL(name)                                                                                         \
  ZoneNamedNC(___tracy_scoped_zone, name, next::sdk::profiler::kProfilerColor_Coresdk1,                                \
              (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::DETAIL))
// Profilinglevel: DETAILED with Markername, Color
#define ProfileCoreNC_DTL(name, color)                                                                                 \
  ZoneNamedNC(___tracy_scoped_zone, name, color,                                                                       \
              (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::DETAIL))
// Profilinglevel: FULL
#define ProfileCore_FLL                                                                                                \
  ZoneNamedC(___tracy_scoped_zone, next::sdk::profiler::kProfilerColor_Coresdk2,                                       \
             (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::FULL))
// Profilinglevel: FULL with Markername
#define ProfileCoreN_FLL(name) ZoneNamedNC(zonename, name, next::sdk::profiler::kProfilerColor_Coresdk2, (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::FULL)
// Profilinglevel: FULL with Markername, Color
#define ProfileCoreNC_FLL(name, color) ZoneNamedNC(zonename, name, color, (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::FULL)

// Profilinglevel: OVERVIEW
#define ProfileUdex_OV                                                                                                 \
  ZoneNamedC(___tracy_scoped_zone, next::sdk::profiler::kProfilerColor_Udex,                                           \
             (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::OVERVIEW))
// Profilinglevel: OVERVIEW with Markername
#define ProfileUdexN_OV(name)                                                                                          \
  ZoneNamedNC(___tracy_scoped_zone, name, next::sdk::profiler::kProfilerColor_Udex,                                    \
              (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::OVERVIEW))
// Profilinglevel: OVERVIEW with Markername, Color
#define ProfileUdexNC_OV(name, color)                                                                                  \
  ZoneNamedNC(___tracy_scoped_zone, name, color,                                                                       \
              (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::OVERVIEW))
// Profilinglevel: DETAILED
#define ProfileUdex_DTL                                                                                                \
  ZoneNamedC(___tracy_scoped_zone, next::sdk::profiler::kProfilerColor_Udex1,                                          \
             (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::DETAIL))
// Profilinglevel: DETAILED with Markername
#define ProfileUdexN_DTL(name)                                                                                         \
  ZoneNamedNC(___tracy_scoped_zone, name, next::sdk::profiler::kProfilerColor_Udex1,                                   \
              (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::DETAIL))
// Profilinglevel: DETAILED with Markername, Color
#define ProfileUdexNC_DTL(name, color)                                                                                 \
  ZoneNamedNC(___tracy_scoped_zone, name, color,                                                                       \
              (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::DETAIL))
// Profilinglevel: FULL
#define ProfileUdex_FLL                                                                                                \
  ZoneNamedC(___tracy_scoped_zone, next::sdk::profiler::kProfilerColor_Udex2,                                          \
             (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::FULL))
// Profilinglevel: FULL with Markername
#define ProfileUdexN_FLL(name) ZoneNamedNC(zonename, name, next::sdk::profiler::kProfilerColor_Udex2, (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::FULL)
// Profilinglevel: FULL with Markername, Color
#define ProfileUdexNC_FLL(name, color) ZoneNamedNC(zonename, name, color, (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::FULL)

// Profilinglevel: OVERVIEW
#define ProfileControl_OV                                                                                              \
  ZoneNamedC(___tracy_scoped_zone, next::sdk::profiler::kProfilerColor_Control,                                        \
             (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::OVERVIEW))
// Profilinglevel: OVERVIEW with Markername
#define ProfileControlN_OV(name)                                                                                       \
  ZoneNamedNC(___tracy_scoped_zone, name, next::sdk::profiler::kProfilerColor_Control,                                 \
              (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::OVERVIEW))
// Profilinglevel: OVERVIEW with Markername, Color
#define ProfileControlNC_OV(name, color)                                                                               \
  ZoneNamedNC(___tracy_scoped_zone, name, color,                                                                       \
              (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::OVERVIEW))
// Profilinglevel: DETAILED
#define ProfileControl_DTL                                                                                             \
  ZoneNamedC(___tracy_scoped_zone, next::sdk::profiler::kProfilerColor_Control1,                                       \
             (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::DETAIL))
// Profilinglevel: DETAILED with Markername
#define ProfileControlN_DTL(name)                                                                                      \
  ZoneNamedNC(___tracy_scoped_zone, name, next::sdk::profiler::kProfilerColor_Control1,                                \
              (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::DETAIL))
// Profilinglevel: DETAILED with Markername, Color
#define ProfileControlNC_DTL(name, color)                                                                              \
  ZoneNamedNC(___tracy_scoped_zone, name, color,                                                                       \
              (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::DETAIL))
// Profilinglevel: FULL
#define ProfileControl_FLL                                                                                             \
  ZoneNamedC(___tracy_scoped_zone, next::sdk::profiler::kProfilerColor_Control2,                                       \
             (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::FULL))
// Profilinglevel: FULL with Markername
#define ProfileControlN_FLL(name) ZoneNamedNC(zonename, name, next::sdk::profiler::kProfilerColor_Control2, (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::FULL)
// Profilinglevel: FULL with Markername, Color
#define ProfileControlNC_FLL(name, color) ZoneNamedNC(zonename, name, color, (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::FULL)

// Profilinglevel: OVERVIEW
#define ProfileBridges_OV                                                                                              \
  ZoneNamedC(___tracy_scoped_zone, next::sdk::profiler::kProfilerColor_Bridges,                                        \
             (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::OVERVIEW))
// Profilinglevel: OVERVIEW with Markername
#define ProfileBridgesN_OV(name)                                                                                       \
  ZoneNamedNC(___tracy_scoped_zone, name, next::sdk::profiler::kProfilerColor_Bridges,                                 \
              (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::OVERVIEW))
// Profilinglevel: OVERVIEW with Markername, Color
#define ProfileBridgesNC_OV(name, color)                                                                               \
  ZoneNamedNC(___tracy_scoped_zone, name, color,                                                                       \
              (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::OVERVIEW))
// Profilinglevel: DETAILED
#define ProfileBridges_DTL                                                                                             \
  ZoneNamedC(___tracy_scoped_zone, next::sdk::profiler::kProfilerColor_Bridges1,                                       \
             (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::DETAIL))
// Profilinglevel: DETAILED with Markername
#define ProfileBridgesN_DTL(name)                                                                                      \
  ZoneNamedNC(___tracy_scoped_zone, name, next::sdk::profiler::kProfilerColor_Bridges1,                                \
              (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::DETAIL))
// Profilinglevel: DETAILED with Markername, Color
#define ProfileBridgesNC_DTL(name, color)                                                                              \
  ZoneNamedNC(___tracy_scoped_zone, name, color,                                                                       \
              (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::DETAIL))
// Profilinglevel: FULL
#define ProfileBridges_FLL                                                                                             \
  ZoneNamedC(___tracy_scoped_zone, next::sdk::profiler::kProfilerColor_Bridges2,                                       \
             (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::FULL))
// Profilinglevel: FULL with Markername
#define ProfileBridgesN_FLL(name) ZoneNamedNC(zonename, name, next::sdk::profiler::kProfilerColor_Bridges2, (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::FULL)
// Profilinglevel: FULL with Markername, Color
#define ProfileBridgesNC_FLL(name, color) ZoneNamedNC(zonename, name, color, (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::FULL)

// Profilinglevel: OVERVIEW
#define ProfilePlayer_OV                                                                                               \
  ZoneNamedC(___tracy_scoped_zone, next::sdk::profiler::kProfilerColor_Player,                                         \
             (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::OVERVIEW))
// Profilinglevel: OVERVIEW with Markername
#define ProfilePlayerN_OV(name)                                                                                        \
  ZoneNamedNC(___tracy_scoped_zone, name, next::sdk::profiler::kProfilerColor_Player,                                  \
              (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::OVERVIEW))
// Profilinglevel: OVERVIEW with Markername, Color
#define ProfilePlayerNC_OV(name, color)                                                                                \
  ZoneNamedNC(___tracy_scoped_zone, name, color,                                                                       \
              (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::OVERVIEW))
// Profilinglevel: DETAILED
#define ProfilePlayer_DTL                                                                                              \
  ZoneNamedC(___tracy_scoped_zone, next::sdk::profiler::kProfilerColor_Player1,                                        \
             (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::DETAIL))
// Profilinglevel: DETAILED with Markername
#define ProfilePlayerN_DTL(name)                                                                                       \
  ZoneNamedNC(___tracy_scoped_zone, name, next::sdk::profiler::kProfilerColor_Player1,                                 \
              (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::DETAIL))
// Profilinglevel: DETAILED with Markername, Color
#define ProfilePlayerNC_DTL(name, color)                                                                               \
  ZoneNamedNC(___tracy_scoped_zone, name, color,                                                                       \
              (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::DETAIL))
// Profilinglevel: FULL
#define ProfilePlayer_FLL                                                                                              \
  ZoneNamedC(___tracy_scoped_zone, next::sdk::profiler::kProfilerColor_Player2,                                        \
             (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::FULL))
// Profilinglevel: FULL with Markername
#define ProfilePlayerN_FLL(name) ZoneNamedNC(zonename, name, next::sdk::profiler::kProfilerColor_Player2, (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::FULL)
// Profilinglevel: FULL with Markername, Color
#define ProfilePlayerNC_FLL(name, color) ZoneNamedNC(zonename, name, color, (next::sdk::profiler::getProfilingLevel() >= next::sdk::profiler::ProfilingLevel::FULL)
namespace next {
namespace sdk {
namespace profiler {

static constexpr tracy::Color::ColorType kProfilerColor_Coresdk{tracy::Color::Blue};
static constexpr tracy::Color::ColorType kProfilerColor_Coresdk1{tracy::Color::Blue1};
static constexpr tracy::Color::ColorType kProfilerColor_Coresdk2{tracy::Color::Blue2};

static constexpr tracy::Color::ColorType kProfilerColor_Udex{tracy::Color::Green};
static constexpr tracy::Color::ColorType kProfilerColor_Udex1{tracy::Color::Green1};
static constexpr tracy::Color::ColorType kProfilerColor_Udex2{tracy::Color::Green2};

static constexpr tracy::Color::ColorType kProfilerColor_Control{tracy::Color::Yellow};
static constexpr tracy::Color::ColorType kProfilerColor_Control1{tracy::Color::Yellow1};
static constexpr tracy::Color::ColorType kProfilerColor_Control2{tracy::Color::Yellow2};

static constexpr tracy::Color::ColorType kProfilerColor_Bridges{tracy::Color::Orange};
static constexpr tracy::Color::ColorType kProfilerColor_Bridges1{tracy::Color::Orange1};
static constexpr tracy::Color::ColorType kProfilerColor_Bridges2{tracy::Color::Orange2};

static constexpr tracy::Color::ColorType kProfilerColor_Player{tracy::Color::Brown};
static constexpr tracy::Color::ColorType kProfilerColor_Player1{tracy::Color::Brown1};
static constexpr tracy::Color::ColorType kProfilerColor_Player2{tracy::Color::Brown2};

} // namespace profiler
} // namespace sdk
} // namespace next
#endif

#endif // NEXTSDK_PROFILE_TAGS_H

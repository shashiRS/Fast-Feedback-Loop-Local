// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_TRACE_INFO_H_
#define FDP_BASE_TRACE_INFO_H_

#include "Platform_Types.h"

namespace fdp_base {

/// Trace Info Of Obstacle
struct TraceInfo {
  /**
  @range{0,18446744073709551615}

  Timestamp of Birth

  */
  uint64 tSoB;
  /**
  @range{0,65535}

  Array of IDs of Contributing Sensors

  */
  uint16 contributingSensors[4];
};

} // namespace fdp_base

#endif // FDP_BASE_TRACE_INFO_H_

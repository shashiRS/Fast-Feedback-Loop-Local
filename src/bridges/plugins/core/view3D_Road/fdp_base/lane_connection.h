// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_LANE_CONNECTION_H_
#define FDP_BASE_LANE_CONNECTION_H_

#include "Platform_Types.h"
#include "fdp_base/lane_connection_type_enum.h"
#include "fdp_base/safety_qualifier_enum.h"

namespace fdp_base {

struct LaneConnection {
  safetyQualifierEnum safetyQualifier;
  uint16 contributingSensors[32];
  laneConnectionTypeEnum laneConnectionType;
  uint8 in;
  uint8 out;
  float32 inOffset;
  float32 outOffset;
};

} // namespace fdp_base

#endif // FDP_BASE_LANE_CONNECTION_H_

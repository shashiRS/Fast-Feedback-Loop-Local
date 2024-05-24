// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_ROAD_CONNECTION_H_
#define FDP_BASE_ROAD_CONNECTION_H_

#include "Platform_Types.h"
#include "fdp_base/road_connection_type_enum.h"
#include "fdp_base/safety_qualifier_enum.h"

namespace fdp_base {

struct RoadConnection {
  safetyQualifierEnum safetyQualifier;
  uint16 contributingSensors[32];
  uint8 inIndex;
  uint8 outIndex;
  roadConnectionTypeEnum typeBitMask;
};

} // namespace fdp_base

#endif // FDP_BASE_ROAD_CONNECTION_H_

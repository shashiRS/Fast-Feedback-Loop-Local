// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_SENSOR_ASSOCIATION_H_
#define FDP_BASE_SENSOR_ASSOCIATION_H_

#include "Platform_Types.h"
#include "fdp_base/maintenance_state_enum.h"
#include "fdp_base/polyline.h"
#include "fdp_base/safety_qualifier_enum.h"

namespace fdp_base {

struct SensorAssociation {
  safetyQualifierEnum safetyQualifier;
  uint16 contributingSensors[32];
  maintenanceStateEnum maintenanceState;
  Polyline range;
};

} // namespace fdp_base

#endif // FDP_BASE_SENSOR_ASSOCIATION_H_

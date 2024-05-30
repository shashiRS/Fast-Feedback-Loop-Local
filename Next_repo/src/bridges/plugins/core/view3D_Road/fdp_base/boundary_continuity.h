// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_BOUNDARY_CONTINUITY_H_
#define FDP_BASE_BOUNDARY_CONTINUITY_H_

#include "Platform_Types.h"
#include "fdp_base/polyline.h"

namespace fdp_base {

struct BoundaryContinuity {
  uint8 value;
  Polyline range;
};

} // namespace fdp_base

#endif // FDP_BASE_BOUNDARY_CONTINUITY_H_

// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_TP_T_OBJ_GEOMETRY_H_
#define FDP_BASE_CEM_TP_T_OBJ_GEOMETRY_H_

#include "Platform_Types.h"

namespace fdp_base {

/// Object size related properties
struct CEM_TP_t_ObjGeometry {
  /**
  @unit{m}

  @range{0,10}

  Object height (longitunal dimension)

  */
  float32 fHeight;
  /**
  @unit{m}

  @range{-1,10}

  Object height standard deviation. Default set to negative value (-1.0).

  */
  float32 fHeightStd;
  /**
  @unit{m}

  @range{-1,10}

  Distance of objects lower boundary to the ground plane, independent
of vehicle pitch / roll movements

  */
  float32 fOffsetToGround;
  /**
  @unit{m}

  fOffsetToGround standard deviation: Set to negative value (-1.0)
if signal not available.

  */
  float32 fOffsetToGroundStd;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_TP_T_OBJ_GEOMETRY_H_

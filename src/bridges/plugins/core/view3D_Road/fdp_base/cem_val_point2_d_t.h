// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_VAL_POINT2_D_T_H_
#define FDP_BASE_CEM_VAL_POINT2_D_T_H_

#include "Platform_Types.h"

namespace fdp_base {

/// Vertices of the polygon in 2D format
struct CEM_VAL_Point2D_t {
  /**
  @unit{meters}

  X_coordinate of vertex in Cartesian coordinate system

  */
  float32 f_X;
  /**
  @unit{meters}

  Y_coordinate of vertex in Cartesian coordinate system

  */
  float32 f_Y;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_VAL_POINT2_D_T_H_

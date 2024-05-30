// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_TP_T_OBJ_SHAPE_POINT_COORD_H_
#define FDP_BASE_CEM_TP_T_OBJ_SHAPE_POINT_COORD_H_

#include "Platform_Types.h"

namespace fdp_base {

/// Coordinates relative to the fDistX, fDistY point. To form the shape they should be connected neighbourwise.
struct CEM_TP_t_ObjShapePointCoord {
  /**
  @unit{m}

  @range{-50,50}

  X Position of Point, Relative to fDistX longitudinal coordinate
, aligned with the ego coordinates axes

  */
  float32 fPosX;
  /**
  @unit{m}

  @range{-50,50}

  Y Position of Point, Relative to fDistY lateral coordinate, aligned
with the ego coordinates axes
Relative to reference point and
orientation

  */
  float32 fPosY;
  /**
  @unit{cm}

  @range{0,65535}

  Standard Deviation of X Position of Point in cm

  */
  uint16 uiPosXStd;
  /**
  @unit{cm}

  @range{0,65535}

  Standard Deviation of Y Position of Point in cm

  */
  uint16 uiPosYStd;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_TP_T_OBJ_SHAPE_POINT_COORD_H_

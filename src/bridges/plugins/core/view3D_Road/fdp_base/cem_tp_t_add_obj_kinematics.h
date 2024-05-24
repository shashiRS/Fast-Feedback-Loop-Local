// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_TP_T_ADD_OBJ_KINEMATICS_H_
#define FDP_BASE_CEM_TP_T_ADD_OBJ_KINEMATICS_H_

#include "Platform_Types.h"

namespace fdp_base {

/// Additional kinematic object properties.
struct CEM_TP_t_AddObjKinematics {
  /**
  @unit{m}

  @range{-250,250}

  Vertical (parallel to ego-vehicle z-axis) distance to object
kinematic point. Positive in direction up.

  */
  float32 fDistZ;
  /**
  @unit{m}

  @range{-1,100}

  Standard deviation of fDistZ. Set to a negative value (-1.0)
if invalid.

  */
  float32 fDistZStd;
  /**
  @unit{rad}

  Yaw Angle of object. Positive counterclockwise min:0

  */
  float32 fYaw;
  /**
  @unit{rad}

  @range{-1,2*Pi}

  Standard deviation of fYaw. Negative if invalid. min:0

  */
  float32 fYawStd;
  /**
  @unit{rad/s}

  Yaw Rate of object (only based on motion model). Positive counterclockwise

  */
  float32 fYawRate;
  /**
  @unit{rad/s}

  Standard deviation of fYawRate. Negative if invalid. min:0

  */
  float32 fYawRateStd;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_TP_T_ADD_OBJ_KINEMATICS_H_

// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_TP_T_OBJ_KINEMATICS_H_
#define FDP_BASE_CEM_TP_T_OBJ_KINEMATICS_H_

#include "Platform_Types.h"

namespace fdp_base {

/// Kinematic object properties
struct CEM_TP_t_ObjKinematics {
  /**
  @unit{m}

  @range{-250,250}

  Longitudinal (parallel to ego-vehicle x-axis) distance to object
kinematic point.

  */
  float32 fDistX;
  /**
  @unit{m}

  @range{-1,250}

  Standard deviation of fDistX. Negative if invalid. Default set to negative value (-1.0).

  */
  float32 fDistXStd;
  /**
  @unit{m}

  @range{-250,250}

  Lateral (parallel to ego-vehicle y-axis) distance to object kinematic
point. Positive to the left.

  */
  float32 fDistY;
  /**
  @unit{m}

  @range{-1,250}

  Standard deviation of fDistY. Negative if invalid. Default set to negative value (-1.0).

  */
  float32 fDistYStd;
  /**
  @unit{m/s}

  @range{-128,128}

  Relative longitudinal velocity: Difference in longitudinal velocity
of the host and the object. This value shall be positive when
the object velocity is higher than the host velocity (opening
velocity).

  */
  float32 fVrelX;
  /**
  @unit{m/s}

  @range{-128,128}

  Relative longitudinal velocity standard deviation: Set to negative
value (-1.0) if signal not available.

  */
  float32 fVrelXStd;
  /**
  @unit{m/s}

  @range{-128,128}

  Relative lateral velocity: Velocity of the object in lateral
direction, relative to the host vehicle. This value shall be
positive when the object is moving towards the left.

  */
  float32 fVrelY;
  /**
  @unit{m/s}

  @range{-128,128}

  Relative lateral velocity standard deviation: Set to negative
value (-1.0) if signal not available.

  */
  float32 fVrelYStd;
  /**
  @unit{m/s^2}

  @range{-25,25}

  Relative longitudinal acceleration: Difference in longitudinal
acceleration of the host and the object. This value shall be
positive when the object acceleration is higher than the host
acceleration.

  */
  float32 fArelX;
  /**
  @unit{m/s^2}

  @range{-25,25}


Relative longitudinal acceleration standard deviation: Set to
negative value (-1.0) if signal not available.

  */
  float32 fArelXStd;
  /**
  @unit{m/s^2}

  @range{-25,25}

  Relative lateral acceleration: Acceleration in lateral direction
of the object, relative to the host vehicle. This value shall
be positive when the object accelerates towards the right.

  */
  float32 fArelY;
  /**
  @unit{m/s^2}

  @range{-25,25}


Relative lateral acceleration standard deviation: Set to negative
value (-1.0) if signal not available.

  */
  float32 fArelYStd;
  /**
  @unit{m/s}

  @range{-128,128}

  Absolute longitudinal velocity of the object: This value shall
be positive when the object velocity is points to the same direction
like the host vehicle velocity.

  */
  float32 fVabsX;
  /**
  @unit{m/s}

  @range{-128,128}

  Absolute longitudinal velocity standard deviation. Set to negative
value (-1.0) if signal not available.

  */
  float32 fVabsXStd;
  /**
  @unit{m/s}

  @range{-128,128}

  Absolute lateral velocity of the object: Velocity of the object
in lateral direction. This value shall be positive when the
object is moving towards the left.

  */
  float32 fVabsY;
  /**
  @unit{m/s}

  @range{-128,128}

  Absolute lateral velocity standard deviation: Set to negative
value (-1.0) if signal not available.

  */
  float32 fVabsYStd;
  /**
  @unit{m/s^2}

  @range{-25,25}

  Absolute longitudinal acceleration: Longitudinal acceleration
of the object.

  */
  float32 fAabsX;
  /**
  @unit{m/s^2}

  @range{-25,25}

  Absolute  longitudinal acceleration standard deviation: Set to
negative value (-1.0) if signal not available.

  */
  float32 fAabsXStd;
  /**
  @unit{m/s^2}

  @range{-25,25}

  Absolute lateral acceleration: Acceleration in lateral direction
of the object. This value shall be positive when the object
accelerates towards the left.

  */
  float32 fAabsY;
  /**
  @unit{m/s^2}

  @range{-25,25}

  Absolute lateral acceleration standard deviation: Set to negative
value (-1.0) if signal not available.

  */
  float32 fAabsYStd;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_TP_T_OBJ_KINEMATICS_H_

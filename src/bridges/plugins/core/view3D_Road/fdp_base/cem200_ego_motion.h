// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM200_EGO_MOTION_H_
#define FDP_BASE_CEM200_EGO_MOTION_H_

#include "fdp_base/cem_long_motion_state.h"
#include "fdp_base/cem_normal_distribute_f32.h"
#include "fdp_base/cem_vector3_nd32.h"
#include "fdp_base/sensor_bitfield.h"

namespace fdp_base {

/// Ego motion estimation in the local coordinate system
struct Cem200EgoMotion {
  /**
  @unit{m/s}

  The velocites of the car as a spatial (3D) vector. For a planar
movement model, the z-entry should have a negative standard
deviation.

  */
  CEM_Vector3_ND32 velocityTrans;
  /**
  @unit{m/s}

  The angular velocites of the car as a spatial (3D) vector. For
a planar movement model, only the z-entry should be filled and
the other entries need a negative standard deviation.

  */
  CEM_Vector3_ND32 velocityAng;
  /**
  @unit{m/s}

  The translational accelerations of the car as a spatial (3D)
vector. For a planar movement model, the z-entry should have
a negative standard deviation.

  */
  CEM_Vector3_ND32 accelerationTrans;
  /**
  @unit{m/s}

  The angular accelerations of the car as a spatial (3D) vector.
For a planar movement model, only the z-entry should be filled
and the other entries need a negative standard deviation.

  */
  CEM_Vector3_ND32 accelerationAng;
  /**
  @unit{rad}

  The angle between the X-axis and the velocity vector that represents
the instantaneous vehicle velocity at that point along the path.

  */
  CEM_NormalDistribute_f32 sideSlipAngle;
  /**
  Motion State of Vehicle

  */
  CEM_LongMotionState motionState;
  /**
  List of sensors contributing to fusion

  */
  SensorBitfield contributingSensors;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM200_EGO_MOTION_H_

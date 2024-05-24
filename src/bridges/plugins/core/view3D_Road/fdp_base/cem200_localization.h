// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM200_LOCALIZATION_H_
#define FDP_BASE_CEM200_LOCALIZATION_H_

#include "fdp_base/cem_global_pose.h"
#include "fdp_base/cem_local_pose.h"
#include "fdp_base/cem_normal_distribute_f32.h"
#include "fdp_base/sensor_bitfield.h"

namespace fdp_base {

/// vehicle pose two different world coordinate systems (local and global)
struct Cem200Localization {
  /**
  List of sensors contributing to fusion

  */
  SensorBitfield contributingSensors;
  CEM_LocalPose localPose;
  CEM_GlobalPose globalPose;
  /**
  @unit{1/m}

  The future trajectory designed by a circle. Disregarding the
effects of a steered rear-axis it is assumed, that the turn-center
for the trajectory is on the rear-axis. It is then defined by
the radius of the circle. Numerically more interesting is the
curvature, the inverse of the radius, which is why it is taken
here.
The polarity (the sign) of the curvature is indicating,
to which direction the vehicle moves (iff the curvature is not
zero). The vehicle moves to the left, when the curvature is
positive and it moves to the right, when the curvature is negative.
This is in line with the yaw rate, which is defined (mathematically)
positive around the z-axis of the vehicle coordinate system.
Remark: to estimate the direction of the future trajectory, the
motion state can be used.

  */
  CEM_NormalDistribute_f32 curvatureC0;
  /**
  @unit{1/ms}

  First derivative of the future trajectory (curvatureC0).

  */
  CEM_NormalDistribute_f32 curvatureC1;
  /**
  Covered Distance from start of ignition cycle. This is a scalar
value and is valid in any coordinate system unit:exp: m

  */
  CEM_NormalDistribute_f32 coveredDistance;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM200_LOCALIZATION_H_

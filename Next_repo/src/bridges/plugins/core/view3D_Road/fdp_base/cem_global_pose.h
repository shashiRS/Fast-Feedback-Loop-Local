// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_GLOBAL_POSE_H_
#define FDP_BASE_CEM_GLOBAL_POSE_H_

#include "fdp_base/cem_normal_distribute_f32.h"

namespace fdp_base {

/// vehicle localization in an absolute world COS (WGS84)
struct CEM_GlobalPose {
  /**
  WGS84 geographic latitude. [-pi/2, pi/2] : angle between the
equatorial plane and the normal vector of the ellipsoid at the
considered point unit:exp: rad

  */
  CEM_NormalDistribute_f32 latitude;
  /**
  WGS84 geographic longitude. [-pi, pi] : angle in east-west direction
(origin at Greenwich meridian) unit:exp: rad

  */
  CEM_NormalDistribute_f32 longitude;
  /**
  Vehicle heading angle unit:exp: rad

  */
  CEM_NormalDistribute_f32 heading;
  /**
  Elevation above mean sea level unit:exp: m

  */
  CEM_NormalDistribute_f32 elevation;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_GLOBAL_POSE_H_

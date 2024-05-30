// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_LOCAL_POSE_H_
#define FDP_BASE_CEM_LOCAL_POSE_H_

#include "fdp_base/cem_rot_par_zyx_nd32.h"
#include "fdp_base/cem_vector3_nd32.h"

namespace fdp_base {

/// The pose - given in the local coordinate system. Its origin and orientation relative to the car is defined by these
/// parameters. Remark that the angles are parameters which can not be evaluated in an coordinate system directly as the
/// paremterize this transformation directly. For small angles however they might yield good approximations.
struct CEM_LocalPose {
  /**
  @unit{m}

  The position of the reference point.

  */
  CEM_Vector3_ND32 position;
  /**
  @unit{rad}

  The orientation of the reference point.

  */
  CEM_RotParZYX_ND32 rotationParameters;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_LOCAL_POSE_H_

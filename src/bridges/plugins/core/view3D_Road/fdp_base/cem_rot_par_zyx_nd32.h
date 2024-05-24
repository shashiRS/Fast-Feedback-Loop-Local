// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_ROT_PAR_ZYX_ND32_H_
#define FDP_BASE_CEM_ROT_PAR_ZYX_ND32_H_

#include "fdp_base/cem_normal_distribute_f32.h"

namespace fdp_base {

/// 3D rotation parameters for a spatial vector transformation from one coordinate system to the other. It follows the
/// rotation order ZY'X'', which is defined in DIN 8855, before DIN 70000 and is the standard rotation order in
/// automotive industry.
struct CEM_RotParZYX_ND32 {
  /**
  The yaw angle, i.e. the angular displacement about z-axis of
the reference coordinate system. It is the 1st rotation axis
from reference system to the new system (ZY'X''-rotation order).

  */
  CEM_NormalDistribute_f32 yaw;
  /**
  The pitch angle, i.e.. the angular displacement about y-axis
(y'-axis) of the first intermediate coordinate system. It is
the 2nd rotation axis from reference system to the new system
(ZY'X''-rotation order).

  */
  CEM_NormalDistribute_f32 pitch;
  /**
  The roll angle, i.e.. the angular displacement about x-axis (x''-axis)
of the second intermediate coordinate system. It is the 3rd
rotation axis from reference system to the new system  (ZY'X''-rotation
order). The x''-axis is also the x-axis of the new system.

  */
  CEM_NormalDistribute_f32 roll;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_ROT_PAR_ZYX_ND32_H_

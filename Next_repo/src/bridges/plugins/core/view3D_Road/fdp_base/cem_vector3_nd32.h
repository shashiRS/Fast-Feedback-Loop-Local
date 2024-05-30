// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_VECTOR3_ND32_H_
#define FDP_BASE_CEM_VECTOR3_ND32_H_

#include "fdp_base/cem_normal_distribute_f32.h"

namespace fdp_base {

/// 3D vector of CEM_NormalDistribute_f32 type entries
struct CEM_Vector3_ND32 {
  /**
  x coordinate / 1st entry of the the vector

  */
  CEM_NormalDistribute_f32 x;
  /**
  y coordinate / 1st entry of the the vector

  */
  CEM_NormalDistribute_f32 y;
  /**
  z coordinate / 1st entry of the the vector

  */
  CEM_NormalDistribute_f32 z;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_VECTOR3_ND32_H_

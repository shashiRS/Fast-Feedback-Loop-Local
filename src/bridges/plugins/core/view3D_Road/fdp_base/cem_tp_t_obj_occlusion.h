// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_TP_T_OBJ_OCCLUSION_H_
#define FDP_BASE_CEM_TP_T_OBJ_OCCLUSION_H_

#include "tp_t_obj_occlusion_states.h"

namespace fdp_base {

/// Occlusion specific attributes of the Object
struct CEM_TP_t_ObjOcclusion {
  /**
  Describes if/how Object is covered by closer objects in sensing
direction. (TP_OBJ_ATTRIBUTE_OCCLUSION)

  */
  TP_t_ObjOcclusionStates eObjectOcclusion;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_TP_T_OBJ_OCCLUSION_H_

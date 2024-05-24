// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_TP_T_OBJ_DYNAMIC_PROPERTY_H_
#define FDP_BASE_CEM_TP_T_OBJ_DYNAMIC_PROPERTY_H_

#include "Platform_Types.h"
#include "cem_tp_t_obj_dynamic_properties.h"

namespace fdp_base {

/// Dynamic property of the traffic participant.
struct CEM_TP_t_ObjDynamicProperty {
  /**
  This signal indicates wether the object is moving, stationary
or oncoming, based on comparison of the longitudinal relative
velocity with the host vehicle velocity.

  */
  CEM_TP_t_ObjDynamicProperties eDynamicProperty;
  /**
  @range{0,100}

  Confidence of estimated dynamic property

  */
  uint8 uiDynConfidence;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_TP_T_OBJ_DYNAMIC_PROPERTY_H_

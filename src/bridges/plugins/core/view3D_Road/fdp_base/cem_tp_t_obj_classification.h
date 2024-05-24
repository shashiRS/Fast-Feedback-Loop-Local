// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_TP_T_OBJ_CLASSIFICATION_H_
#define FDP_BASE_CEM_TP_T_OBJ_CLASSIFICATION_H_

#include "Platform_Types.h"
#include "cem_tp_t_obj_classifications.h"

namespace fdp_base {

/// Object probability to be of a specific kind.
struct CEM_TP_t_ObjClassification {
  /**
  This signal indicates the class of the object.

  */
  CEM_TP_t_ObjClassifications eClassification;
  /**
  @range{0,100}

  Confidence of estimated object class

  */
  uint8 uiClassConfidence;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_TP_T_OBJ_CLASSIFICATION_H_

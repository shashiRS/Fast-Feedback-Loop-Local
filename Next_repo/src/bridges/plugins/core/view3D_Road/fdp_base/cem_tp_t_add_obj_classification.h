// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_TP_T_ADD_OBJ_CLASSIFICATION_H_
#define FDP_BASE_CEM_TP_T_ADD_OBJ_CLASSIFICATION_H_

#include "cem_percentage_t.h"
#include "cem_tp_t_obj_active_flag.h"

namespace fdp_base {

/// Object probabilities for specific object class.
struct CEM_TP_t_AddObjClassification {
  /**
  @unit{%}

  @range{0,100}

  Probability to be a Pedestrian

  */
  CEM_percentage_t probabilityPedestrian;
  /**
  @unit{%}

  @range{0,100}

  Probability to be a Cyclist

  */
  CEM_percentage_t probabilityCyclist;
  /**
  @unit{%}

  @range{0,100}

  Probability to be a Motorbike

  */
  CEM_percentage_t probabilityMotorbike;
  /**
  @unit{%}

  @range{0,100}

  Probability to be a Car

  */
  CEM_percentage_t probabilityCar;
  /**
  @unit{%}

  @range{0,100}

  Probability to be a Truck

  */
  CEM_percentage_t probabilityTruck;
  /**
  @unit{%}

  @range{0,100}

  Probability to be another kind of vehicle

  */
  CEM_percentage_t probabilityOtherVehicle;
  /**
  @unit{%}

  @range{0,100}

  Probability to be a road boundary

  */
  CEM_percentage_t probabilityRoadBoundary;
  /**
  @unit{%}

  @range{0,100}

  Probability to be an animal

  */
  CEM_percentage_t probabilityAnimal;
  /**
  @unit{%}

  @range{0,100}

  Probability to be an unknown object

  */
  CEM_percentage_t probabilityUnknown;
  /**
  Active flags for each individual object class. Specifies if an
object is intended to be delivered on the customer bus

  */
  CEM_TP_t_ObjActiveFlag eActiveFlags;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_TP_T_ADD_OBJ_CLASSIFICATION_H_

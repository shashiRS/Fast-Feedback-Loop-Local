// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_TP_T_OBJ_LIST_HEADER_H_
#define FDP_BASE_CEM_TP_T_OBJ_LIST_HEADER_H_

#include "fdp_base/cem_obj_number_t.h"
#include "fdp_base/sensor_bitfield.h"

namespace fdp_base {

/// Header Object list properties
struct CEM_TP_t_ObjListHeader {
  /**
  @range{0,100}

  Current number of tracked objects

  */
  CEM_ObjNumber_t iNumOfUsedObjects;
  /**
  @range{0,100}

  Array of object indexes sorted by assending longitudinal displacement

  */
  CEM_ObjNumber_t iSortedObjectList[100];
  /**
  In order to have the information of which sensor contributed
to a given output type (sensor or fused detection), a sensor
coding will be used having a compressed version in the form of
a 32-bit-field where each bit represents a sensor

  */
  SensorBitfield contributingSensors;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_TP_T_OBJ_LIST_HEADER_H_

// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_LINEAR_OBJECT_H_
#define FDP_BASE_LINEAR_OBJECT_H_

#include "Platform_Types.h"
#include "linear_object_type_enum.h"
#include "polyline.h"
#include "property_int_enum.h"
#include "sub_type_enum.h"

namespace fdp_base {

struct LinearObject {
  Polyline geometry;
  linearObjectTypeEnum linearObjectType;
  subTypeEnum subType;
  uint8 confidence;
  propertyIntEnum propertyInt;
  float32 propertyFloat;
};

} // namespace fdp_base

#endif // FDP_BASE_LINEAR_OBJECT_H_

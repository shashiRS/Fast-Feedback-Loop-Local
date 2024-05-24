// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_ASSOCIATION_RANGE_H_
#define FDP_BASE_ASSOCIATION_RANGE_H_

#include "Platform_Types.h"
#include "fdp_base/interpolation_type_enum.h"
#include "fdp_base/segment_type_enum.h"

namespace fdp_base {

struct AssociationRange {
  segmentTypeEnum segmentType;
  uint8 element;
  float32 start;
  interpolationTypeEnum interpolationType;
};

} // namespace fdp_base

#endif // FDP_BASE_ASSOCIATION_RANGE_H_

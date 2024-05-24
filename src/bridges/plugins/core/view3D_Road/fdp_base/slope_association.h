// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_SLOPE_ASSOCIATION_H_
#define FDP_BASE_SLOPE_ASSOCIATION_H_

#include "Platform_Types.h"
#include "fdp_base/association_range.h"

namespace fdp_base {

struct SlopeAssociation {
  sint8 value;
  AssociationRange range;
};

} // namespace fdp_base

#endif // FDP_BASE_SLOPE_ASSOCIATION_H_

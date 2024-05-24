// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_STATIC_ELEMENT_ATTRIBUTES_H_
#define FDP_BASE_STATIC_ELEMENT_ATTRIBUTES_H_

#include "fdp_base/cem_height_property.h"

namespace fdp_base {

/// Trace Info Of Obstacle
/// Timestamp of Birth
/// Array of IDs of Contributing Sensors
struct StaticElementAttributes {
  CEM_HeightProperty heightProperty;
};

} // namespace fdp_base

#endif // FDP_BASE_STATIC_ELEMENT_ATTRIBUTES_H_

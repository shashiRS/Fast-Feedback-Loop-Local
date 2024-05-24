// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_EGO_VEHICLE_KINEMATICS_H_
#define FDP_BASE_CEM_EGO_VEHICLE_KINEMATICS_H_

#include "Platform_Types.h"
#include "fdp_base/cem200_ego_motion.h"
#include "fdp_base/cem200_localization.h"
#include "fdp_base/cem200_states.h"
#include "fdp_base/cem_signal_header_t.h"

namespace fdp_base {

struct CEM_EgoVehicleKinematics {
  /**
  The timestep of the signal Header signalizes the time, for when
the given output is valid. This time can be the last input time
of any sensor input, but also times after this (e.g. the processing
time itself, or any requested end time). This means, the output
would be extrapolated.

  */
  CEM_SignalHeader_t sigHeader;
  /**
  State of the EML component

  */
  Cem200States state[23];
  /**
  source of ego motion : sensors used to estimate current egomotion
and the vehicle pose

  */
  uint32 emlInputSource;
  /**
  Ego motion velocity estimation in the vehicle coordinate system
(body fixed coordinate system at the rear axis on ground level)

  */
  Cem200EgoMotion egoMotion;
  /**
  vehicle pose two different world coordinate systems (local and
global)

  */
  Cem200Localization localization;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_EGO_VEHICLE_KINEMATICS_H_

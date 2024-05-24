// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_VAL_OUTPUT_DATA_PROVIDER_H_
#define FDP_BASE_CEM_VAL_OUTPUT_DATA_PROVIDER_H_

#include "fdp_base/cem_signal_header_t.h"
#include "fdp_base/cem_val_ifov_t.h"
#include "fdp_base/cem_val_sensor_degradation_t.h"

namespace fdp_base {

/// VAL output port
struct CEM_VAL_OutputDataProvider {
  /**
  Signal header with common signal information

  */
  CEM_SignalHeader_t sSigHeader;
  /**
  Structure represents the ideal field of view of the sensor setup
attached to the ego-vehicle

  */
  CEM_VAL_IFOV_t VAL_IFOV;
  /**
  Structure represents the degradation field of view of the sensor
setup attached to the ego-vehicle

  */
  CEM_VAL_SensorDegradation_t VAL_SensorDegradation;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_VAL_OUTPUT_DATA_PROVIDER_H_

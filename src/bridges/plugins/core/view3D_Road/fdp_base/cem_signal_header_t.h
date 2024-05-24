// Attention, this file is generated!

#ifdef ECO_C_TYPES_USED
#ifndef ECO_ALLOW_INTERFACE_MIXING
#error eco C and C++ types mixed
#endif
#endif
#define ECO_CPP_TYPES_USED

#ifndef FDP_BASE_CEM_SIGNAL_HEADER_T_H_
#define FDP_BASE_CEM_SIGNAL_HEADER_T_H_

#include "fdp_base/cem_algo_cycle_counter_t.h"
#include "fdp_base/cem_algo_data_time_stamp_t.h"
#include "fdp_base/cem_algo_signal_state_t.h"

namespace fdp_base {

/// Common header for all structured data types
struct CEM_SignalHeader_t {
  /**
  Time stamp of the signal (for SIP: T0 timestamp of input signal
, otherwise: T7 timestamp to which signal has been adjusted)

  */
  CEM_AlgoDataTimeStamp_t uiTimeStamp;
  /**
  Deprecated, not used in CEM. Should be set to 0.

  */
  CEM_AlgoCycleCounter_t uiMeasurementCounter;
  /**
  Rolling counter of source component. Will be incremented in every
call to the exec method of the component.

  */
  CEM_AlgoCycleCounter_t uiCycleCounter;
  /**
  Validity status of the signal. If not set to AL_SIG_STATE_OK,
then do not evaluate the content of the associated signal!

  */
  CEM_AlgoSignalState_t eSigStatus;
};

} // namespace fdp_base

#endif // FDP_BASE_CEM_SIGNAL_HEADER_T_H_

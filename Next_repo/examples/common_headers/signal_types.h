/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * ========================== NEXT Project ==================================
 */

#ifndef NEXT_SIGNAL_TYPES_H
#define NEXT_SIGNAL_TYPES_H

namespace next {
namespace examples {
namespace signal_types {

struct SignalHeader_64 {
  uint64_t uiTimeStamp;
  uint16_t uiMeasurementCounter;
  uint16_t uiCycleCounter;
  uint8_t eSigStatus;
};

struct generic_signal_64 {
  uint32_t uiVersionNumber;
  SignalHeader_64 sSigHeader;
  uint32_t payload;
};

} // namespace signal_types
} // namespace examples
} // namespace next

#endif // NEXT_SIGNAL_TYPES_H

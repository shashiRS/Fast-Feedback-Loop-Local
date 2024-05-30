#ifndef VEHDYN_T_H
#define VEHDYN_T_H

namespace VehDyn_t_V_1 {
static const uint32_t VERSION = 1;
typedef uint8_t a_reserve_array_t[3];
typedef struct {
  uint32_t uiTimeStamp;
  uint16_t uiMeasurementCounter;
  uint16_t uiCycleCounter;
  uint8_t eSigStatus;
  a_reserve_array_t a_reserve;
} SignalHeader_t;
typedef struct {
  uint32_t versionNumber;
  SignalHeader_t sigHeader;
  float dummy_float;
  int dummy_int;
} VehDyn_t;
} // namespace VehDyn_t_V_1
namespace VehDyn_t_V_2 {
static const uint32_t VERSION = 2;
typedef uint8_t a_reserve_array_t[3];
typedef struct {
  uint32_t uiTimeStamp;
  uint16_t uiMeasurementCounter;
  uint16_t uiCycleCounter;
  uint8_t eSigStatus;
  a_reserve_array_t a_reserve;
} SignalHeader_t;
typedef struct {
  uint32_t versionNumber;
  SignalHeader_t sigHeader;
  float dummy_float;
  int dummy_int;
  bool dummy_bool;
} VehDyn_t;
} // namespace VehDyn_t_V_2
namespace VehDyn_t_V_Gen {
static const uint32_t VERSION = 15001372;
typedef uint8_t a_reserve_array_t[3];
typedef struct {
  uint32_t uiTimeStamp;
  uint16_t uiMeasurementCounter;
  uint16_t uiCycleCounter;
  uint8_t eSigStatus;
  a_reserve_array_t a_reserve;
} SignalHeader_t;
typedef struct {
  uint32_t versionNumber;
  SignalHeader_t sigHeader;
  float dummy_float;
  int dummy_int;
  uint32_t uiSourceVersion;
  unsigned long ulSourceSize;
  bool dummy_bool;
} VehDyn_t;
} // namespace VehDyn_t_V_Gen
#endif /*VEHDYN_T_H*/

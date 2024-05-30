/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: Next_SDK
 * ========================== NEXT Project ==================================
 */
/**
 * @file     cycle_event.h
 * @brief    cycle event is the event which would send and receive cycles
 *
 **/
#ifndef NEXT_SDK_CYCLE_EVENT_H_
#define NEXT_SDK_CYCLE_EVENT_H_

#include <string>
#include <vector>

#include "../event_base/base_event_binary_blocking.h"
#include "../sdk_config.hpp"
#include "../sdk_macros.h"

namespace next {
namespace sdk {
namespace events {

//!@brief possible states of a Cycle
enum class CycleState : uint8_t {
  no_cycle = 0,
  start = 0b001,
  end = 0b010,
  body = 0b100,
  start_end = start | end,
  start_body = start | end,
  body_end = body | end,
  start_body_end = body | start | end
};

const std::unordered_map<CycleState, std::string> kCycleStatus = {{CycleState::no_cycle, "NO_CYCLE"},
                                                                  {CycleState::start, "CYCLE_START"},
                                                                  {CycleState::end, "CYCLE_END"},
                                                                  {CycleState::body, "CYCLE_BODY"},
                                                                  {CycleState::start_end, "CYCLE_START_END"},
                                                                  {CycleState::start_body, "CYCLE_START_BODY"},
                                                                  {CycleState::body_end, "CYCLE_BODY_END"},
                                                                  {CycleState::start_body_end, "CYCLE_START_BODY_END"}};

const std::unordered_map<std::string, CycleState> kCycleStatusStrToEnum = {
    {"NO_CYCLE", CycleState::no_cycle},
    {"CYCLE_START", CycleState::start},
    {"CYCLE_END", CycleState::end},
    {"CYCLE_BODY", CycleState::body},
    {"CYCLE_START_END", CycleState::start_end},
    {"CYCLE_START_BODY", CycleState::start_body},
    {"CYCLE_BODY_END", CycleState::body_end},
    {"CYCLE_START_BODY_END", CycleState::start_body_end}};

struct Cycle {
  Cycle(uint32_t id, CycleState state) : cycle_id(id), cycle_state(state) {}
  Cycle() = default;
  uint32_t cycle_id{0u};
  CycleState cycle_state{CycleState::no_cycle};
};

NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS
class DECLSPEC_nextsdk CycleEvent : public BaseEventBinaryBlocking<Cycle> {
  NEXT_RESTORE_WARNINGS_WINDOWS

public:
  CycleEvent(std::string name);

protected:
  payload_type serializeEventPayload(const Cycle &message) const override;
  Cycle deserializeEventPayload(const payload_type &buf) const override;
};

} // namespace events
} // namespace sdk
} // namespace next

#endif // NEXT_SDK_CYCLE_EVENT_H_

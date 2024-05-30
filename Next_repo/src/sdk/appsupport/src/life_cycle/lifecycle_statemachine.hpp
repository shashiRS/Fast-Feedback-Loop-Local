/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     Lifecycle_Statemachine.hpp
 * @brief    Including funcctions for lifecycle handling
 *
 * Includ
 *
 */

#ifndef LIFECYCLE_STATEMACHINE_H_
#define LIFECYCLE_STATEMACHINE_H_

#include <exception>
#include <memory>
#include <string>

#include "include_boost.hpp"

#include "next/appsupport/lifecycle/lifecycle_base.hpp"

namespace next {
namespace appsupport {
namespace lifecycleStatemachine {
// Events
struct EVENT_RESET {};
struct EVENT_SHUTDOWN {};
struct EVENT_ERROR {};

struct EVENT_CONFIG_INPUT {};

struct EVENT_START_EXECUTION {};
struct EVENT_EXIT_EXECUTION {};

// this will be the new BASE_STATE:
struct LFCBaseState {
  LFCBaseState(EState id = EState::UNKNOWN_STATE) : stateID(id) {}

public:
  EState stateID;
};

struct INIT_STATE : public boost::msm::front::state<LFCBaseState> {
  INIT_STATE() { stateID = EState::INIT_STATE; }
  // every (optional) entry/exit methods get the event passed.
  template <class Event, class FSM>
  void on_entry(Event const &, FSM &fsm) {
    fsm.on_entering_state = stateID;
    fsm.lfb->privateInit();
  }
  template <class Event, class FSM>
  void on_exit(Event const &, FSM &) {}
};

struct RESET_STATE : public boost::msm::front::state<LFCBaseState> {
  RESET_STATE() { stateID = EState::RESET_STATE; }
  // every (optional) entry/exit methods get the event passed.
  template <class Event, class FSM>
  void on_entry(Event const &, FSM &fsm) {
    fsm.on_entering_state = stateID;
    fsm.lfb->privateReset();
  }
  template <class Event, class FSM>
  void on_exit(Event const &, FSM &) {}
};

struct CONFIG_STATE : public boost::msm::front::state<LFCBaseState> {
  CONFIG_STATE() { stateID = EState::CONFIG_STATE; }
  // every (optional) entry/exit methods get the event passed.
  template <class Event, class FSM>
  void on_entry(Event const &, FSM &fsm) {
    fsm.on_entering_state = stateID;
    fsm.lfb->privateConfig();
  }
  template <class Event, class FSM>
  void on_exit(Event const &, FSM &) {}
};

struct CONFIG_OUTPUT : public boost::msm::front::state<LFCBaseState> {
  CONFIG_OUTPUT() { stateID = EState::CONFIG_OUTPUT; }
  // every (optional) entry/exit methods get the event passed.
  template <class Event, class FSM>
  void on_entry(Event const &, FSM &fsm) {
    fsm.on_entering_state = stateID;
    fsm.lfb->privateConfigOutput();
  }
  template <class Event, class FSM>
  void on_exit(Event const &, FSM &) {}
};

struct CONFIG_INPUT : public boost::msm::front::state<LFCBaseState> {
  CONFIG_INPUT() { stateID = EState::CONFIG_INPUT; }
  // every (optional) entry/exit methods get the event passed.
  template <class Event, class FSM>
  void on_entry(Event const &, FSM &fsm) {
    fsm.on_entering_state = stateID;
    fsm.lfb->privateConfigInput();
  }
  template <class Event, class FSM>
  void on_exit(Event const &, FSM &) {}
};

struct READY_STATE : public boost::msm::front::state<LFCBaseState> {
  READY_STATE() { stateID = EState::READY_STATE; }
  // every (optional) entry/exit methods get the event passed.
  template <class Event, class FSM>
  void on_entry(Event const &, FSM &fsm) {
    fsm.on_entering_state = stateID;
    fsm.lfb->privateReady();
  }

  template <class Event, class FSM>
  void on_exit(Event const &, FSM &) {}
};

struct EXECUTE_STATE : public boost::msm::front::state<LFCBaseState> {
  EXECUTE_STATE() { stateID = EState::EXECUTE_STATE; }
  // every (optional) entry/exit methods get the event passed.
  template <class Event, class FSM>
  void on_entry(Event const &, FSM &fsm) {
    fsm.on_entering_state = stateID;
    fsm.lfb->privateExecute();
  }
  template <class Event, class FSM>
  void on_exit(Event const &, FSM &) {}
};

struct SHUTDOWN_STATE : public boost::msm::front::state<LFCBaseState> {
  SHUTDOWN_STATE() { stateID = EState::SHUTDOWN_STATE; }
  // every (optional) entry/exit methods get the event passed.
  template <class Event, class FSM>
  void on_entry(Event const &, FSM &fsm) {
    fsm.on_entering_state = stateID;
    fsm.lfb->privateShutdown();
  }
  template <class Event, class FSM>
  void on_exit(Event const &, FSM &) {}
};

struct ERROR_STATE : public boost::msm::front::state<LFCBaseState> {
  ERROR_STATE() { stateID = EState::ERROR_STATE; }
  // every (optional) entry/exit methods get the event passed.
  template <class Event, class FSM>
  void on_entry(Event const &, FSM &fsm) {
    fsm.on_entering_state = stateID;
    fsm.lfb->privateError("TODO: need better Error handling here");
  }
  template <class Event, class FSM>
  void on_exit(Event const &, FSM &) {}
};

struct LifeCycleStateMachineImpl
    : public boost::msm::front::state_machine_def<LifeCycleStateMachineImpl, LFCBaseState> {
  LifeCycleStateMachineImpl(LifecycleBase *lfb_) : lfb(lfb_) {}

  typedef INIT_STATE initial_state;

  // clang-format off
    struct transition_table
        : boost::mpl::vector<
//                     Source           Event                          Target           Action                       Guard
boost::msm::front::Row<INIT_STATE,      boost::msm::front::none,     RESET_STATE,       boost::msm::front::none,     boost::msm::front::none>,
boost::msm::front::Row<RESET_STATE,     boost::msm::front::none,     CONFIG_STATE,      boost::msm::front::none,     boost::msm::front::none>,
boost::msm::front::Row<CONFIG_STATE,    boost::msm::front::none,     CONFIG_OUTPUT,     boost::msm::front::none,     boost::msm::front::none>,
boost::msm::front::Row<CONFIG_OUTPUT,   EVENT_CONFIG_INPUT,          CONFIG_INPUT,      boost::msm::front::none,     boost::msm::front::none>,
boost::msm::front::Row<CONFIG_OUTPUT,   EVENT_RESET,                 RESET_STATE,       boost::msm::front::none,     boost::msm::front::none>,
boost::msm::front::Row<CONFIG_INPUT,    boost::msm::front::none,     READY_STATE,       boost::msm::front::none,     boost::msm::front::none>,
boost::msm::front::Row<READY_STATE,     EVENT_RESET,                 RESET_STATE,       boost::msm::front::none,     boost::msm::front::none>,
boost::msm::front::Row<CONFIG_OUTPUT,   EVENT_SHUTDOWN,              SHUTDOWN_STATE,    boost::msm::front::none,     boost::msm::front::none>,
boost::msm::front::Row<READY_STATE,     EVENT_SHUTDOWN,              SHUTDOWN_STATE,    boost::msm::front::none,     boost::msm::front::none>,
boost::msm::front::Row<READY_STATE,     EVENT_START_EXECUTION,       EXECUTE_STATE,     boost::msm::front::none,     boost::msm::front::none>,
boost::msm::front::Row<EXECUTE_STATE,   EVENT_EXIT_EXECUTION,        READY_STATE,       boost::msm::front::none,     boost::msm::front::none>,
boost::msm::front::Row<INIT_STATE,      EVENT_ERROR,                 ERROR_STATE,       boost::msm::front::none,     boost::msm::front::none>,
boost::msm::front::Row<RESET_STATE,     EVENT_ERROR,                 ERROR_STATE,       boost::msm::front::none,     boost::msm::front::none>,
boost::msm::front::Row<CONFIG_STATE,    EVENT_ERROR,                 ERROR_STATE,       boost::msm::front::none,     boost::msm::front::none>,
boost::msm::front::Row<READY_STATE,     EVENT_ERROR,                 ERROR_STATE,       boost::msm::front::none,     boost::msm::front::none>,
boost::msm::front::Row<EXECUTE_STATE,   EVENT_ERROR,                 ERROR_STATE,       boost::msm::front::none,     boost::msm::front::none>,
boost::msm::front::Row<ERROR_STATE,     boost::msm::front::none,     SHUTDOWN_STATE,    boost::msm::front::none,     boost::msm::front::none>            
                            > {};
  // clang-format on
  // Replaces the default no-transition response.
  template <class FSM, class Event>
  void no_transition(Event const &event, [[maybe_unused]] FSM &fsm, int stateID) {
    // if state is Execute but RESET or SHUTDOWN Event was triggered, than an external timeout thread will be started
    if (stateID == (int)EState::EXECUTE_STATE) {
      if (typeid(event).hash_code() == typeid(EVENT_RESET).hash_code()) {
        lfb->waitTimeoutBeforeProcessEvent("RESET");
      } else if (typeid(event).hash_code() == typeid(EVENT_SHUTDOWN).hash_code()) {
        lfb->waitTimeoutBeforeProcessEvent("SHUTDOWN");
      }
    }
    //  perform error message that this transition is no allowed
  }
  template <class FSM, class Event>
  void exception_caught(Event const &, FSM &, std::exception &e) {
    next::sdk::logger::warn(__FILE__, "[LifeCycleStateMachineImpl] Exception : {}", e.what());
  }

public:
  LifecycleBase *lfb;
  EState on_entering_state = EState::UNKNOWN_STATE;
};

struct LifeCycleStateMachine : public boost::msm::back::state_machine<LifeCycleStateMachineImpl> {

  LifeCycleStateMachine(LifecycleBase *lfb_) : boost::msm::back::state_machine<LifeCycleStateMachineImpl>(lfb_) {}

  EState getCurrentState() {
    auto state = get_state_by_id(current_state()[0]);

    if (state == nullptr || state == NULL) {
      return EState::UNKNOWN_STATE;
    } else {
      return state->stateID;
    }
  }
  template <class FSM, class Event>
  void exception_caught(Event const &, FSM &, std::exception &e) {
    next::sdk::logger::warn(__FILE__, "[LifeCycleStateMachine] Exception : {}", e.what());
  }
  EState getOnEnteringState() { return on_entering_state; }
};

} // namespace lifecycleStatemachine
} // namespace appsupport
} // namespace next

#endif // LIFECYCLE_STATEMACHINE_H_

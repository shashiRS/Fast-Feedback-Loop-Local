/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     state_error.hpp
 * @brief    Error State implementation for the Player State machine
 *
 **/

#ifndef STATE_ERROR_H_
#define STATE_ERROR_H_

#ifdef __linux__
#pragma GCC system_header // disable all warnings from boost lib for gcc++
#endif
#ifdef _WIN32
#pragma warning(push, 0) // disable all warnings from boost lib for vc++
#endif
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/state_machine_def.hpp>
#ifdef _WIN32
#pragma warning(pop)
#endif

#include <next/sdk/logger/logger.hpp>

#include "../error_codes.hpp"
#include "state_machine_events.hpp"

class StateError : public boost::msm::front::state<> {
public:
  StateError(){};
  template <class Event, class FSM>
  void on_entry(Event const &e, FSM &fsm) {

    next::sdk::logger::warn(__FILE__, "[PlayerStateMachine] : entry state ERROR using [{0}]", e.name);

    fsm.current_state_ = ESTATES::E_ERROR;
    event_ERROR event = (event_ERROR)e;
    switch (event.error) {

    case PlayerError::E_OpenFile: {
      fsm.SendMetaEvent(next::control::events::PlayerState::ON_ERROR);
      fsm.process_event(event_handled_error());
      break;
    }
    default:
      fsm.process_event(event_unhandled_error());
    }
  }

  template <class Event, class FSM>
  void on_exit(Event const &e, FSM &) {
    next::sdk::logger::warn(__FILE__, "[PlayerStateMachine] : exit state ERROR using [{0}]", e.name);
  }
};

#endif // STATE_ERROR_H_

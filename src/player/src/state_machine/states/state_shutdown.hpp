/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     state_shutdown.hpp
 * @brief    Shutdown State implementation for the Player State machine
 *
 **/

#ifndef STATE_SHUTDOWN_H_
#define STATE_SHUTDOWN_H_

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

#include <next/control/event_types/player_command.h>
#include <next/control/event_types/player_state_event.h>
#include <next/sdk/logger/logger.hpp>

#include "state_machine_events.hpp"

class StateShutdown : public boost::msm::front::state<> {
public:
  StateShutdown(){};

  template <class Event, class FSM>
  void on_entry(Event const &e, FSM &fsm) {
    fsm.DisableStatePublish();
    next::sdk::logger::debug(__FILE__, "[PlayerStateMachine] : entry state SHUTDOWN using [{0}]", e.name);
    fsm.SendMetaEvent(next::control::events::PlayerState::ON_SHUTDOWN);
    if (fsm.current_state_ == ESTATES::E_PLAY) {
      fsm.process_event(command_PAUSE());
      fsm.process_event(command_CLOSE());
    }
    if (fsm.current_state_ == ESTATES::E_READY) {
      fsm.process_event(command_CLOSE());
    }
    fsm.current_state_ = ESTATES::E_SHUTDOWN;
  }

  template <class Event, class FSM>
  void on_exit(Event const &e, FSM &) {
    next::sdk::logger::info(__FILE__, "[PlayerStateMachine] : exit state SHUTDOWN using event [{0}]", e.name);
  }

private:
};

#endif // STATE_SHUTDOWN_H_

/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: Next-Player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     state_ready.hpp
 * @brief    Ready State implementation for the Player State machine
 *
 **/
#ifndef STATE_READY_H
#define STATE_READY_H

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
#include <fstream>
#include <iostream>

#include "state_machine_events.hpp"

class StateReady : public boost::msm::front::state<> {
public:
  template <class Event, class FSM>
  void on_entry(Event const &e, FSM &fsm) {
    next::sdk::logger::debug(__FILE__, "[PlayerStateMachine] : entry state READY using [{0}]", e.name);
    fsm.current_state_ = ESTATES::E_READY;
    fsm.SendMetaEvent(next::control::events::PlayerState::ON_READY);
    fsm.EnableStatePublish();
  }
  template <class Event, class FSM>
  void on_exit(Event const e, FSM &fsm) {
    next::sdk::logger::info(__FILE__, "[PlayerStateMachine] : exit state READY using [{0}]", e.name);
    fsm.DisableStatePublish();
  }
};
#endif

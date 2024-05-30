/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: Next-Player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     state_skip_to.hpp
 * @brief    SkipTo State implementation for the Player State machine
 *
 **/
#ifndef STATE_SKIP_TO_H
#define STATE_SKIP_TO_H

#include <fstream>
#include <iostream>

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

#include "../../publisher/publisher_manager.hpp"
#include "../../reader/file_reader.hpp"
#include "state_machine_events.hpp"

class StateSkipTo : public boost::msm::front::state<> {
public:
  StateSkipTo() : status_message_(new next::control::events::PlayerMetaInformationMessage()) {}
  void Initialize(std::shared_ptr<FileReader> fileReader) { file_reader_ptr_ = fileReader; }
  template <class Event, class FSM>
  void on_entry(Event const &e, FSM &fsm) {
    next::sdk::logger::debug(__FILE__, "[PlayerStateMachine] : entry state SKIP_TO using [{0}]", e.name);
    fsm.current_state_ = ESTATES::E_SKIP_TO;
    command_JUMP command = (command_JUMP)e;
    status_message_ = fsm.send_state_message_;
    file_reader_ptr_->JumpToTimestamp(e.skipTimestamp, e.timestampType);

    file_reader_ptr_->GetFileStats(status_message_->min_timestamp, status_message_->max_timestamp,
                                   status_message_->timestamp);
    fsm.process_event(event_jump_complete());
  }
  template <class Event, class FSM>
  void on_exit(Event const e, FSM fsm) {
    next::sdk::logger::debug(__FILE__, "[PlayerStateMachine] : exit state SKIP_TO using [{0}]", e.name);
    fsm.SendMetaEvent(next::control::events::PlayerState::EXIT_SKIP_TO, *status_message_.get());
  }

private:
  std::shared_ptr<FileReader> file_reader_ptr_;
  std::shared_ptr<next::control::events::PlayerMetaInformationMessage> status_message_;
};
#endif

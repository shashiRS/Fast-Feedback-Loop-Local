/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     state_unload_recording.hpp
 * @brief    Unload_recording State implementation for the Player State machine
 *
 **/

#ifndef STATE_UNLOAD_RECORDING_H_
#define STATE_UNLOAD_RECORDING_H_

#include <memory>

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

#include "../../publisher/publisher_manager.hpp"
#include "../../reader/file_reader.hpp"
#include "state_machine_events.hpp"

class StateUnloadRecording : public boost::msm::front::state<> {
public:
  void Initialize(std::shared_ptr<IResourceManager> resourceManager, std::shared_ptr<FileReader> fileReader) {
    resource_manager_ptr_ = resourceManager;
    file_reader_ptr_ = fileReader;
  }
  template <class Event, class FSM>
  void on_entry(Event const &e, FSM &fsm) {
    next::sdk::logger::debug(__FILE__, "[PlayerStateMachine] : entry state UNLOAD_RECORDING using [{0}]", e.name);
    fsm.current_state_ = ESTATES::E_UNLOAD_RECORDING;

    // close file
    resource_manager_ptr_->GetDataBuffer()->stopPush();
    resource_manager_ptr_->GetDataBuffer()->clearBuffer();
    file_reader_ptr_->ResetRecording();
    resource_manager_ptr_->GetUdexService()->Terminate();
    next::sdk::logger::info(__FILE__, "[PlayerStateMachine] : Recording closed ");
    fsm.SendMetaEvent(next::control::events::PlayerState::ON_NEW);

    // UNLOAD RECORDING
    fsm.process_event(event_recording_unloaded_OK());
  }
  template <class Event, class FSM>
  void on_exit(Event const e, FSM) {
    next::sdk::logger::debug(__FILE__, "[PlayerStateMachine] : exit state UNLOAD_RECORDING using  [{0}]", e.name);
  }

private:
  std::shared_ptr<FileReader> file_reader_ptr_;
  std::shared_ptr<IResourceManager> resource_manager_ptr_;
};

#endif // STATE_UNLOAD_RECORDING_H_

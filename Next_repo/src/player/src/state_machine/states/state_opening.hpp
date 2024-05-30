/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     state_opening.hpp
 * @brief    Opening State implementation for the Player State machine
 *
 **/

#ifndef STATE_OPENING_H_
#define STATE_OPENING_H_

#include <future>
#include <memory>

#include <next/sdk/sdk_macros.h>

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

NEXT_DISABLE_DEPRECATED_WARNING
#include <mts/runtime/offline/replay_proxy.hpp>
NEXT_RESTORE_WARNINGS

#include <next/control/event_types/player_command.h>
#include <next/control/event_types/player_state_event.h>
#include <next/appsupport/next_status_code/next_status_code.hpp>

#include "../../publisher/publisher_manager.hpp"
#include "../../reader/file_reader.hpp"
#include "../error_codes.hpp"
#include "state_machine_events.hpp"

class StateOpening : public boost::msm::front::state<> {
public:
  StateOpening();
  void Initialize(std::shared_ptr<IResourceManager> resourceManager, std::shared_ptr<PublisherManager> publisherManager,
                  std::shared_ptr<FileReader> fileReader);

  template <class Event, class FSM>
  void on_entry(Event const &e, FSM &fsm) {
    fsm.SendMetaEvent(next::control::events::PlayerState::ON_OPENING);
    next::sdk::logger::debug(__FILE__, "[PlayerStateMachine] : entry state OPENING using [{0}]", e.name);
    next::appsupport::NextStatusCode::PublishStatusCode(next::appsupport::StatusCode::INFO_START_LOAD_RECORDING,
                                                        "[Player] Start to load the recording.");
    fsm.current_state_ = ESTATES::E_OPENING;
    command_OPEN command = (command_OPEN)e;
    next::sdk::logger::info(__FILE__, "Opening file [{0}]", command.recName.front()); // vector
    status_message_ = fsm.send_state_message_;
    resource_manager_ptr_->Initialize();
    // publisher_manager_ptr_->Reset();
    if (OpenFile(command.recName)) { // vector
      // open success
      fsm.process_event(event_recording_loaded_OK());
      next::appsupport::NextStatusCode::PublishStatusCode(next::appsupport::StatusCode::SUCC_RECORDING_LOADED,
                                                          "[Player] Loading Recording successful.");
    } else {
      // open failed
      fsm.process_event(event_ERROR(PlayerError::E_OpenFile));
      next::appsupport::NextStatusCode::PublishStatusCode(next::appsupport::StatusCode::ERR_RECORDING_LOADED,
                                                          "[Player] Loading Recording failed.");
    }
  }
  template <class Event, class FSM>
  void on_exit(Event const &e, FSM &fsm) {
    next::sdk::logger::debug(__FILE__, "[PlayerStateMachine] : exit state  OPENING using event [{0}]", e.name);
    fsm.SendMetaEvent(next::control::events::PlayerState::EXIT_OPENING, *status_message_.get());
  }

private:
  std::shared_ptr<next::control::events::PlayerMetaInformationMessage> status_message_;

  std::shared_ptr<PublisherManager> publisher_manager_ptr_;
  std::shared_ptr<FileReader> file_reader_ptr_;
  std::shared_ptr<IResourceManager> resource_manager_ptr_;

  bool OpenFile(const std::vector<std::string> &file_path);
  void SafeErrorToStatusEvent(const std::vector<std::string> &file_path);
};

#endif // STATE_OPENING_H_

/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     player_state_machine.hpp
 * @brief    Player State machine Implementation
 *
 **/

#ifndef PLAYER_STATE_MACHINE_H
#define PLAYER_STATE_MACHINE_H

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_VECTOR_SIZE 50
#define BOOST_MPL_LIMIT_MAP_SIZE 50

#include <chrono>
#include <condition_variable>
#include <exception>
#include <memory>
#include <mutex>
#include <thread>

#ifdef __linux__
#pragma GCC system_header // disable all warnings from boost lib for gcc++
#endif
#ifdef _WIN32
#pragma warning(push, 0) // disable all warnings from boost lib for vc++
#endif
#include <boost/msm/back/tools.hpp>
#ifdef _WIN32
#pragma warning(pop)
#endif

#include <next/control/event_types/open_recording_status_event.h>
#include <next/control/event_types/player_command.h>
#include <next/control/event_types/player_state_event.h>
#include <next/appsupport/next_status_code/next_status_code.hpp>
#include <next/sdk/sdk.hpp>

#include "states/state_error.hpp"
#include "states/state_machine_events.hpp"
#include "states/state_opening.hpp"
#include "states/state_play.hpp"
#include "states/state_ready.hpp"
#include "states/state_shutdown.hpp"
#include "states/state_skip_to.hpp"
#include "states/state_unload_recording.hpp"

constexpr int kUpdateStateWaitTimeSec = 3;

// front-end: define the FSM structure
class PlayerStateMachine : public boost::msm::front::state_machine_def<PlayerStateMachine> {

public:
  PlayerStateMachine();
  ESTATES current_state_;
  std::shared_ptr<std::mutex> process_event_mutex_;
  void Shutdown();

  struct StateNew : public boost::msm::front::state<> {
    template <class Event, class FSM>
    void on_entry(Event const &, FSM &fsm) {
      fsm.SendMetaEvent(next::control::events::PlayerState::ENTER_NEW);
      fsm.current_state_ = ESTATES::E_NEW;
      fsm.send_state_message_->min_timestamp = 0;
      fsm.send_state_message_->max_timestamp = 0;
      fsm.send_state_message_->timestamp = 0;
      fsm.send_state_message_->speed_factor = 0;
      fsm.send_state_message_->status = next::control::events::PlayerState::UNSPECIFIED;
      fsm.send_state_message_->recording_name = "";
      fsm.send_state_message_->stepping_type = "";
      fsm.send_state_message_->stepping_value = 0;
      fsm.EnableStatePublish();

      next::appsupport::NextStatusCode::PublishStatusCode(next::appsupport::StatusCode::SUCC_COMP_INIT_FINISHED,
                                                          "[Player] Entry state new finished");
    }
    template <class Event, class FSM>
    void on_exit(Event const, FSM &fsm) {
      next::sdk::logger::debug(__FILE__, "[PlayerStateMachine] : exit state NEW");
      fsm.DisableStatePublish();
    }
  };

  // Actions
  struct do_action {
    template <class FSM, class EVT, class SourceState, class TargetState>
    void operator()(EVT const &, FSM &, SourceState &, TargetState &) {}
  };

  typedef StateNew initial_state;
  // clang-format off
    struct transition_table
        : boost::mpl::vector<
            //                     Source                 Event                        Target                 Action        Guard
            boost::msm::front::Row<StateNew,              command_OPEN,                StateOpening,          do_action,    boost::msm::front::none>,
            boost::msm::front::Row<StateNew,              test_command_READY,          StateReady,            do_action,    boost::msm::front::none>,
            boost::msm::front::Row<StateOpening,          event_recording_loaded_OK,   StateReady,            do_action,    boost::msm::front::none>,
            boost::msm::front::Row<StateOpening,          event_ERROR,                 StateError,            do_action,    boost::msm::front::none>,
            boost::msm::front::Row<StateReady,            command_CLOSE,               StateUnloadRecording,  do_action,    boost::msm::front::none>,
            boost::msm::front::Row<StateUnloadRecording,  event_recording_unloaded_OK, StateNew,              do_action,    boost::msm::front::none>,
            boost::msm::front::Row<StateReady,            command_JUMP,                StateSkipTo,           do_action,    boost::msm::front::none>,
            boost::msm::front::Row<StateSkipTo,           event_jump_complete,         StateReady,            do_action,    boost::msm::front::none>,
            boost::msm::front::Row<StateReady,            command_STEP_FORWARD,        StatePlay,             do_action,    boost::msm::front::none>,
            boost::msm::front::Row<StatePlay,             command_PAUSE,               StateReady,            do_action,    boost::msm::front::none>,
            boost::msm::front::Row<StatePlay,             event_step_complete,         StateReady,            do_action,    boost::msm::front::none>,
            boost::msm::front::Row<StateNew,              event_shutdown,              StateShutdown,         boost::msm::front::none,    boost::msm::front::none>,
            boost::msm::front::Row<StateOpening,          event_shutdown,              StateShutdown,         boost::msm::front::none,    boost::msm::front::none>,
            boost::msm::front::Row<StateReady,            event_shutdown,              StateShutdown,         boost::msm::front::none,    boost::msm::front::none>,
            boost::msm::front::Row<StatePlay,             event_shutdown,              StateShutdown,         boost::msm::front::none,    boost::msm::front::none>,
            boost::msm::front::Row<StateSkipTo,           event_shutdown,              StateShutdown,         boost::msm::front::none,    boost::msm::front::none>,
            boost::msm::front::Row<StateUnloadRecording,  event_shutdown,              StateShutdown,         boost::msm::front::none,    boost::msm::front::none>,
            boost::msm::front::Row<StateError,            event_handled_error,         StateNew,              boost::msm::front::none,    boost::msm::front::none>,
            boost::msm::front::Row<StateError,            event_unhandled_error,       StateShutdown,         boost::msm::front::none,    boost::msm::front::none>,
            boost::msm::front::Row<StateSkipTo,           event_unhandled_error,       StateShutdown,         boost::msm::front::none,    boost::msm::front::none>,
            boost::msm::front::Row<StateOpening,          reset_on_exception,          StateNew,              boost::msm::front::none,    boost::msm::front::none>,
            boost::msm::front::Row<StateSkipTo,           reset_on_exception,          StateNew,              boost::msm::front::none,    boost::msm::front::none>,
            boost::msm::front::Row<StateUnloadRecording,  reset_on_exception,          StateNew,              boost::msm::front::none,    boost::msm::front::none>
            > {};
  // clang-format on

  // Replaces the default no-transition response.
  template <class FSM, class Event>
  void no_transition(Event const &e, FSM &, int state) {
    typedef typename boost::msm::back::recursive_get_transition_table<FSM>::type recursive_stt;
    typedef typename boost::msm::back::generate_state_set<recursive_stt>::type all_states;
    std::string stateName;
    boost::mpl::for_each<all_states, boost::msm::wrap<boost::mpl::placeholders::_1>>(
        boost::msm::back::get_state_name<recursive_stt>(stateName, state));
    next::sdk::logger::warn(__FILE__, "No Transition Possible from state [{0}] using [{1}]",
                            boost::core::demangle(stateName.c_str()), e.name);
  }

  template <class FSM, class Event>
  void exception_caught(Event const &ev, FSM &fsm, std::exception &e) {
    next::sdk::logger::warn(__FILE__, "[PlayerStateMachine] Exception [{0}] happend using event [{1}]", e.what(),
                            ev.name);
    auto state = fsm.current_state()[0];
    typedef typename boost::msm::back::recursive_get_transition_table<FSM>::type recursive_stt;
    typedef typename boost::msm::back::generate_state_set<recursive_stt>::type all_states;
    std::string stateName;
    boost::mpl::for_each<all_states, boost::msm::wrap<boost::mpl::placeholders::_1>>(
        boost::msm::back::get_state_name<recursive_stt>(stateName, state));
    if (ev.name == "event_shutdown") {
      fsm.current_state_ = ESTATES::E_SHUTDOWN;
    }

    // we don't know how much trouble the unhandled exception caused; will go back to new state
    if (stateName == "class StateReady") {
      fsm.process_event(command_CLOSE());
    } else if (stateName == "class StatePlay") {
      fsm.process_event(command_PAUSE());
      fsm.process_event(command_CLOSE());
    } else {
      fsm.process_event(reset_on_exception());
    }
  }
  void EnableStatePublish();
  void DisableStatePublish();
  void SendMetaEvent(next::control::events::PlayerState);
  void SendMetaEvent(next::control::events::PlayerState, next::control::events::PlayerMetaInformationMessage);

  std::shared_ptr<next::control::events::OpenRecordingStatusEvent> open_recording_status_publisher_;

  std::shared_ptr<next::control::events::PlayerStateEvent> player_state_publisher_;
  std::shared_ptr<next::control::events::PlayerMetaInformationMessage> send_state_message_;

  void UpdateOpenRecordingStatus(const std::string &executed_function, const std::string &description,
                                 const bool areIndexesUsed, const size_t currentIndex, const size_t maxIndex);

private:
  void SendState();
  next::control::events::PlayerState ConvertStateToMetaInfoStatus(ESTATES state);
  std::shared_ptr<std::thread> state_thread_;

  std::shared_ptr<std::mutex> send_current_state_mutex_;
  std::shared_ptr<std::condition_variable> send_state_cv_;

  bool state_ready_{false};
  bool shutdown_{false};
  std::chrono::steady_clock::time_point last_event_send_time_{std::chrono::steady_clock::now()};
  const std::chrono::milliseconds kMetaInformationWait{500};
  std::chrono::steady_clock::time_point last_open_recording_status_send_time_{std::chrono::steady_clock::now()};
  const std::chrono::milliseconds kOpenRecordingStatusSendInterval{100};
};

#endif

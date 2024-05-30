/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     state_play.hpp
 * @brief    Play State implementation for the Player State machine
 *
 **/

#include <future>
#include <memory>

#ifndef STATE_PLAY_H
#define STATE_PLAY_H

#ifdef __linux__
#pragma GCC system_header // disable all warnings from boost lib for gcc++
#endif
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/state_machine_def.hpp>

#include <next/control/event_types/player_command.h>
#include <next/control/event_types/player_state_event.h>
#include <next/sdk/logger/logger.hpp>

#include "../../publisher/publisher_manager.hpp"
#include "state_machine_events.hpp"

class StatePlay : public boost::msm::front::state<> {
public:
  StatePlay();
  void Initialize(std::shared_ptr<PublisherManager> publisherManager,
                  std::shared_ptr<IResourceManager> resourceManager);
  template <class Event, class FSM>
  void on_entry(Event const &e, FSM &fsm) {
    player_state_publisher_ = fsm.player_state_publisher_;
    statusMessage_ = fsm.send_state_message_;
    next::sdk::logger::debug(__FILE__, "[PlayerStateMachine] : entry state PLAY using [{0}]", e.name);
    publisher_manager_ptr_->ResetSpeedFactor();
    current_timestamp_ = 0; // reset timestamp used in step calculation
    fsm.current_state_ = ESTATES::E_PLAY;
    command_STEP_FORWARD command = (command_STEP_FORWARD)e;
    number_of_steps_to_run_ = command.steps;
    if (command.until_play) {
      play_until_ = true;
      timestamp_play_until_ = command.timestamp_until;
    }

    if (number_of_steps_to_run_ != INT32_MAX)
      is_steping_ = true;
    resource_manager_ptr_->GetDataBuffer()->SignalPlay();
    read_thread_active_flag_ = true;
    read_future_ = std::make_shared<std::future<void>>(std::async(std::launch::async, [this, &fsm]() {
      StartReadingData();
      if (number_of_steps_to_run_ == 0) {
        player_state_publisher_->updateStatus(next::control::events::PlayerState::ON_READY);
        player_state_publisher_->send();
        std::lock_guard<std::mutex> guard(*fsm.process_event_mutex_);
        fsm.process_event(event_step_complete());
      }
    }));
  }

  template <class Event, class FSM>
  void on_exit(Event const &e, FSM &) {
    read_thread_active_flag_ = false;
    resource_manager_ptr_->GetDataBuffer()->SignalPause();
    if (number_of_steps_to_run_ != 0) {
      // wait for run thread to finish only if steps remain
      if (read_future_->wait_for(std::chrono::seconds(0)) != std::future_status::ready)
        read_future_->get();
    }
    next::sdk::logger::debug(__FILE__, "[PlayerStateMachine] : exit state PLAY using [{0}]", e.name);
  }

private:
  bool is_steping_{false};
  uint64_t timestamp_play_until_ = 0;
  bool play_until_ = false;
  bool read_thread_active_flag_{true}; // maybe use something more nice here
  unsigned long number_of_steps_to_run_{0};
  uint64_t current_timestamp_{0};
  std::shared_ptr<next::control::events::PlayerMetaInformationMessage> statusMessage_;
  std::shared_ptr<std::future<void>> read_future_;
  std::shared_ptr<next::control::events::PlayerStateEvent> player_state_publisher_;
  std::shared_ptr<PublisherManager> publisher_manager_ptr_;
  std::shared_ptr<IResourceManager> resource_manager_ptr_;

  void StartReadingData();
  bool StepByCycle(uint32_t cycleId, uint8_t cycleState);
  bool StepByTimestamp(uint64_t pkgTimestamp);
  void UpdateNumberOfSteps(uint32_t cycleId, uint8_t cycleState, uint64_t currentTimestamp);
  void ResetAtEoF();
};
#endif

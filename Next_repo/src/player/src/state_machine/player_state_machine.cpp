/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     player_state_machine.cpp
 * @brief    Player State machine Implementation
 *
 **/

#include "player_state_machine.hpp"

PlayerStateMachine::PlayerStateMachine()
    : process_event_mutex_(new std::mutex()),
      open_recording_status_publisher_(new next::control::events::OpenRecordingStatusEvent("")),
      player_state_publisher_(new next::control::events::PlayerStateEvent("")),
      send_state_message_(new next::control::events::PlayerMetaInformationMessage()),
      state_thread_(new std::thread(([&]() { SendState(); }))), send_current_state_mutex_(new std::mutex()),
      send_state_cv_(new std::condition_variable()) {}

next::control::events::PlayerState PlayerStateMachine::ConvertStateToMetaInfoStatus(ESTATES state) {
  next::control::events::PlayerState retVal = next::control::events::PlayerState::UNSPECIFIED;

  switch (state) {
  case ESTATES::E_NEW: {
    retVal = next::control::events::PlayerState::ON_NEW;
    break;
  }
  case ESTATES::E_READY: {
    retVal = next::control::events::PlayerState::ON_READY;
    break;
  }
  case ESTATES::E_PLAY: {
    retVal = next::control::events::PlayerState::ON_PLAY;
    break;
  }
  default:
    break;
  };
  return retVal;
}

void PlayerStateMachine::Shutdown() {
  shutdown_ = true;
  send_state_cv_->notify_all();
  if (state_thread_->joinable()) {
    state_thread_->join();
  }
}

void PlayerStateMachine::SendState() {
  shutdown_ = false;
  while (!shutdown_) {
    std::this_thread::sleep_for(std::chrono::seconds(kUpdateStateWaitTimeSec));
    std::unique_lock<std::mutex> lock(*send_current_state_mutex_);
    send_state_cv_->wait(lock, [&]() {
      bool returnVal = false;
      if (state_ready_)
        returnVal = true;
      if (shutdown_)
        returnVal = true;
      return returnVal;
    });
    if (!shutdown_) {
      auto status = ConvertStateToMetaInfoStatus(current_state_);
      SendMetaEvent(status, *send_state_message_.get());
    }
  }
}

void PlayerStateMachine::EnableStatePublish() {
  std::lock_guard<std::mutex> lck(*send_current_state_mutex_);
  state_ready_ = true;
  send_state_cv_->notify_one();
}

void PlayerStateMachine::DisableStatePublish() {
  std::lock_guard<std::mutex> lck(*send_current_state_mutex_);
  state_ready_ = false;
  send_state_cv_->notify_one();
}

void PlayerStateMachine::SendMetaEvent(next::control::events::PlayerState state) {
  auto now = std::chrono::steady_clock::now();
  auto elapsed_playspeed_check = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_event_send_time_);
  if (elapsed_playspeed_check < kMetaInformationWait) {
    std::this_thread::sleep_for(std::chrono::milliseconds(kMetaInformationWait - elapsed_playspeed_check));
  }
  player_state_publisher_->updateStatus(state);
  player_state_publisher_->send();
  last_event_send_time_ = std::chrono::steady_clock::now();
}

void PlayerStateMachine::SendMetaEvent(next::control::events::PlayerState state,
                                       next::control::events::PlayerMetaInformationMessage message) {
  auto now = std::chrono::steady_clock::now();
  auto elapsed_playspeed_check = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_event_send_time_);
  if (elapsed_playspeed_check < kMetaInformationWait) {
    std::this_thread::sleep_for(std::chrono::milliseconds(kMetaInformationWait - elapsed_playspeed_check));
  }
  player_state_publisher_->_message = message;
  player_state_publisher_->updateStatus(state);

  player_state_publisher_->send();
  last_event_send_time_ = std::chrono::steady_clock::now();
}
void PlayerStateMachine::UpdateOpenRecordingStatus(const std::string &executed_function, const std::string &description,
                                                   const bool areIndexesUsed, const size_t currentIndex,
                                                   const size_t maxIndex) {
  auto now = std::chrono::steady_clock::now();
  auto elapsed_open_recording_status_send_time =
      std::chrono::duration_cast<std::chrono::milliseconds>(now - last_open_recording_status_send_time_);
  // we should only use the time interval if we have an index representation and it's not the last index
  if (areIndexesUsed && currentIndex != maxIndex) {
    if (elapsed_open_recording_status_send_time < kOpenRecordingStatusSendInterval) {
      return;
    }
  }
  open_recording_status_publisher_->message_.executed_function = executed_function;
  open_recording_status_publisher_->message_.description = description;
  open_recording_status_publisher_->message_.areIndexesUsed = areIndexesUsed;
  open_recording_status_publisher_->message_.currentIndex = currentIndex;
  open_recording_status_publisher_->message_.maxIndex = maxIndex;
  open_recording_status_publisher_->send();
  last_open_recording_status_send_time_ = std::chrono::steady_clock::now();
  if (areIndexesUsed) {
    debug(__FILE__, "[{0}] : {1} : index {2} of {3} ", executed_function, description, currentIndex, maxIndex);
  } else {
    debug(__FILE__, "[{0}] : {1}", executed_function, description);
  }
}

#include "state_play.hpp"

StatePlay::StatePlay() : statusMessage_(new next::control::events::PlayerMetaInformationMessage()) {}

void StatePlay::Initialize(std::shared_ptr<PublisherManager> publisherManager,
                           std::shared_ptr<IResourceManager> resourceManager) {
  publisher_manager_ptr_ = publisherManager;
  resource_manager_ptr_ = resourceManager;
}

void StatePlay::StartReadingData() {
  uint32_t cycleId = 0;
  uint8_t cycleState = 0;
  statusMessage_->speed_factor = 0;
  statusMessage_->status = next::control::events::PlayerState::ON_PLAY;
  player_state_publisher_->_message = *statusMessage_.get();
  player_state_publisher_->send();
  resource_manager_ptr_->GetReplayStatistics()->replay_start_time = std::chrono::steady_clock::now();
  bool eof = false;
  while (read_thread_active_flag_ && number_of_steps_to_run_) {
    if (publisher_manager_ptr_->PublishDataPackage(statusMessage_->timestamp, cycleId, cycleState, eof)) {

      // extra feature for handling use case when to play recording until certain timeframe
      if (play_until_ && statusMessage_->timestamp > timestamp_play_until_) {
        eof = true;
        break;
      }

      float speed_factor = publisher_manager_ptr_->GetCurrentSpeedFactor();
      if (speed_factor != -1) {
        statusMessage_->speed_factor = speed_factor;
        statusMessage_->status = next::control::events::PlayerState::ON_PLAY;
        player_state_publisher_->_message = *statusMessage_.get();
        player_state_publisher_->send();
      }
    } else {
      if (eof) {
        ResetAtEoF();
      }
    }
    // stepping
    if (is_steping_ && !eof) {
      UpdateNumberOfSteps(cycleId, cycleState, statusMessage_->timestamp);
    }
  }

  if (eof) {
    ResetAtEoF();
  }
}

void StatePlay::ResetAtEoF() {
  read_thread_active_flag_ = false;
  number_of_steps_to_run_ = 0;
  auto now = std::chrono::steady_clock::now();
  auto elapsed =
      std::chrono::round<std::chrono::seconds>(now - resource_manager_ptr_->GetReplayStatistics()->replay_start_time)
          .count();
  debug(__FILE__, "Replay paused/stopped");
  debug(__FILE__, "Replay time: {0} s", elapsed);
  debug(__FILE__, "Raw packages = {}", resource_manager_ptr_->GetReplayStatistics()->package_count);
  debug(__FILE__, "Published packages = {}", resource_manager_ptr_->GetReplayStatistics()->published_package_count);
  debug(__FILE__, "Published data = {:.2f} MB",
        (float)resource_manager_ptr_->GetReplayStatistics()->published_bytes / (1 << 20));
  player_state_publisher_->_message = *statusMessage_.get();
  player_state_publisher_->updateStatus(next::control::events::PlayerState::ON_READY);
  player_state_publisher_->send();
}

bool StatePlay::StepByCycle(uint32_t cycleId, uint8_t cycleState) {
  if (cycleId == publisher_manager_ptr_->GetCycleIdFromConfigFile()) {
    if (cycleState == mts_package_cycle_end) {
      return true;
    }
  }
  return false;
}

bool StatePlay::StepByTimestamp(uint64_t pkgTimestamp) {
  if (current_timestamp_ == 0) {
    current_timestamp_ = pkgTimestamp;
  }
  if ((pkgTimestamp - current_timestamp_) > publisher_manager_ptr_->GetTimeStampFromConfigFile()) {
    current_timestamp_ = pkgTimestamp;
    return true;
  }
  return false;
}

void StatePlay::UpdateNumberOfSteps(uint32_t cycleId, uint8_t cycleState, uint64_t currentTimestamp) {
  switch (publisher_manager_ptr_->GetSteppingMode()) {
  case STEPPING_BY_CYCLE_ID:
    if (StepByCycle(cycleId, cycleState) == true)
      number_of_steps_to_run_--;
    break;
  case STEPPING_BY_TIMESTAMP:
    if (StepByTimestamp(currentTimestamp) == true)
      number_of_steps_to_run_--;
    break;
  default:
    // package based
    number_of_steps_to_run_--;
  }
}

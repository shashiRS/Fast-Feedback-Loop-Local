/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     speed_factor.cpp
 * @brief    calculates speed factor and updates simulation speed
 *
 **/

#include "speed_factor.hpp"

#include <cmath>

SpeedFactor::SpeedFactor() {}
void SpeedFactor::SetSpeedFactor(float value) {
  std::lock_guard<std::mutex> guard(update_speed_factor_mutex_);
  speed_factor_ = value;
}

void SpeedFactor::Reset() {
  std::lock_guard<std::mutex> guard(update_speed_factor_mutex_);
  last_received_timestamp_ = 0;
  speed_factor_reset_ = true;
}

float SpeedFactor::UpdateSimulationSpeed(uint64_t currentTimestamp) {
  std::lock_guard<std::mutex> guard(update_speed_factor_mutex_);
  float retVal = -1;
  auto now = std::chrono::steady_clock::now();
  auto elapsed_playspeed_check =
      std::chrono::duration_cast<std::chrono::milliseconds>(now - last_playspeed_check_time_);
  if (elapsed_playspeed_check > playspeed_check_period_) {

    // we check the playspeed and adapt it if needed and update the last time for check and last received ts
    adaptPlaySpeed(last_playspeed_check_time_, currentTimestamp, last_received_timestamp_);
    last_received_timestamp_ = currentTimestamp;
    last_playspeed_check_time_ = std::chrono::steady_clock::now();

    retVal = UpdateSpeedFactorForGui(now, currentTimestamp);

    if (speed_factor_reset_) {
      ResetSteppedFactor(currentTimestamp);
    }
  }

  return retVal;
}

void SpeedFactor::adaptPlaySpeed(std::chrono::steady_clock::time_point &last_check_time, uint64_t current_ts,
                                 uint64_t last_ts) {
  float speed_factor = getSpeedFactor(last_check_time, current_ts, last_ts);
  const float sim_time_diff = float(current_ts - last_ts);
  const float real_time_diff =
      float(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - last_check_time)
                .count());

  if (speed_factor_ > 0.f && last_ts < current_ts && !speed_factor_reset_) {
    if (speed_factor > speed_factor_) {
      speed_factor = speed_factor_;
      float difference = std::roundf((sim_time_diff - (speed_factor * real_time_diff)) / speed_factor);
      std::unique_lock<std::mutex> lk(adapt_speed_mutex_);
      adapt_speed_cond_variable_.wait_for(lk, std::chrono::microseconds(uint64_t(difference)));
    }
  }
}

float SpeedFactor::getSpeedFactor(std::chrono::steady_clock::time_point last_time_point, uint64_t current_ts,
                                  uint64_t last_ts) {
  // Real Time Diff
  auto real_time_diff =
      std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - last_time_point).count();
  last_time_point = std::chrono::steady_clock::now();

  // Timestamp Diff
  auto sim_time_diff = current_ts - last_ts;
  // qoutient
  float speed_factor = 0.f;
  if (real_time_diff != 0) {
    speed_factor = float(sim_time_diff) / float(real_time_diff);
  } else {
    speed_factor = speed_factor_;
  }
  return speed_factor;
}

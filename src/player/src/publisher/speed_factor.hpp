/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: Next-Player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     speed_factor.hpp
 * @brief    calculates speed factor and updates simulation speed
 *
 **/

#ifndef SPEED_FACTOR_
#define SPEED_FACTOR_

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <future>
#include <mutex>

class SpeedFactor {
public:
  SpeedFactor();
  void SetSpeedFactor(float value);
  // !Updates the simultaion play speed and returns the current speed factor
  float UpdateSimulationSpeed(uint64_t currentTimestamp);
  void Reset();

private:
  void adaptPlaySpeed(std::chrono::steady_clock::time_point &last_check_time, uint64_t current_ts, uint64_t last_ts);
  float getSpeedFactor(std::chrono::steady_clock::time_point last_time_point, uint64_t current_ts, uint64_t last_ts);
  inline void ResetSteppedFactor(uint64_t currentTimestamp) {
    speed_factor_reset_ = false;
    // we also reset the meta information time
    last_meta_information_time_ = std::chrono::steady_clock::now();
    last_received_timestamp_meta_info_ = currentTimestamp;
  };

  inline float UpdateSpeedFactorForGui(std::chrono::steady_clock::time_point now, uint64_t currentTimestamp) {
    float retVal = -1;
    auto elapsed_meta_info = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_meta_information_time_);
    if (elapsed_meta_info >= meta_information_wait_ && !speed_factor_reset_) {
      // prepare status val for metadata metadata
      float speed_factor =
          getSpeedFactor(last_meta_information_time_, currentTimestamp, last_received_timestamp_meta_info_);
      last_meta_information_time_ = std::chrono::steady_clock::now();
      last_received_timestamp_meta_info_ = currentTimestamp;
      retVal = speed_factor;
    }
    return retVal;
  }
  // variables used by the play speed adaptation
  std::chrono::steady_clock::time_point last_playspeed_check_time_{std::chrono::steady_clock::now()};
  std::chrono::steady_clock::time_point last_meta_information_time_{std::chrono::steady_clock::now()};
  uint64_t last_received_timestamp_{0};
  uint64_t last_received_timestamp_meta_info_{0};
  std::chrono::milliseconds meta_information_wait_{500};
  std::chrono::milliseconds playspeed_check_period_{25};
  std::condition_variable adapt_speed_cond_variable_;
  std::mutex adapt_speed_mutex_;
  std::mutex update_speed_factor_mutex_;
  std::atomic_bool speed_factor_reset_{true}; // flag used by the adaptPlaySpeed function to do/not do speed correction
  std::atomic<float> speed_factor_;
};
#endif

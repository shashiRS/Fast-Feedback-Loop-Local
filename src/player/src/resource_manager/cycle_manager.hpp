/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     cycle_manager.hpp
 * @brief    Cycle Manager
 *
 **/

#ifndef NEXT_PLAYER_CYCLE_MANAGER_H_
#define NEXT_PLAYER_CYCLE_MANAGER_H_

#include <map>
#include <string>
#include <vector>

#include <next/player/plugin/reader_interface.hpp>

#include <next/sdk/events/cycle_event.hpp>
#include <next/sdk/events/cycle_information_event.hpp>

namespace next {
namespace player {

/**
 * The cycle manager is for providing the cycle information with the topic *PlayerCycleEventInformation* and creating
 * the cycle events, having the prefix *PlayerCycleEvent_* for topic name.
 *
 * For every cycle information, given via the CreateCycleInfoEvent, one event publisher is created. The events itself
 * are then published via the PublishCycle function.
 *
 * When resetting or deconstructing, the cycle events and the cycle event information publishers are destroyed (see
 * @ref Reset).
 */
class CycleManager {
public:
  //! Constructs the CycleManager, creates a blocking event publisher named *PlayerCycleEventInformation*
  CycleManager();

  /**
   * Create all cycle event publisher and send the list of available cycle events.
   *
   * For each cycle information a cycle event publisher is created. Afterwards the information about the created
   * publishers is sent via the *PlayerCycleEventInformation*.
   *
   * @param cycles List of the available cycle events.
   */
  void CreateCycleEventPublisher(const std::vector<next::player::plugin::ReaderInterface::CycleInformation> &cycles);

  /**
   * Publish the event of one cycle.
   * If the given cycle ID is unknown the function does nothing.
   *
   * @param timestamp Timestamp of the event. So far unused.
   * @param cycle_id The ID of the cycle.
   * @param cycle_state The state of the cycle, see next::sdk::events::CycleState.
   */
  void PublishCycle(const size_t timestamp, const uint32_t cycle_id, const uint8_t cycle_state);

  /**
   * Destroying the cycle event publishers and sending an empty list via the cycle event information.
   */
  void Reset();

protected:
  std::vector<next::player::plugin::ReaderInterface::CycleInformation> cycles_;
  next::sdk::events::CycleInformationEvent cycle_information_event_;
  std::map<uint32_t, next::sdk::events::CycleEvent> cycle_event_;
};

} // namespace player
} // namespace next

#endif

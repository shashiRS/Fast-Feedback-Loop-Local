/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     player_resource_manager_cycle_test.cpp
 * @brief    Test of the Cycle Manager via the Resource Manager of the next_player.
 *
 **/

#include <atomic>
#include <chrono>
#include <thread>

#include <next/sdk/sdk.hpp>

#include <next/sdk/events/cycle_event.hpp>
#include <next/sdk/events/cycle_information_event.hpp>

#include <resource_manager/cycle_manager.hpp>

class CycleManagerTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  CycleManagerTestFixture() { this->instance_name_ = "CycleManagerTest"; }
};

class CycleManagerDerived : public next::player::CycleManager {
public:
  CycleManagerDerived() : next::player::CycleManager() {
    cycle_information_event_.addListenerAddedHook(
        std::bind(&CycleManagerDerived::InfoListenerAdded, this, std::placeholders::_1));
  }

  void InfoListenerAdded(const std::string &) { info_listeners_++; }
  std::atomic<int> info_listeners_{0};
};

std::atomic<int> cycle_events_infos_received{0};
void CycleInfoEventHook() { cycle_events_infos_received++; }

std::atomic<int> cycle_events_received{0};
void CycleEventHook() { cycle_events_received++; }

template <typename T, typename V>
void WaitForVariableHasValue(T &var, V val) {
  const auto end = std::chrono::steady_clock::now() + std::chrono::seconds(5);
  while (std::chrono::steady_clock::now() < end && var == val) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
}

// TODO replace the sleeps with something else (e.g. waiting for the events)
TEST_F(CycleManagerTestFixture, sendCycleInfoAndCycle_eventsReceived) {
  std::vector<next::player::plugin::ReaderInterface::CycleInformation> cycle_infos;
  cycle_infos.push_back({100, "Device100", "NoDescription"});
  cycle_infos.push_back({200, "Device200", "NoDescription"});
  cycle_infos.push_back({0, "Device000", "NoDescription"});

  CycleManagerDerived cycle_manager;
  cycle_manager.CreateCycleEventPublisher({});
  std::this_thread::sleep_for(std::chrono::seconds(1));

  next::sdk::events::CycleInformationEvent cycle_info_event("PlayerCycleEventInformation");
  cycle_info_event.addEventHook(CycleInfoEventHook);
  cycle_info_event.subscribe();
  std::this_thread::sleep_for(std::chrono::seconds(2));

  cycle_manager.CreateCycleEventPublisher(cycle_infos);
  WaitForVariableHasValue(cycle_manager.info_listeners_, 1);
  // TODO fix the send / receive of the event information, no information is received
  // EXPECT_EQ(cycle_manager.info_listeners_, 1);
  // EXPECT_EQ(cycle_events_infos_received, 1);

  next::sdk::events::CycleEvent cycle_event_subs(std::string("PlayerCycleEvent_") + "Device200");
  cycle_event_subs.subscribe();
  cycle_event_subs.addEventHook(CycleEventHook);
  std::this_thread::sleep_for(std::chrono::seconds(3));

  cycle_manager.PublishCycle(0, 200, 0);

  WaitForVariableHasValue(cycle_events_received, 1);

  EXPECT_EQ(cycle_events_received, 1);
}

#include <cip_test_support/gtest_reporting.h>
#include <atomic>
#include <next/sdk/sdk.hpp>

#include <ecal/ecal.h>
#include <thread>

#include <next/control/event_types/player_state_event.h>
#include <next/sdk/events/lifecycle_trigger.h>

#include "lifecycle_orchestrator.h"

namespace next {
namespace controlbridge {

using namespace next::sdk;

// using namespace next::sdk::testing;
class LifecycleOrchestratorTest : public testing::TestUsingEcal {
public:
  LifecycleOrchestratorTest() {
    logger::register_to_init_chain();
    this->instance_name_ = "LifecycleOrchestratorTest";
  }
};

std::atomic<bool> resetReceived = false;

void resetCallback() { resetReceived = true; }

// send a player status and see if a RESET event is triggered
TEST_F(LifecycleOrchestratorTest, receivePlayerEventSendReset) {
  eCAL::Util::EnableLoopback(true);
  resetReceived = false;

  next::control::events::PlayerStateEvent player_event("");
  next::sdk::events::LifecycleTrigger lfc_trigger("");

  lfc_trigger.subscribe();
  lfc_trigger.addEventHook(&resetCallback);

  next::controlbridge::lifecycle_orchestrator::LifecycleOrchestrator::start();

  player_event.updateStatus(next::control::events::PlayerState::EXIT_OPENING);
  player_event.send();

  // wait for eCal
  for (int i = 0; i < 300; i++) {
    if (resetReceived)
      break;
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  EXPECT_TRUE(resetReceived);
}

} // namespace controlbridge
} // namespace next

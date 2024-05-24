/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     lifecycle_orchestrator.h
 * @brief    reacts on next-p[layer events and sends out lifecycle trigger
 *
 **/

#ifndef CONTROLBRIDGE_LIFECYCLE_ORCHESTRATOR_
#define CONTROLBRIDGE_LIFECYCLE_ORCHESTRATOR_

#include <memory>

#include <next/sdk/event_base/event_types.h>

// forward declarations
namespace next {
namespace control {
namespace events {
class PlayerStateEvent;
}
} // namespace control
} // namespace next

namespace next {
namespace sdk {
namespace events {
class LifecycleTrigger;
}
} // namespace sdk
} // namespace next

namespace next {
namespace sdk {
namespace events {
class LifecycleState;
}
} // namespace sdk
} // namespace next

namespace next {
namespace controlbridge {
namespace lifecycle_orchestrator {

//! This class is a Singleton, it waits for external triggers e.g. new Recording loaded from Player or RESET trigger
//! from GUI and sends
class LifecycleOrchestrator {
public:
  static constexpr unsigned int kNumberOfListenersSelf = 1;

  //! starts the LifecycleOrchestrator and subscribes to eventsystem
  static bool start();

  //! create a singleton instance of LifecycleOrchestrator. this is threadsafe.
  static std::unique_ptr<LifecycleOrchestrator> &GetInstance();

  //! this hook will be executed before a RESET lifecycle trigger is send
  void addOnResetHook(const next::sdk::events::notify_hook &hook);

  //! this hook will be executed before a SHUTDOWN lifecycle trigger is send
  void addOnShutdownHook(const next::sdk::events::notify_hook &hook);

  //! public destructor needed by std::unique_ptr
  virtual ~LifecycleOrchestrator();

  //! publish reset trigger for lifecycle eventsystem, executes all reset_hooks
  void publishReset();

private:
  //! constructor, may init eventsystem
  LifecycleOrchestrator();

  //! since we are a singleton don't allow copy and assignment
  LifecycleOrchestrator(const LifecycleOrchestrator &) = delete;
  LifecycleOrchestrator &operator=(const LifecycleOrchestrator &) = delete;

  //! startup and register everything needed
  bool setup();

  std::unique_ptr<next::sdk::events::LifecycleTrigger> lifecycle_trigger_;
  std::unique_ptr<next::control::events::PlayerStateEvent> player_event_;
  std::unique_ptr<next::sdk::events::LifecycleState> lifecycle_event_;

  //! is called by player eventsystem
  void OnPlayerEvent();

  //! is called by lifecycle state change
  void OnLifeCycleEvent();

  //! publish shutdown trigger for lifecycle eventsystem, executes all shutdown_hooks
  void publishShutdown();

  void publishConfigureInput();

  //! all with addOnResetHook() registered reset hooks
  std::vector<next::sdk::events::notify_hook> reset_hooks;

  //! all with addOnShutdownHook() registered shutdown hooks
  std::vector<next::sdk::events::notify_hook> shutdown_hooks;

  unsigned int number_of_expected_nodes_ = 0;
  unsigned int number_of_nodes_in_config_state_ = 0;
  unsigned int number_of_nodes_in_config_output_state_ = 0;
};

} // namespace lifecycle_orchestrator
} // namespace controlbridge
} // namespace next

#endif // CONTROLBRIDGE_LIFECYCLE_ORCHESTRATOR_

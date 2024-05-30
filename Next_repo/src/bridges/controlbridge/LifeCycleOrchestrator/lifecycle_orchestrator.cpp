/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     lifecycle_orchestrator.cpp
 * @brief    reacts on next-p[layer events and sends out lifecycle trigger
 **/

#include "lifecycle_orchestrator.h"

#include <next/control/event_types/player_state_event.h>
#include <next/sdk/events/lifecycle_state.h>
#include <next/sdk/events/lifecycle_trigger.h>
#include <next/appsupport/lifecycle/lifecycle_statemachine_enum.hpp>

#include <functional>

namespace next {
namespace controlbridge {
namespace lifecycle_orchestrator {

bool LifecycleOrchestrator::start() { return GetInstance()->setup(); }

std::unique_ptr<LifecycleOrchestrator> &LifecycleOrchestrator::GetInstance() {
  static std::unique_ptr<LifecycleOrchestrator> lifecycle_orchestrator =
      std::unique_ptr<LifecycleOrchestrator>(new LifecycleOrchestrator());
  return lifecycle_orchestrator;
}

LifecycleOrchestrator::~LifecycleOrchestrator() {}

LifecycleOrchestrator::LifecycleOrchestrator() {
  lifecycle_trigger_ = std::make_unique<next::sdk::events::LifecycleTrigger>("");
  player_event_ = std::make_unique<next::control::events::PlayerStateEvent>("");
  lifecycle_event_ = std::make_unique<next::sdk::events::LifecycleState>("");
}

bool LifecycleOrchestrator::setup() {
  auto player_event_bind = std::bind(&LifecycleOrchestrator::OnPlayerEvent, this);
  player_event_->addEventHook(player_event_bind);
  player_event_->subscribe();

  auto lifecycle_event_bind = std::bind(&LifecycleOrchestrator::OnLifeCycleEvent, this);
  lifecycle_event_->addEventHook(lifecycle_event_bind);
  lifecycle_event_->subscribe();

  return true;
}

void LifecycleOrchestrator::publishReset() {
  // the event itself is it's own listener
  number_of_expected_nodes_ = (unsigned int)lifecycle_event_->getNumberOfListeners() - kNumberOfListenersSelf;
  number_of_nodes_in_config_state_ = 0;
  number_of_nodes_in_config_output_state_ = 0;

  // execute all registered hook
  for (auto hook : reset_hooks) {
    hook();
  }
  lifecycle_trigger_->publishTrigger(next::appsupport::lifecycleStatemachine::EState_toString(
                                         next::appsupport::lifecycleStatemachine::EState::RESET_STATE),
                                     "");
}

void LifecycleOrchestrator::publishShutdown() {
  // execute all registered hook
  for (auto hook : shutdown_hooks) {
    hook();
  }
  lifecycle_trigger_->publishTrigger(next::appsupport::lifecycleStatemachine::EState_toString(
                                         next::appsupport::lifecycleStatemachine::EState::SHUTDOWN_STATE),
                                     "");
}

void LifecycleOrchestrator::OnPlayerEvent() {
  next::control::events::PlayerMetaInformationMessage message = player_event_->getEventData();
  switch (message.status) {
  case next::control::events::PlayerState::EXIT_OPENING:
    publishReset();
    break;
  default:
    break;
  }
}

void LifecycleOrchestrator::publishConfigureInput() {
  lifecycle_trigger_->publishTrigger(next::appsupport::lifecycleStatemachine::EState_toString(
                                         next::appsupport::lifecycleStatemachine::EState::CONFIG_INPUT),
                                     "");
}

void LifecycleOrchestrator::OnLifeCycleEvent() {
  next::sdk::events::LifecycleStateMessage message = lifecycle_event_->getEventData();

  auto state = next::appsupport::lifecycleStatemachine::EState_fromString(message.state);
  switch (state) {
  case next::appsupport::lifecycleStatemachine::EState::CONFIG_STATE:
    // count nodes that started config
    number_of_nodes_in_config_state_++;
    break;
  case next::appsupport::lifecycleStatemachine::EState::CONFIG_OUTPUT:
    // count nodes that finishes configuring output
    number_of_nodes_in_config_output_state_++;
    break;
  default:
    break;
  }

  if ((state == next::appsupport::lifecycleStatemachine::EState::CONFIG_OUTPUT) &&
      (number_of_nodes_in_config_state_ == number_of_nodes_in_config_output_state_) &&
      (number_of_expected_nodes_ == number_of_nodes_in_config_output_state_)) {
    publishConfigureInput();
  }
}

void LifecycleOrchestrator::addOnResetHook(const next::sdk::events::notify_hook &hook) {
  reset_hooks.push_back(std::move(hook));
}

void LifecycleOrchestrator::addOnShutdownHook(const next::sdk::events::notify_hook &hook) {
  shutdown_hooks.push_back(std::move(hook));
}

} // namespace lifecycle_orchestrator
} // namespace controlbridge
} // namespace next

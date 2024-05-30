/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     lifecycle.cpp
 * @brief    Handling states, transitions and life cycle workflow
 */

#include <next/sdk/lifecycle/lifecycle_interface.hpp>

#include <ecal/ecal.h>

namespace next {
namespace sdk {
namespace lifecycle {

// Ctor that sets a default config map using the following optional parameters: eCal instance name
ComponentLifeCycle::ComponentLifeCycle(const std::string &instancename) {
  InitChain::ConfigMap mapconfig;
  mapconfig.emplace(EcalSingleton::getInstancenameCfgKey(), instancename);
  Init(mapconfig);
}

// Function that sets a specified config map
void ComponentLifeCycle::setConfigMap(const InitChain::ConfigMap &mapconfig) {
  lifecycle::LifeCycle::initmap = mapconfig;
}

// Function that initialize the component with life cycle
void ComponentLifeCycle::Init(const InitChain::ConfigMap &mapconfig) {
  lifecycle::LifeCycle::initmap = mapconfig;
  create();
}

// Component life cycle d-tor
ComponentLifeCycle::~ComponentLifeCycle() {}

LifeCycle::LifeCycle() { logger::debug(__FILE__, "The object with life cycle is constructed."); }

LifeCycle::~LifeCycle() {
  logger::debug(__FILE__, "The object with life cycle is destructed.");

  // Close logger - flush and reset console_sink / file_sink
  if (logger::isInitialized()) {
    logger::close();
  }

  if (lifecycle_state.end_state != NodeState::Finalized) {
    shutdown();
  }
}

// Return the given state as a string
const std::string LifeCycle::getStateAsString(const NodeState &state) {
  switch (state) {
  case NodeState::Inactive:
    return "Inactive";
  case NodeState::Active:
    return "Active";
  case NodeState::Finalized:
    return "Finalized";
  default:
    return "Unconfigured";
  }
}

// Return the given transition as a string
const std::string LifeCycle::getTransitionAsString(const TransitionType &transition) {
  switch (transition) {
  case TransitionType::Configuring:
    return "Configuring";
  case TransitionType::CleaningUp:
    return "CleaningUp";
  case TransitionType::ShuttingDown:
    return "ShuttingDown";
  case TransitionType::Activating:
    return "Activating";
  case TransitionType::Deactivating:
    return "Deactivating";
  case TransitionType::ErrorProcessing:
    return "ErrorProcessing";
  case TransitionType::None:
    return "None";
  default:
    return "Unknown";
  }
}

void LifeCycle::setLifeCycleState(NodeState start_state, NodeState end_state, TransitionType transition,
                                  TransitionState result_code) {
  lifecycle_state.start_state = start_state;
  lifecycle_state.end_state = end_state;
  lifecycle_state.transition = transition;
  lifecycle_state.result_code = result_code;
}

// Handle transitions to the next state of the node and update the lifecycle state
// Return true if the transition was successful
// Otherwise try to handle the errors by executing the code existing in onError() and return the success status
bool LifeCycle::transitionHandler(TransitionType transition) {

  if (current_transition != TransitionType::None)
    return false;

  current_transition = transition;

  for (auto it = transitions_list_.find(transition); it != transitions_list_.end(); it++) {
    if (it->second.first == current_state) {
      // set latest lifecycle state
      lifecycle_state.start_state = current_state;
      lifecycle_state.end_state = it->second.second;
      lifecycle_state.transition = transition;

      if (executeTransition(transition)) {
        current_state = it->second.second; // Update current state

        if (current_transition != TransitionType::ErrorProcessing) {
          current_transition =
              TransitionType::None; // No transition is executing anymore; the component is ready to do transitions
          lifecycle_state.result_code = TransitionState::SUCCESS;
        } else {
          lifecycle_state.result_code = TransitionState::FAILURE;
          if (current_state != NodeState::Unconfigured)
            return executeTransition(TransitionType::ErrorProcessing);
        }
        return true;
      } else {
        lifecycle_state.result_code = TransitionState::FAILURE;
        return errorProcessing();
      }
    }
  }
  logger::warn(__FILE__, "Invalid transition! Cannot determine the next state of the node for transition {} {} {}",
               getTransitionAsString(transition), "and state", getStateAsString(current_state));
  return false;
}

// Try to execute the transition code
// Return true if the code was executed without errors, or false otherwise
bool LifeCycle::executeTransition(TransitionType transition) {
  try {
    switch (transition) {
    case TransitionType::Configuring:
      onConfigure();
      break;
    case TransitionType::CleaningUp:
      onCleanup();
      break;
    case TransitionType::ShuttingDown:
      onShutdown();
      break;
    case TransitionType::Activating:
      onActivate();
      break;
    case TransitionType::Deactivating:
      onDeactivate();
      break;
    case TransitionType::ErrorProcessing:
      onError();
      break;
    default:
      return false;
    }
    logger::debug(__FILE__, "Transition {} {}", getTransitionAsString(transition), ": executed without errors.");
    return true;
  } catch (const std::exception &e) {
    logger::error(__FILE__, "Transition {} {}", getTransitionAsString(transition), "executed with errors:", e.what());
  }
  return false;
}

// Handle the errors by executing the code existing in onError() function
// If error handling is successfully the component will transition to Unconfigured
// If a full cleanup is not possible the component will transition to Finalized in preparation for destruction
bool LifeCycle::errorProcessing() {
  current_transition = TransitionType::ErrorProcessing;

  if (!executeTransition(TransitionType::ErrorProcessing)) {
    destroy();
    current_transition = TransitionType::None;
    setLifeCycleState(current_state, NodeState::Finalized, current_transition, TransitionState::FAILURE);
    current_state = NodeState::Finalized;
    return false;
  }
  return true;
}

// Set the initial state of the component as Unconfigured
void LifeCycle::create() {
  // Initialize logger
  if (!logger::isInitialized()) {
    logger::init(initmap);
  }

  current_state = NodeState::Unconfigured;
  current_transition = TransitionType::None;

  // set initial lifecycle state
  setLifeCycleState(current_state, current_state, current_transition, TransitionState::SUCCESS);
}

// Configure the component
// Change state from Unconfigured to Inactive by executing onConfigure function
// Call eCAL::Initialize();
bool LifeCycle::configure() {

  bool bFlagConfigurationSuccess = transitionHandler(TransitionType::Configuring);
  if (!bFlagConfigurationSuccess) {
    return false;
  }

  // Handle eCAL initialization
  if (eCAL::IsInitialized()) {
    use_ecal_singleton = false; // ecal was already initialized from outside lifecycle interface
    if (EcalSingleton != nullptr) {
      logger::warn(__FILE__, "eCAL::Initialize() called outside life cycle eCAL Singleton");
    }
  } else if (use_ecal_singleton) {
    if (EcalSingleton == nullptr) {
      EcalSingleton = &EcalSingleton::get_instance();
    }
    EcalSingleton::do_init(initmap);
  }

  return true;
}

// Cleanup the component
// Change state from Inactive to Unconfigured by executing onCleanup function
bool LifeCycle::cleanup() { return transitionHandler(TransitionType::CleaningUp); }

// Activate the component
// Change state from Inactive to Active by executing onActivate function
bool LifeCycle::activate() { return transitionHandler(TransitionType::Activating); }

// Deactivate the component
// Change state from Active to Inactive by executing onDeactivate function
bool LifeCycle::deactivate() { return transitionHandler(TransitionType::Deactivating); }

// Shutdown the component by changing the state either to Finalized or Unconfigured
// If both onShutdown and onError fail and no shutdown was requested then change the state to Unconfigured
// Otherwise change the state to Finalized and destroy the object
void LifeCycle::shutdown() {
  bool bFlagShutdownSuccess = transitionHandler(TransitionType::ShuttingDown);
  destroy();
  if (!bFlagShutdownSuccess) {
    // shutdown and error processing failed
    create(); // set state to Unconfigured
  }
}

// Destroy the component after shutdown, before calling the destructor
// call eCAL::Finalize();
void LifeCycle::destroy() {
  if (eCAL::IsInitialized()) {
    if (EcalSingleton != nullptr) {
      EcalSingleton::do_reset();
    } else {
      eCAL::Finalize();
    }
  }
}

} // namespace lifecycle
} // namespace sdk
} // namespace next

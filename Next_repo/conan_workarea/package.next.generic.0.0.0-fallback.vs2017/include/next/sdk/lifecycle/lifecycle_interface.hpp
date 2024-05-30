/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     lifecycle_interface.hpp
 * @brief    Definition of interface for using life cycle
 *
 */
#ifndef NEXTSDK_LIFECYCLE_INTERFACE_H
#define NEXTSDK_LIFECYCLE_INTERFACE_H

#include <map>

#include "../ecal/ecal_singleton.hpp"
#include "../logger/logger.hpp"
#include "../sdk_config.hpp"

namespace next {
namespace sdk {
namespace lifecycle {

/**
 * Life cycle definition class
 * Used for defining states and transitions
 */
class LifeCycle {

protected:
  typedef enum {
    Unconfigured, // initial state of node, after being instantiated
    Inactive,     // the node is not currently performing any processing
    Active, // the node performs any processing, responds to service requests, reads and processes data, produces output
    Finalized // terminal state of node, before being destroyed
  } NodeState;

  typedef enum {
    None,
    Configuring,
    CleaningUp,
    ShuttingDown,
    Activating,
    Deactivating,
    ErrorProcessing
  } TransitionType;

  typedef enum { SUCCESS = true, FAILURE = false } TransitionState;

  // List with valid transitions
  std::multimap<TransitionType, std::pair<NodeState, NodeState>> transitions_list_ = {
      {TransitionType::Configuring, std::make_pair(NodeState::Unconfigured, NodeState::Inactive)},
      {TransitionType::CleaningUp, std::make_pair(NodeState::Inactive, NodeState::Unconfigured)},
      {TransitionType::Activating, std::make_pair(NodeState::Inactive, NodeState::Active)},
      {TransitionType::Deactivating, std::make_pair(NodeState::Active, NodeState::Inactive)},
      {TransitionType::ShuttingDown, std::make_pair(NodeState::Inactive, NodeState::Finalized)},
      {TransitionType::ShuttingDown, std::make_pair(NodeState::Unconfigured, NodeState::Finalized)},
      {TransitionType::ShuttingDown, std::make_pair(NodeState::Active, NodeState::Finalized)},
      {TransitionType::ErrorProcessing, std::make_pair(NodeState::Inactive, NodeState::Unconfigured)},
      {TransitionType::ErrorProcessing, std::make_pair(NodeState::Active, NodeState::Inactive)}};

  struct LifecycleState {
    NodeState start_state;
    NodeState end_state;
    TransitionType transition;
    TransitionState result_code;
  };

  // Ecal Singleton usage
  // Allow use of life cycle management without eCAL Singleton
  // The instance of eCAL Singleton will be set from the derived class
  next::sdk::EcalSingleton *EcalSingleton = nullptr;
  InitChain::ConfigMap initmap = InitChain::ConfigMap();

  // Virtual functions for defining the code for each transition

  // Load node configuration and conduct any required setup
  virtual void DECLSPEC_nextsdk onConfigure(){};

  // Clear all state and return the node to a functionally equivalent state as when first created
  virtual void DECLSPEC_nextsdk onCleanup(){};

  // Do any final preparations to start executing
  virtual void DECLSPEC_nextsdk onActivate(){};

  // Execute any cleanup to start executing - should reverse the onActivate changes
  virtual void DECLSPEC_nextsdk onDeactivate(){};

  // Do any cleanup necessary before destruction
  virtual void DECLSPEC_nextsdk onShutdown(){};

  // Clean up all state from any previous state
  virtual void DECLSPEC_nextsdk onError(){};

private:
  // Current state of the node
  NodeState current_state;

  // Current transition
  TransitionType current_transition;

  bool use_ecal_singleton = true;

  // Latest lifecycle state
  // published every time that a transition is triggered, whether successful or not.
  LifecycleState lifecycle_state;

  bool DECLSPEC_nextsdk transitionHandler(TransitionType transition);
  bool DECLSPEC_nextsdk executeTransition(TransitionType transition);
  bool DECLSPEC_nextsdk errorProcessing();
  void DECLSPEC_nextsdk setLifeCycleState(NodeState start_state, NodeState end_state, TransitionType transition,
                                          TransitionState result_code);

public:
  DECLSPEC_nextsdk LifeCycle();
  DECLSPEC_nextsdk virtual ~LifeCycle();

  inline NodeState DECLSPEC_nextsdk getCurrentState() { return current_state; }
  inline TransitionType DECLSPEC_nextsdk getCurrentTransition() { return current_transition; }
  const std::string DECLSPEC_nextsdk getStateAsString(const NodeState &state);
  const std::string DECLSPEC_nextsdk getTransitionAsString(const TransitionType &transition);

  // Internal functions called when triggering a transition

  void DECLSPEC_nextsdk create();
  bool DECLSPEC_nextsdk configure();
  bool DECLSPEC_nextsdk cleanup();
  bool DECLSPEC_nextsdk activate();
  bool DECLSPEC_nextsdk deactivate();
  void DECLSPEC_nextsdk shutdown();
  void DECLSPEC_nextsdk destroy();

}; // LifeCycle

/**
 * Life cycle interface definition class
 * Used for managing life cycle of nodes
 */
class ComponentLifeCycle : public LifeCycle, public next::sdk::EcalSingleton {

private:
  // Initialization of component
  void DECLSPEC_nextsdk Init(const InitChain::ConfigMap &mapconfig);

public:
  // Ctor that sets a default config map using the following optional parameters: eCal instance name
  DECLSPEC_nextsdk ComponentLifeCycle(const std::string &instancename = "Life_cycle_managed_node");

  // Function that sets a specified config map
  void DECLSPEC_nextsdk setConfigMap(const InitChain::ConfigMap &mapconfig);

  // Dtor
  DECLSPEC_nextsdk virtual ~ComponentLifeCycle();
};

} // namespace lifecycle
} // namespace sdk
} // namespace next

#endif // NEXTSDK_LIFECYCLE_INTERFACE_H

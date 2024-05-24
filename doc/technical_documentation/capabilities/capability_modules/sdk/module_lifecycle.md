Lifecycle Management {#sdk_module_lifecycle}
=============

Module Overview Contents
* @ref sdk_module_main
* @ref sdk_module_config
* @ref sdk_module_event_system
* @ref sdk_module_initchain
* @ref sdk_module_ecalsingleton
* @ref sdk_module_tracy
___

# General description

The lifecycle Interface provides functionality that allows greater control over the state of the nodes.

The transitions from one state to another allow the control of initialization before it allows any component to begin executing its behavior.
It will also make it possible to handle errors and cleanup before reuse.

**Lifecycle Architecture (ROS2 Design):**
![lifecycle Architecture](https://design.ros2.org//img/node_lifecycle/life_cycle_sm.png "lifecycle Architecture")

There are 4 primary states:
- Unconfigured
- Inactive
- Active
- Finalized

There are 6 transition:
- Configuring
- CleaningUp
- ShuttingDown
- Activating
- Deactivating
- ErrorProcessing

There are 7 transitions exposed to a supervisory process:
- create
- configure
- cleanup
- activate
- deactivate
- shutdown
- destroy

The implemented lifecycle uses ROS2 Design. An extensive documentation of states and transitions can be found at the following link:
http://design.ros2.org/articles/node_lifecycle.html

# Lifecycle Flow in NEXT

This plantuml shows the `LifeCycle` state machine defined within `LifecycleBase`

The main config and run lifecycle flows from Reset -> Config -> Reader / Execute -> Reset

All State transitions are handled and called within the NEXT Simulation Framework. Execution triggers from external
nodes are blocked until the component is in state Ready. The transition to Execute is also handled automatically by
the NEXT Component Interface

Also startExecution and exitExecution are called and handled via by NEXT Component Interface

@startuml
Init    : Init chain: setup frameworks and components
Reset   : delete/set config back
Config  : handover configuration like Ports, trigger etc \n from USER to NEXT INTERFACE
ConfigOutput : ReadConfig and CreatePublishers
ConfigInput : ReadConfig and CreateSubscribers
Ready   : can receive new events to __trigger__ Execute
Execute : blocks state transition until __trigger__ is processed
Shutdown: exit the node/application
Error   : any Error ends here
state TimeoutThread #yellow
TimeoutThread : Wait and trigger event
state ExternalTrigger #orange
ExternalTrigger : Decide when to ConfigInput

[*] --> Init
Init --> Reset
Reset --> Config
Config --> ConfigOutput
ConfigOutput -[#green]->  ConfigInput : EVENT_CONFIG_INPUT \n only on startup
ConfigOutput --> TimeoutThread
TimeoutThread -->  ConfigInput : EVENT_CONFIG_INPUT
ConfigOutput --> ExternalTrigger : PublishState
ExternalTrigger --> ConfigInput : EVENT_CONFIG_INPUT
ConfigOutput -->  Reset : EVENT_RESET
ConfigInput --> Ready
Ready --> Shutdown : EVENT_SHUTDOWN
Ready --> Reset : EVENT_RESET
Ready --> Execute : START_EXECUTION
Execute --> Ready : EXIT_EXECUTION
Error --> Shutdown
Shutdown --> [*]
@enduml

# Integration within nodes & Usage

1. The use of the lifecycle management involves the derivation of the node class with the class of the lifecycle interface, located at next::sdk::lifecycle::ComponentLifeCycle.

2. The code to be executed in the case of each transition can be defined in the following virtual functions inherited from the lifecycle interface:
- onConfigure: load node configuration and conduct any required setup
- onActivate: do any final preparations to start executing
- onShutdown: do any cleanup necessary before destruction (called automatically at destruction)
- onCleanup: clear all state and return the node to a functionally equivalent state as when first created
- onDeactivate: execute any cleanup to start executing - should reverse the onActivate changes
- onError: clean up all state from any previous state

Redefining these virtual functions is optional – if a transition function is not defined in the derived class then calling the corresponding supervisory function will have the effect of changing the state.

3. The initialization of transitions is made by calling the following functions:
- configure()
- cleanup()
- activate()
- deactivate()
- shutdown()

The create() and destroy() transitions are managed internally and are exposed through the constructor and destructor.

The functions will return a boolean value that represents the success of the transition and can be used to validate whether the node is in the state required to execute the desired actions.

5. A simple example of using the lifecycle interface can be found here:
<!-- TO DO: update the link after renaming the repository -->
https://github-am.geo.conti.de/ADAS/next_sdk/tree/master/tests/component/testlifecycleapp/lifecycle_test.cpp 

# Integrated components

The lifecycle interface uses the following components of Next SDK:

#### 1. eCal Singleton

- eCAL::Initialize() is called within the Configuring transition, when using the configure() function;
- eCAL::Finalize() is called within the ShuttingDown transition, when using the shutdown function.

The two functions can also be used from the outside of the singleton, in which case they will not be called for the second time.

# Lifecycle
The next_sdk provides a simple state machine for common lifecycle management.

The basic idea is, that all nodes have a similar behavior at init and shutdown and can react on the same RESET and SHUTDOWN event.

@startuml
Init    : Initchain: setup frameworks and components
Reset   : delete/set config back
Config  : establish configuration like Ports etc
Ready   : can receive new events to __trigger__ Execute
Execute : blocks state transition until __trigger__ is processed
Shutdown: exit the node/application
Error   : any Error ends here

[*] --> Init
Init --> Reset
Reset --> Config
Config --> Ready
Ready --> Shutdown : EVENT_SHUTDOWN
Ready --> Reset : EVENT_RESET
Ready --> Execute : START_EXECUTION
Execute --> Ready : EXIT_EXECUTION
Error --> Shutdown
Shutdown --> [*]
@enduml

The next_sdk lifecycle inits its state machine and calls user code with "enterInit()" to init the node, than it transits to the next state "Reset".

Reset calls user code "enterReset()" followed by Config state which also executes user code "enterConfig()".

After this init chain the state machine ends up in the Ready state. There it waits for events.

If a known event appears like EVENT_SHUTDOWN or EVENT_RESET then it performs the transitions like described in the state machine above.

to set the statemachine into EXECUTE state the user can call the method "startExecution()" and to exit it use "exitExecution()".

While the statemachine is in EXECUTION state and a "Reset" or "Shutdown" event is triggered, then the statemachine spawns a new thread which waits 10 seconds (configurable), if after 10 seconds the statemachine is still in EXECUTE state then it switches to Error state and performs a shutdown.

To use the lifecycle management just derive from the class `LifecycleBase` in lifcycle_base.hpp and overwrite the methods:
```cpp
  virtual bool enterConfig() = 0;
  virtual bool enterReset() = 0;
  virtual bool enterInit() = 0;
  virtual bool enterShutdown() = 0;
  virtual bool onError(std::string error, std::exception *e = nullptr) = 0;
```
to start the statemachine call the method run()
NOTE: the run() method does not return it blocks until shutdown is triggered
for an example see tests/component/testlifecycleapp
-->

# Lifecycle with the simulation flow

# LifeCycle

* currenlty a simulation node starts up and configures
* searches for connection on inport level
* if connection is to another node the connections are not available yet due to the outport / inport configuration not bein aligned

@startuml
participant player
rec -> player : finished
player -> controlbridge : recording loaded
participant rec

box "controlbridge" #Orange
participant controlbridge
participant lifecyclemanager
endbox

lifecyclemanager -> node1 : reset
lifecyclemanager -> node2 : reset
node1 -> lifecyclemanager : resetFinished
node2 -> lifecyclemanager : resetFinished

box "node1" #LightBlue
participant node1
participant node1inport1
participant node1output1
endbox

box "node2" #LightGreen
participant node2
participant node2inport2
participant node2output2
endbox

== ConfigOutput ==

controlbridge -[#red]> node1 : configOutput
controlbridge -[#red]> node2 : configOutput

node1 -> node1output1 : config
node2 -> node2output2 : config

node1 -> controlbridge : configFinished
node2 -> controlbridge : configFinished

== ConfigInput ==
controlbridge -> lifecyclemanager : are all nodes set up?
lifecyclemanager -> controlbridge
controlbridge -[#red]> node1 : configInput
controlbridge -[#red]> node2 : configInput

node1inport1 -> node2output2 : missing
node2inport2 -> rec : config

node1 -> controlbridge : inputFinished
node2 -> controlbridge : inputFinished

@enduml






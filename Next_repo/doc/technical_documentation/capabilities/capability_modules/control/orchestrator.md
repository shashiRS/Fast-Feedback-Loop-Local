Orchestrator {#control_capabilities_orchestrator}
==============
[TOC]

The \named{Orchestrator}\glos{orchestrator} is the central module within NEXT Control to make execution flow available for the user.
It is based internally on the \named{Event System}\glos{event_system} of \named{NEXT SDK}\glos{next_sdk}.

The \named{Orchestrator}\glos{orchestrator} is build up from two classes which interact with each other
* next::control::orchestrator::OrchestratorMaster  providing functionality to trigger multiple next::control::orchestrator::OrchestratorTriggerHandler based on a defined flow table and trigger signals
* next::control::orchestrator::OrchestratorTriggerHandler to receive the respective trigger and execute attached hooks for specified trigger names

All execution calls are realized with a synchronous remote procedure call and rely on the \named{ServiceServer} / \named{ServiceClient} mechanism of \named{eCAL}\glos{eCAL}.


## Basic Execution flow

The triggers which are transferred also incorporate a trigger event message from the next::control::orchestrator::OrchestratorMaster to the next::control::orchestrator::OrchestratorTriggerHandler and a response scheme to deliver error or status updates from `next::control::orchestrator::OrchestratorTriggerHandler to next::control::orchestrator::OrchestratorMaster.

@startuml
OrchestratorMaster-> OrchestratorMaster : setNodeName(nodeName)
OrchestratorTriggerHandler-> OrchestratorTriggerHandler : setNodeName(nodeName)
OrchestratorTriggerHandler -> OrchestratorMaster: registerHook (methodName)
OrchestratorMaster -> OrchestratorTriggerHandler: TriggerEvent (contains Cycle Id)
OrchestratorTriggerHandler -> OrchestratorMaster: Response (containing status or error message)
@enduml

## Extending Execution Flow with Custom Scheduler

The \named{OrchestratorTriggerHandler}` also contains functionality to send out a trigger on its own. This can be used to emulate a scheduler with help of the \named{NEXT Simulation Framework}\glos{simulation_framework}

@startuml

"NEXT-Player (OrchestratorMaster)" --> "Scheduler (OrchestratorTriggerHandler)": TriggerEvent (contains Cycle ID)

group project scheduler: enclosed project specific part

"Scheduler (OrchestratorTriggerHandler)" -> "Algo1 (OrchestratorTriggerHandler)": TriggerEvent (contains adapted BaseData for example)
"Algo1 (OrchestratorTriggerHandler)" -> "Scheduler (OrchestratorTriggerHandler)": Response (contains customized Response)

"Scheduler (OrchestratorTriggerHandler)" -> "Algo2 (OrchestratorTriggerHandler)": TriggerEvent (contains adapted BaseData for example)
"Algo2 (OrchestratorTriggerHandler)" -> "Scheduler (OrchestratorTriggerHandler)": Response (contains customized Response)
end

"Scheduler (OrchestratorTriggerHandler)" -> "NEXT-Player (OrchestratorMaster)": Response containing status or error message
@enduml

@ref control_capabilities_orchestrator_details

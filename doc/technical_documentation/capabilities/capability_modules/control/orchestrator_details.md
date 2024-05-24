Orchestrator Details {#control_capabilities_orchestrator_details}
==============
[TOC]

# Orchestrator Basic Diagram

* Diagram of how Master interacts with Clients using Flows.

<img src="Orchestration_new.png" width="800">


## Flow

The \named{Flow} is the data structure comprising following fields:
```{.py}
{
    "id": "flow1",
    "meta": {
        "delay": 2,
        "sync": true
    },
    "type": "time_stamp",
    "value": "20000",
    "synch_timestamp_url": "timestamp_url",
    "synch_id_url": "synch_id_url",
    "clients": {
        "client1": {
            "portname1": ""
        },
        "EmfNodeTriggering": {
            "runSequencer_response": ""
        },
        "componentName": {
            "componentInstanceName_m_simpleTrigger": ""
        }
    }
}

```

* `id` : Unique ID of the flow.
* `meta` : Meta information if used, it has two properties delay and sync.
    *   `delay` : when enabled would introduce caching mechanism in flows, which means if there are many flows present there would be `delay` in execution by factor of value of `delay`.
    *   `sync`: When true it would execute flow in synchronously, otherwise asynchronously which is achieved by spawning its own thread of execution.
* `type`: What kind of flow it is? it has possible values and they are time_stamp, cycle_start, cycle_end and signal_name.
    * what does each value mean to flow?
    * if value is `time_stamp`: execute the flow when time value is more than the specified one from the component.
    * if value is `signal_name`: execute the flow when component specifies the same signal_name.
    * if value is `cycle_start` or `cycle_end`: execute the flow when cycle_id specified from the component matches with either cycle_start or cycle_end.
*  `value`: It can be a URL when type is `signal_name` (e.g. "ADC4xx.FDP_21P.FDP_ObjectData"), can be a  numeric value if type is `time_stamp` or `cycle_start`/`cycle_end` (e.g. 1644562260)
* `clients` : Its an array of targets who would be called after the `value` for `type` in the flow would be matched.
* e.g "EmfNodeTriggering", "runSequencer_response"

## Payload
* Payload  `OrchestratorTrigger` can be used to pass data from OrchestratorMaster to OrchestratorTriggerHandler.
* `OrchestratorTrigger` is wrapping a time_stamp and flow_id.

## Example Code
* \named{Component} which uses OrchestratorTriggerHandler to listen.

```{.cpp}
next::control::orchestrator::OrchestratorTriggerHandler orchestrator_client("EmfNodeTriggering");
orchestrator_client.registerHook("runSequence_response",
                                 [] (next::control::orchestrator::OrchestratorErrors &error)
   {
      std::cout<<" callback called "<<"\n;
   }
);
```


* \named{Component} which uses OrchestratorMaster, triggering the remote call.

```{.cpp}

next::control::orchestrator::OrchestratorMaster orchestrator_master;

std::string node_name ="EmfNodeTriggering";

std::string method_name ="runSequence_response";

orchestrator_master.fireTrigger(node_name+method_name);

```

## Explanation by Flow property
##delay: When flow has some delay, orchestrator caches the execution for that \named{type} of Flow.

@startuml
Player->OrchestratorMaster:reads_json
OrchestratorMaster->OrchestratorMaster:flow_has_delay
Player->OrchestratorMaster:sends_package_every_cycle
OrchestratorMaster->OrchestratorMaster:queues_packages_for_that_type
OrchestratorMaster->OrchestratorMaster:execute_flow_popping_queue
@enduml
<br>


##type: When flow has a type SignalName
@startuml
Player->OrchestratorMaster:reads_json
OrchestratorMaster->OrchestratorMaster:flow_has_signal_name
Player->OrchestratorMaster:sends_package_every_cycle
OrchestratorMaster->OrchestratorMaster:checks_and_compares_for_signal_name
OrchestratorMaster->OrchestratorMaster:fires_remote_method_when_signal_name_is_found
@enduml
<br>

##type: When flow has a type TimeStamp

@startuml
Player->OrchestratorMaster:reads_json
OrchestratorMaster->OrchestratorMaster:flow_has_time_stamp
Player->OrchestratorMaster:sends_package_every_cycle
OrchestratorMaster->OrchestratorMaster:compares_difference_between_old_saved_time_and_sent_timestamp_with_flow_time_stamp
OrchestratorMaster->OrchestratorMaster:fires_remote_method_when_time_stamp_difference_is_greater
@enduml
<br>

##type: When flow has a type CycleID

@startuml
Player->OrchestratorMaster:reads_json
OrchestratorMaster->OrchestratorMaster:flow_has_cycle_id
Player->OrchestratorMaster:sends_package_every_cycle
OrchestratorMaster->OrchestratorMaster:compares_cycle_id
OrchestratorMaster->OrchestratorMaster:fires_remote_method_when_cycle_id_is_same
@enduml
<br>

## Error handling in Orchestrator
* Orchestrator doesn't throw any exception or handle errors in such way.
* Orchestrator's functions return bool values, true meaning function was a success and false meaning it failed.

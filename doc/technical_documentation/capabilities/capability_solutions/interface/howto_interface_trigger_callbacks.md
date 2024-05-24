Interface Trigger Callbacks {#howto_interface_trigger}
==========================

Interface Setup:
* @ref interface_main
* @ref howto_interface_input_ports
* @ref howto_interface_output_ports

___

[TOC]

# Setup Trigger

The Next Component Interface provides a function to set up trigger Callbacks for your component.

To do this the ``next::NextComponentInterface::SetupTriggers`` function needs to be called. This function takes a list of pairs as input. The pair consists of a ``string`` and a ``triggerCallback``. The given string is the \named{Trigger Name}. This needs to match the naming given in the flow table provided to the \named{Orchestrator}.(TODO: Add link to orchestrator). The ``triggerCallback`` is the function that shall be executed when the trigger is fired. The function needs to return an integer and take a string as parameter. The integer value will be used to verify that the funciton executed successful. 
The string will be used as error message.

Below is explained how a trigger will be handled and how the data for the component can be received.

# Dataflow Trigger Callback

In the diagram below the dataflow during a callback is shown in more detail.<br>
The code starts in the ``OnConfig`` state. During this state the input ports are setup (More details [here](@ref howto_interface_input_ports)).<br>
Part of the input port setup is the setup of the \named{Sync Method}\glos{sync_mode}. During this step a map is filled which maps the port names to the selected \named{Sync Modes}.<br>
Afterwards for each port an extractor is created and mapped to the port name.<br>
Finally the subscribe URL is fetched from the configuration and also mapped to each port name.

Triggers will then be fired from the orchestrator client of the component.
The trigger will start the OnTrigger Callback function of the Next Component Interface.
The OnTrigger function forwards the trigger info to the SyncManager. Depending on the previously set \named{Sync Mode} the \named{Sync Manager}\glos{sync_manager} decides which reference value is needed for the call. If the \named{Sync Mode} is set up as \named{SyncRef} an additional step to get the \named{Sync Value} from the \named{SyncRef} is needed. Otherwise the \named{Sync Values} can be taken directly from the trigger.
The \named{Sync Values} are mapped to each Port Name in the \named{Sync Manager}.
As soon as the \named{Sync Values} are updated, the \named{Component Code} will be triggered. In the code the component can use the ```getTriggerAsyncPush``` function to get the available data. To call this function only the port name is needed.

In the function all previously mapped information is fetched. This inlcudes:
1. The \named{Data Extractor} of the port (The Sync Algo is already set up for this extractor)
2. The Subscribe URL of the port
3. The SyncValue of the port

With this information the \named{Data Extractor} can get the correct data from the \named{UDex} and return it to the \named{Component Code}.
  
@startuml Dataflow before each Trigger Callback
hide empty description

 state OnConfig #orange {
   SetupInputPort --> createExtractors 

   map_of_extractors_ --> SetupSyncMethods
   SetupSyncMethods --> map_of_extractors_
   

   map_of_extractors_ --> getValidSubscribeUrl
   getValidSubscribeUrl --> map_of_extractors_
   getValidSubscribeUrl --> map_of_subscribe_urls_#green

   
   createExtractors --> map_of_extractors_ #green
 }
 Orchestrator #grey --> Next.OnTriggerCallback
 state OnExecute {

 Next.OnTriggerCallback: Callback internal to the NEXT interface
 Next.SyncManager: manages reference values for all registered ports
 CallUserExecute: Eco Glue Code

 state Next.SyncManager #yellow {
   m_sync_config_map: map all ports to sync config (syncType, reference value)
   state triggerInfo <<expansionInput>>
   state SyncRefData <<expansionInput>>

   state forkSyncMode   <<fork>>
   triggerInfo --> forkSyncMode
   SetSyncConfig -> forkSyncMode : Set Sync mode
   forkSyncMode --> FillReferenceValue : basic sync mode
   forkSyncMode -> ReadSyncRef : syncRef mode
   ReadSyncRef -> FillReferenceValue
   SyncRefData -> ReadSyncRef
   SetupSyncMethods --> SetSyncConfig : read from Config
   SetSyncConfig --> SetupSyncMethods : SyncAlgoConfig
   FillReferenceValue --> m_sync_config_map : update values
   FillReferenceValue --> CallUserExecute
   m_sync_config_map #green --> GetSyncValue
    
 }

 state next.getTriggerAsyncPush #yellow {
   state PortName <<expansionInput>>
   PortName -> Extractor
   map_of_extractors_ --> Extractor
   Extractor --> GetSyncExtractedData : use reference values
   GetSyncValue --->  GetSyncExtractedData
   map_of_subscribe_urls_ --> GetSyncExtractedData
 }

 Next.OnTriggerCallback --> triggerInfo

 state CallUserExecute #lightblue {
   state PortData <<expansionOutput>>
   getTriggerAsyncPush -> PortName : portname
   getTriggerAsyncPush --> ComponentExecute : cycleId
   GetSyncExtractedData --> PortData
   PortData -> ComponentExecute
 }
 }
 @enduml

# Sequence Diagram Trigger Callbacks

In this diagram the sequence of how to get the synchronized data into the component is displayed to show the timing during a trigger callback.<br>
As soon as a trigger is fired the interface will call the ``FillReferenceValue`` function. This function loops through all available ports and updates the \named{Sync Values}\glos{sync_value} based on the set \named{Sync Modes}\glos{sync_mode} from the configuration.

After the correct values are updated for each port, the component code gets started.<br>
To get the proper values for the component, the function ``GetSyncValue`` can be called. This returns the previously synced values.

@startuml Sequence Diagram Add Trigger Callback
participant Orchestrator order 10
participant Component order 20
participant NextInterface order 30
participant SyncManager order 50
participant FillSyncValuePerPort order 60

Orchestrator -> NextInterface: trigger(triggerInfo{})
activate NextInterface
NextInterface -> SyncManager: FillReferenceValue(triggerInfo{trigger_name, signal_type, signal_value})
activate SyncManager
group loop [for each port]
SyncManager --> FillSyncValuePerPort: triggerInfo
activate FillSyncValuePerPort
FillSyncValuePerPort --> FillSyncValuePerPort: Check SyncMode
FillSyncValuePerPort --> FillSyncValuePerPort: Set SyncValue from Trigger
FillSyncValuePerPort --> SyncManager: Fill map(portName, SyncValue)
deactivate FillSyncValuePerPort
end
SyncManager --> NextInterface
deactivate SyncManager

NextInterface -> Component: callback()
activate Component
Component -> NextInterface : getTriggerAsyncPush(port_name)
NextInterface -> SyncManager : GetSyncValue(port_name)
activate SyncManager
return SyncValue{SignalType, dataType}
NextInterface --> Component: synchronized_extracted_data(DataPackage{void*, size_t})
return : execution ended (error message)
deactivate Component
NextInterface -> Orchestrator : execution ended (error message)
deactivate NextInterface
@enduml

# SnycRef

What is the SnycRef 
The Syncref values define what data is expected on each port for each cycle. It is a meas-frozen value available with the recording.
When simulating data it is possible, that information on a port is available earlier than expected. This would cause an issue, as the component is expecting different data.
Using the SyncRef for synchronizing enables the ports to get the matching data for each cycle.

@startuml
participant PortA order 10
participant PortB order 10
participant Component order 20
participant SyncRef order 30

== Sent at start ==
--> PortA : Package 1
--> PortB : Package 1
== Execute 1 ==
Component -> PortA : GetData
PortA -> Component : package 1
Component -> PortB : GetData
PortB -> Component : package 1
== Execute1 End ==


== Execute 2 ECU ==
Component -> PortA : GetData Cycle 2
PortA-> Component: package 2
Component -> PortB : GetData Cycle 2
PortB-> Component: package 1

== Execute 2 SIM ==
--> PortA: package 2
--> PortA: package 3
Component -> PortA : GetData 
PortA-> Component: **package 3**
Component -> PortB : GetData 
PortB-> Component: package 1

== Execute 2 SIM Syncref ==
Component -> SyncRef: GetData Cyle 2
SyncRef -> Component: PortA=Cycle 2 
SyncRef -> Component: PortB=Cycle 1 

Component -> PortA : GetData Cycle 2
PortA-> Component: package 2
Component -> PortB : GetData Cycle 1
PortB-> Component: package 1

@enduml

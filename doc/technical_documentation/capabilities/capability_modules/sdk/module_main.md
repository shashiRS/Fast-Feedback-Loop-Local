NEXT-SDK Module Overview{#sdk_module_main}
==============

Module Overview Contents
* @ref sdk_module_config
* @ref sdk_module_lifecycle
* @ref sdk_module_event_system
* @ref sdk_module_initchain
* @ref sdk_module_ecalsingleton
* @ref sdk_module_tracy

___


@startuml

package "Next Control" as n_component {
[Command_Control] as control_command
[Event_Control] as control_event
}

package "eCAL" as ecal

package "Protobuf" as protobuf

package "Boost" as boost

control_command -up-> Control_Client
control_command -up-> Control_Server
control_event -up-> Base_Event

n_component <|-down- boost
n_component <|-down- protobuf
n_component <|-down- ecal

@enduml
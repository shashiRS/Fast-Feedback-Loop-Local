Next Interface Overview {#interface_main}
==========================

Module Overview Contents
 * @ref howto_interface_input_ports
 * @ref howto_interface_output_ports
 * @ref howto_interface_trigger_callbacks

___

[TOC]

# Setup of a Simulation Node

The Diagram displays the usage of the \named{Next Component Interface}\glos{next_component_interface} together with the \named{ECO}\glos{eco} code generation.

@startuml Simulation Node using the Interface with ECO

left to right direction

component [Orchestrator] #Orange
component ECO #lightblue{
    portin inputPort1#green
    portin inputPort2#green
    portin inputPort3#green
    portout outputPort1#RosyBrown
    portout outputPort2#RosyBrown

    component ComponentCode
}

pub1 .. inputPort1
pub2 .. inputPort2
pub3 .. inputPort3
Orchestrator -d-> ComponentCode : triggerCallback 

inputPort1 --> ComponentCode 
inputPort2 --> ComponentCode 
inputPort3 --> ComponentCode 
ComponentCode --> outputPort1 
ComponentCode --> outputPort2 
outputPort1 .. sub1 
outputPort2 .. sub2 

@enduml

The component code is wrapped by auto-generated gluecode provided by \named{Eco}, which uses the \named{Next Component Interface}. The interface provides functions to set up the Input-Ports, Output-Ports and register Callback-Functions to triggers.


# Input Ports {#module_setup_inports}

Input ports are created to receive data from other components. Each Input Port has a \named{Data Extractor}\glos{data_extractor} to get the correct data from the \named{UDex}\glos{udex}.

To set up the input ports of a component, the interface requires an \named{InputPortConfig}\glos{input_port_config} as well as an optional \named{Adapter-DLL}. The \named{InputPortConfig} consists of:
- Port Name
- Port Version Number
- Port Size
- Data Type Name
- Sync Information

More details about the setup of the input ports can be found [here](@ref howto_interface_input_ports).


# Setup Output Ports

Output ports are created to publish data to other components. All output ports are registered in the \named{Data Publisher}\glos{data_publisher} of the component.

To set up the output ports of a component, the interface either a SDL-file or a list of \named{OutputPortConfig}\glos{output_port_config}. The \named{OutputPortConfig} consists of:
- Port Name
- Port Version Number
- Port Size

More details can be found [here](@ref howto_interface_output_ports).


# Setup Trigger

The interface provides the functionality to subscribe to a trigger with a callback function. The trigger name must match the naming given in the flow table entry provided to the \named{Orchestrator} via the \named{Player} (more information [here](todo link to player)).

In the Trigger Callback it is then possible to request the current data for the different iput ports. The data will be synchronized according to the settings from the [Setup Input Ports](@ref module_setup_inports).

More details can be found [here](@ref howto_interface_trigger_callbacks).


# Get Timestamp

The interface provides the functionality to get the current timestamp from a signal in the simulation environment. The information for the timestamp comes from the configuration. Following values need to be set:
'''
component_name_ + ".system_services.timestamp.mode"
component_name_ + ".system_services.timestamp.signal_url"
component_name_ + ".system_services.timestamp.signal_unit"
'''
The mode defines how you want to get the timestamp. Currently supported is "signal".
The signal URL defines which signal value you want to receive.
The signal unit defines the unit of the timestamp in the simulation environment. Currently supported are: Second (s), Millisecond (ms), Microseconds(us)(default), Nanoseconds (ns).

The Timestamp will be updated on every onTrigger call that was set up and the value will always be given in microseconds.

@startuml

hide empty description
state Sim.timestamp
state Next.Timestamp #green
state Next.TimestampSignalData #green

Sim.timestamp --> Next.SetupInputPorts : subscribe to data
Next.SetupInputPorts -> Next.OnTrigger : Set´s up trigger functionality
Next.SetupInputPorts -down-> Next.TimestampSignalData : Stores configuration in class member
ConfigSettings --> Next.SetupInputPorts : set unit
ConfigSettings --> Next.SetupInputPorts : set url
ConfigSettings --> Next.SetupInputPorts : set mode

Next.TimestampSignalData -> Next.FillCurrentTimestampMicroseconds
Next.OnTrigger -down-> Next.FillCurrentTimestampMicroseconds
Next.FillCurrentTimestampMicroseconds -> Next.Timestamp : sets available timestamp on every trigger

Next.Timestamp -up-> Next.GetTimeInMicroseconds
Next.GetTimeInMicroseconds -> eco.SystemService.getSystemTime

@enduml
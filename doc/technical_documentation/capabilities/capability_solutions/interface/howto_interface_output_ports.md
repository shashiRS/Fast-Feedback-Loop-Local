Interface Output Ports {#howto_interface_output}
==========================

Interface Setup:
* @ref interface_main
* @ref howto_interface_input_ports
* @ref howto_interface_trigger_callbacks

___

[TOC]

# Overview Setup Output Ports

Output ports are created to publish data to other components. All output ports are registered in the \named{Data Publisher}\glos{data_publisher} of the component.

This page explains how the output ports are set up in the Next Component Interface.

@startuml Overview Setup Output Ports
 hide empty description
    SetupOutputPorts --> Register Data Description (SDL)
    --> Iterate Over OutputPortConfigs
    if "Port Already available"
        -->[true] "put Information in Config"
    else
        -->[false] Register Data Description (OutputPortConfig)
        --> "put Information in Config"
 @enduml

# Setup Output Ports

As a user only the SetupOutput function is needed. As input parameter this function takes the path to an SDL-File  as well as a list of ComponentOutputPortConfig elements.<br>
The ComponentOutputPortConfig defines an Output port and consists of following elements:

| Element      | Description |
| ----------- | ----------- |
| Port Name      | Name of the port |
| Port Version Number   | Version Number of the Port   |
| Port Size   | Size of packages that can be published on the port |

The Setup Output function takes the provided SDL file and registers the given output ports in the Data Publisher. Afterwards the function iterates over the given ComponentOutputPortConfig elements. For each element it is checked, if the port already exists. If the port already exists, the next element is checked. If the port does not already exist, the information form the element will be used to register the port in the Data Publisher.

At the end, all configured ports will be written into the config.

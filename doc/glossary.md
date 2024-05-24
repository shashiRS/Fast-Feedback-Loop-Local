[TOC]

Glossary {#glossary}

@section GLOSSARY_next NEXT
Short for \named{NEXT Simulation Framework}\glos{simulation_framework}

@section GLOSSARY_binary_blob Binary blob
All data in the \named{NEXT Simulation Network}\glos{simulation_network} is sent via a binary format. The format is chosen so a cast to a usual c-struct is possible without any data loss or format conflict. An automated endianness adaption, byte-alignment and similar low-level issues are taken care of. 
The binary blob can be used as a usual memory block if and only if the correct corresponding c-struct is known. For unknown structures please use the Signal URL\glos{url}-based approach to receive and handle the data correctly. The \named{Binary blob} is an extracted version of the \named{Raw data blob}\glos{raw_data_blob}.

@section GLOSSARY_signal_tree Signal Tree
Tree structure showing all available current signals in the \named{NEXT Simulation Network}\glos{simulation_network}.

@section GLOSSARY_raw_data_blob Raw data blob
Non-extracted and raw memory blob from a recording or different format. Has the original endianness, byte-alignment and not received any low-level data treatment. USE WITH CARE.

@section GLOSSARY_simulation_framework NEXT Simulation Framework
The combined toolset of capability modules and capability solutions provided by NEXT

@section GLOSSARY_simulation_network NEXT Simulation Network
The distributed network of nodes to simulate, replay and debug algorithms on a CPU

@section GLOSSARY_udex NEXT-UDex (NEXT-Universal Data Exchange)
Provides core data transfer capabilites like publishing, subscribing and extracting data from the simulation network

@section GLOSSARY_control NEXT-Control 
Provides core capabilities for synchronizing and execution flow.

@section GLOSSARY_core_sdk NEXT-SDK
Provides core capabilities for developing like logging, crash dump, config management, event systems etc.

@section GLOSSARY_nodes Nodes
An executable which is part of the NEXT simulation framework. This node is able to receive or publish data with help of the NEXT-UDex

@section GLOSSARY_capability_module Capability Module
A low-level module to deliver core functionality within the simulation. Mostly realized with shared libraries and small modules for publishing, triggering, subscribing etc. Take __NEXT-UDex__ as an example.

@section GLOSSARY_capability_solution Capability Solution
High-level functionality which is usually solved with an executable using the \named{Capabilty Modules}\glos{capability_module}.

@section GLOSSARY_component_interface Component Interface
A single fully integrated header allowing for an easy integration into the simulation network with capabilities of input port, output ports, synchronization and triggering. Also involves capabilities for configuration managing, lifecycle management and command-line option parsing.

@section GLOSSARY_player NEXT-Player
A capability solution to replay recorded data with .rrec and .rec formats. Uses internally \named{NEXT-UDex}\glos{udex} and \named{NEXT-Control}\glos{control} for data flow\glos{data_flow} and execution flow\glos{execution_flow} management

@section GLOSSARY_bridges NEXT-Bridges
Union of the \named{NEXT-Databridge}\glos{databridge} and \named{NEXT-Controlbridge}\glos{controlbridge}

@section GLOSSARY_databridge NEXT-Databridge
Executable which connects the NEXT Simulation Framework to the GUI and delivers output data to the GUI

@section GLOSSARY_controlbridge NEXT-Controlbridge
Executable which connects the NEXT Simulation Framework to the GUI taking control over the simulation network

@section GLOSSARY_data_flow Dataflow
Describes the data flow in the NEXT Simulation Network. Includes lossless transport, buffering, synchronization and interpretation of data.

@section GLOSSARY_base_ctrl_data Base Control Data
Metadata for synchronizing simulation nodes to ensure proper data package handling and correct input sorting.

@section GLOSSARY_execution_flow Executionflow
Describes the execution of the different nodes\glos{nodes} within the NEXT simulation network. Functions could be executed in parallel or sequentially with and without error handling and checking.

@section GLOSSARY_eCAL eCAL (enhanced Communication Abstraction Layer)
Open-source middleware providing process synchronization and data transfer capabilities using shared memory, UDP and TCP/IP. Check out more [here](https://eclipse-ecal.github.io/ecal/index.html).

@section GLOSSARY_orchestrator Orchestrator
Module provided by the NEXT-Control repository to control the execution flow of simulation nodes\glos{nodes}

@section GLOSSARY_event_system Event System
Module provided by the NEXT-SDK to allow for event distribution with synchronous and asynchronous events. Used for various functionalities like status updates, execution control, configuration distribution and so on. Can be also accessed and used on its own.

@section GLOSSARY_url Signal-URL
All signals in the \named{NEXT Simulation network}\glos{simulation_network} have a unique address. This is generated automatically in the background using either the device name of the recorded data or the \named{Node}\glos{nodes}-name. Appended to the name is then the structure information to result in a unique URL.

@section GLOSSARY_sync_ref SyncRef
Reference Value that is used for synchronization.

@section GLOSSARY_data_publisher DataPublisher
Publisher for the \named{NEXT Simulation network}\glos{simulation_network}. Provides the only entry into the system for injecting data. It's also used in the \named{NEXT-Player}\glos{player}
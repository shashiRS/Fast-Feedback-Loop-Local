Capabilities {#capabilities_main}
==============

# General
* Cross-platform tooling for Linux and Windows 
* Native docker support and docker-images for NEXT releases
* flexible modular simulation network with low level \named{capability modules}\glos{capability_module} and high-level \named{capability solutions}\glos{capability_solution}
* Bricks package as an easy entry point to get components integrated into a \named{simulation network}\glos{simulation_network}

> A demo [video](https://web.microsoftstream.com/video/88d52252-c7ed-4b28-b197-327836d6e9f4k) is also available.

___

@section CapabilityModules Capability Modules

The \named{NEXT} simulation framework is an open network which emphasises on modularity. Core capability modules are available for all tasks and requirements and are used over different capabiliy solutions.

* Buffered and lossless data transfer between \named{simulation nodes}\glos{nodes}
  * Provided by \named{NEXT UDex}\glos{udex}
  * Configurable input buffer to ensure no data loss
  * Customizable synchronization methods
  * Performance optimized binary data transmission
* Synchronization of exectution flow
  * Provided by \named{NEXT Control}\glos{control}
  * Customizable synchronization modules
  * Customizable exectuion flows
  * Contol over the lifecycle of nodes
* Utilities for developing algorithms
  * Provided by \named{NEXT CoreSDK}\glos{core_sdk}
  * Crash-report
  * Logging
  * Performance tracking
  * Configuration management

<img src="capability_modules.png" width="600">

## NEXT-UDex

To become a member in the simulation network the main entry module is the so-called \named{NEXT-UDex}\glos{udex}.
\named{NEXT UDex} is the main backend component taking care of all data flow, data interpretation and data exploration.
It is an enclosed package which can be used on its own and represents the core of the simulation framework.

This \named{capability module}\glos{capability_module} incorporates all core functionalities to
* publish data into the system using the \named{DataPublisher}
* receive data with callback mechanisms from the \named{DataSubscriber}
* extract data on runtime provided by the \named{DataExtractor}
* explore the current dataflow of the system with help of the \named{SignalExplorer}

## NEXT-SDK

Additionally to taking part in the simulation network basic simulation tooling needs to be available. This is done with help of the
\named{NEXT-SDK}\glos{core_sdk}. It provides functions for logging, network events, initialization, configuration and much more.

This \named{capability module}\glos{capability_module} incorporates all core functionalities to
* log data into files and the console. A gathered log of all components is also in development
* configure components and transfer the configurations around the network as well as visualize them
* basic event system to notify participants of the network
* lifecycle management to incorporate reset and configuration events


## NEXT-Control

The remaining core capabilities missing are regarding orchestration and triggering of network participants. 

This \named{capability module}\glos{capability_module} incorporates all core functionalities to
* trigger network participants from a central orchestrator master
* forward triggers with help of the \named{triggerHandler}


___
@section CapabilitySolution Capability Solutions

* Replaying _*.rec_ files without any project specific adaptions realized with the \named{NEXT Player}\glos{player}
  * additional triggering functionality on a signal-basis
  * configurable simulation speed
  * batch-capability
* Visualizing data with help of simple plugins provided by the \named{NEXT-Databridge}\glos{databridge}
  * Traffic objects
  * Road markings
  * Point clouds
  * Primitives (lines, circles, boxes and more)
  * Ego vehicle movements
  * Table visualization
  * 2D plots
* Visualizing simulation network status with help of the \named{NEXT-Databridge}\glos{databridge}
  * Cpu profiler
  * Simulation node monitor
  * Data transfer details

## NEXT Player

The \named{NEXT Player}\glos{player} is mainly a recording reader and publisher.
* supports various signal formats saved within a _*.rrec_ file.
* taking care of orchestrating the components in the Simulation Framework.
* allows for custom trigger flows and easy adaptions to project needs.
<img src="player_capability_solution.png" width="600">


## NEXT Bridges

The \named{NEXT Bridges} are the connection between the GUI and the backend. Their main purpose is to:
* Visualize data from the Simulation Framework \named{NEXT-Databridge}\glos{databridge}
* Control the Simulation Framework \named{NEXT-Controlbridge}\glos{controlbridge}

<img src="databridge_capability_solution.png" width="600">

Visualizing data is a plugin and project driven activity which is highly configurable to enable a high performance on resimulating.
It has common interfaces to allow for shared efforts and improvements between projects. Examples for plugins of the \named{NEXT-Databridge}\glos{databridge}
* Performance analysis with a built-in performance tracker
* Traffic object visualization
* Signal structure
* Data exporter


The control flow of the \named{NEXT-Controlbridge}\glos{controlbridge} contains:
* Configuration capabilities
* Simulation flow control (_Play_, _Pause_ etc.)
* Forwarding simulation framework information to the GUI

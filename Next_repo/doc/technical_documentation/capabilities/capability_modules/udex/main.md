NEXT UDex in a nutshell {#udex_main}
==========================
[TOC]

\named{NEXT UDex} is a \named{Capability Module}\glos{capability_module} module of the \named{NEXT Simulation Framework}\glos{simulation_framework} which provides the entry functions to interact with the data in the \named{NEXT Simulation Network}\glos{simulation_network}.

<img src="core_capabilities_udex.png" width="800">

The functionalities include
* data subscription with help of the ```DataSubscriber```
* data extraction to interpret and use different data types using ```DataExtractor```.
* data publishing to the network\glos{simulation_network} provided by ```DataPublisher```.
* signal exploration to search for signals and data structures with help of the ```SignalExplorer```.

This means that all data related functions are summarized within \named{NEXT UDex} and allow anyone to interact with the \named{NEXT Simulation Network}\glos{simulation_network}.

Typical use-cases are
* \named{NEXT-Databridge} subscribing and interpreting data (```DataSubscriber``` and ```DataExtractor```) to send visualization data to the web-base GUI
* custom visualization executables used for example in the MFPlot
* data publishing and subscription within the simulated algorithm \named{Nodes}\glos{nodes}


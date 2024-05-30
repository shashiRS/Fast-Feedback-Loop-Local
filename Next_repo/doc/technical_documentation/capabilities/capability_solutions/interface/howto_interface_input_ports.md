Interface Input Ports {#howto_interface_input}
==========================

Interface Setup:
* @ref interface_main
* @ref howto_interface_output_ports
* @ref howto_interface_trigger_callbacks

___

[TOC]

# Overview Setup Input Ports

Input ports are created to receive data from other components. Each Input Port has a \named{Data Extractor}\glos{data_extractor} to get the correct data from the \named{UDex}\glos{udex}.

This page explains how the input ports are set up in the Next Component Interface. This includes creating the \named{Data Extractors}, setting the \named{Sync Methods} and getting the \named{Subscribe URLs} for the ports.

# Setup Input Ports

As a user only the ``next::NextComponentInterface::SetupInput`` function is needed. As input parameter this function takes the path to an GIA-Adapter DLL as optional parameter as well as a list of ComponentInputPortConfig elements.<br>
The ComponenetInputPortConfig defines an Input port and consists of following elements:

| Element      | Description |
| ----------- | ----------- |
| Port Name      | Name of the port |
| Port Version Number   | Version Number of the Port   |
| Port Size   | Size of packages that can be received on the port |
| Data Type Name   | Type of data that will be received on the port |
| Sync Mapping   | Mapping of signal URLs. Maps the URLs for the SyncRef URL and Port Name for Timestamp and Signal URL values for the port |
<br>

@startuml Overview Setup Input Ports
 hide empty description

 state OnConfig #orange {
   SetupInputPort --> createExtractors 
   SetupInputPort -> map_of_configured_inports_#green

   map_of_extractors_ --> SetupSyncMethods
   SetupSyncMethods --> map_of_extractors_
   

   map_of_extractors_ --> getValidSubscribeUrl
   getValidSubscribeUrl --> map_of_extractors_
   getValidSubscribeUrl --> map_of_subscribe_urls_#green
   
   createExtractors --> map_of_extractors_ #green
 }
@enduml

## Write into Config

The Setup Input function iterates over all provided ComponentInputPortConfig elements. Each element will be set up as an input.<br>
The first step is to write the data into the config. This ensures, that the config represents the current state of the port.

## Read from Config

> Concept to be implemented<br>
> To setup the input ports the function reads all needed information from the config.<br>
> This gives the possibility to change the setup of the input ports to match the current setup that shall be used on the maching.<br>

The function reads the given subscribe URLs from the config. It is possible to provide multiple URLs in the config. The setup will iterate from top to bottom over the provided list and will use the first valid URL. If no URL is available at this point, the first given URL will be used. If no URL is provided, the port will subscribe to it´s own name.

## Set Sync Config

For each port the Sync Config needs to be set. The Config consists of the Sync Mode and the Sync Algo.

The Sync Mode defines which value will be used to synchronize the data. Following options are provided:
- Timestamp
- CycleID
- SyncRef

The Sync Algo defines, how the values will be compared. Following options are provided:
- Get Last Element
- Find Exact Value
- Find Nearest Value
- Find Exact Header Timestamp

## Subscribe

After getting all necessary information the Data Extractor will be configured and used to subscribe to the set up topics.


# Sequence Diagram Setup Input Ports

In the following diagram the sequence of how to create the input ports of a component is shown.

@startuml Sequence Diagram Setup Input Ports
participant Component order 20
participant NextInterface order 30
participant Config order 40
participant SyncManager order 50

Component-> NextInterface: setupInport(InputPortConfig)

activate NextInterface
group loop [for each port]
NextInterface -> Config: put(portName) {version, size, type, sync{....}}
NextInterface --> NextInterface: make_shared<DataExtractor>(adapter_dll, queue_size)
Config --> NextInterface: get(subscribeUrl)

NextInterface -> SyncManager: setSyncConfig(portName, syncMapping)
activate SyncManager
SyncManager -> Config: get(SyncMode, SyncAlgo, SyncValues)
Config --> SyncManager 
SyncManager -> SyncManager : map portname to SyncMode and SyncAlgoConfig
SyncManager --> NextInterface: SyncAlgoCfg{syncAlgo, syncPath, signalUrl}
deactivate SyncManager
NextInterface --> NextInterface: dataExtractor->SetSyncAlgo(subscribeURL, syncCfg)

NextInterface --> NextInterface: SubscribeToUrl(subscribeUrl, dataExporter)
end

NextInterface --> Component: success
deactivate NextInterface
@enduml

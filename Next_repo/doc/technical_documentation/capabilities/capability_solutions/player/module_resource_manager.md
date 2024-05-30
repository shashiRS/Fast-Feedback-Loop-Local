Resource Manager{#player_module_resource_manager}
==============

Module Overview Contents
* @ref player_module_file_reader
* @ref player_module_state_machine
* @ref player_module_player_plugin
* @ref player_module_play_manager
* @ref player_module_publisher_manager

___

[TOC]

# Overview

This page explains the Next-Player ResourceManager module.

Main functionalities:
* package buffering
* udex services
* replay statistics

___

# Package buffering
@startuml
left to right direction
state StateOpening #LightBlue {
    ReplayProxy : mts::runtime::offline::read()
    ReplayProxy : mts::runtime::offline::get_packages()
}

state StatePlay #LightBlue {
      UdexService : PublishData(package)
}

state BufferQueue #LightBlue {

}

[*] --> StateOpening  : Open Recording Event
StateOpening --> ReplayProxy #LightBlue : Start Read Ahead Thread
ReplayProxy --> BufferQueue #LightBlue : Push package
BufferQueue --> UdexService #LightBlue : Pop package

note left of BufferQueue
Module used for package buffering
end note

@enduml
___
# Udex Service
UdexService serves as a wrapper for accessing the Udex library for data publishing.

## Register Data Sources
Once a recording is opened, we will register the data sources via Udex using the data sources and data source provider from the replay proxy.

@startuml
left to right direction
state UdexService #LightBlue {
    state RegisterDataSources #LightBlue {
         ReplayProxy : mts::runtime::offline::replay_proxy::get_data_source_provider()
         ReplayProxy : mts::runtime::offline::replay_proxy::get_data_sources()
         Udex : RegisterDataSources(data_sources, data_source_provider)
    }
}
ReplayProxy #LightBlue --> Udex #LightBlue
@enduml

## Publish Data
Wrapper for udex publish data.

___
# Replay Statistics
Replay statistics refer to the collection and analysis of data from player replay.

These statistics provide information and insights into the performance of the player.

These statistics can be found here: replay_statistics.
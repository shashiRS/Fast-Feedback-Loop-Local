Publisher Manager{#player_module_publisher_manager}
==============

Module Overview Contents
* @ref player_module_file_reader
* @ref player_module_state_machine
* @ref player_module_play_manager
* @ref player_module_player_plugin
* @ref player_module_resource_manager

___

[TOC]

# Overview

This page explains the Next-Player PublisherManager module.
Main functionalities:
* publish data</li>
* calculates speed factor and updates simulation speed
* maps triggers and acts as a an Orchestrator Master
___

# Publish Data
On playing the Next-Player will stepforward and pop packages from the BufferQueue, publish packages with UdexService, update replay statistics and adapt replay speed based on the [realtime_factor] section from config file.

The stepping mode can be defined in the config file on the [stepping] section, can be by cycle or by timestamp, by default is package based.

@startuml
left to right direction
state StatePlay #LightBlue {
    state ResourceManager #LightBlue{
        state ReplayStatistics #LightBlue
        state BufferQueue #LightBlue
    }
    state PublishManager : PublishDataPackage()
}

[*] --> StatePlay : Play Event
BufferQueue --> PublishManager #LightBlue : Pop package 
PublishManager --> ReplayStatistics
ReplayStatistics --> PublishManager 
@enduml

___

# Orchestrator Master
Next-Player acts as an orchestrator master for which the trigger it's set from the configuration file [trigger] section with a flow configuration that is loaded by the orchestrator.

Next-Player triggers the execution function of the orchestrator clients upon receiving a package from the configured triggers.

@startuml

"NEXT-Player (OrchestratorMaster)" --> "Scheduler (OrchestratorTriggerHandler)": TriggerEvent (contains Cycle ID)

group project scheduler: enclosed project specific part

"Scheduler (OrchestratorTriggerHandler)" -> "Algo1 (OrchestratorTriggerHandler)": TriggerEvent (contains adapted BaseData for example)
"Algo1 (OrchestratorTriggerHandler)" -> "Scheduler (OrchestratorTriggerHandler)": Response (contains customized Response)

"Scheduler (OrchestratorTriggerHandler)" -> "Algo2 (OrchestratorTriggerHandler)": TriggerEvent (contains adapted BaseData for example)
"Algo2 (OrchestratorTriggerHandler)" -> "Scheduler (OrchestratorTriggerHandler)": Response (contains customized Response)
end

"Scheduler (OrchestratorTriggerHandler)" -> "NEXT-Player (OrchestratorMaster)": Response containing status or error message
@enduml

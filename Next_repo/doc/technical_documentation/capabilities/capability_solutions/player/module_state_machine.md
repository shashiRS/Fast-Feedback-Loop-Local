State Machine{#player_module_state_machine}
==============

Module Overview Contents
* @ref player_module_file_reader
* @ref player_module_play_manager
* @ref player_module_publisher_manager
* @ref player_module_resource_manager

___

[TOC]

# Overview

This page explains the Next-Player PlayerStateMachine module.

Next-Player state machine is a mathematical model used to describe the behavior of the system.

___

# State Machine Architecture
Next-Player state machine consists of a set of states and transitions between them and it is used to describe the behavior of the Next-Player.

The behavior of the system is determined by a finite set of states and transitions where the system needs to respond to external stimuli in a predictable way.

@startuml State Machine
state New #LightBlue{

}

state Opening #LightYellow{

}

state UnloadRecording #LightYellow{

}

state Ready #LightBlue{

}

state SkipTo #LightYellow{

}

state Play #LightBlue{

}

state ErrorProcessing #LightYellow{

}

state ShuttingDown #LightYellow{

}
state onError  <<start>>
state onShutdown  <<start>>

[*] --> New
New --> Opening : OPEN
Opening --> New : [loading failed]
Opening --> Ready : [recording loaded]
UnloadRecording --> New : [recording is closed]
Ready --> UnloadRecording : CLOSE
Ready --> SkipTo : JUMP
Ready --> Play : STEPFORWARD
SkipTo --> Ready : [currentTimestamp == skipTimestamp]
SkipTo --> SkipTo : [currentTimestamp < skipTimestamp]
Play --> Ready : [stepsToPerform == 0]
Play --> Play : [stepsToPerform > 0]
Play --> Ready : PAUSE
Play --> Ready : EOF
onError --> ErrorProcessing
onShutdown --> ShuttingDown
ShuttingDown --> [*]
ErrorProcessing --> [*]

@enduml

We are using Boost.MSM (Meta State Machine), we need to define a set of states and events using C++ structs or classes.

Each state is represented by a struct or class that defines the behavior of the state and each event is represented by a struct or class that defines the event's name and any associated data.

After defining the states and events, the state machine itself is defined using a template class that takes the set of states and events as template arguments. This template class provides a set of member functions used to define the transitions between states in response to events.

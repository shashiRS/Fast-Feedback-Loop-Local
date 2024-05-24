Play Manager{#player_module_play_manager}
==============

Module Overview Contents
* @ref player_module_file_reader
* @ref player_module_state_machine
* @ref player_module_player_plugin
* @ref player_module_publisher_manager
* @ref player_module_resource_manager

___

[TOC]

# Overview

This page explains the Next-Player PlayManager module.
Main functionalities:
* executed received commands
* run command-line

___

# How the Next-Player receives commands?

-# GUI will send a json message via websocket to Next Bridges
-# the payload will be handled
-# the command will be publish via command handler from Next Control Bridge
-# command handler client will notify the command handler server from Next Player
-# the state of the Next Player will change
-# the event will be processed

@startuml
left to right direction
state GUI #LightBlue
state next_bridges #LightBlue{
    command_handler_client : publish command
}
state next_player #LightBlue{
    command_handler_server : request command
    state PlayManager #LightBlue{
        player_state_machine : process_event
    }
}

note left of command_handler_server
Next-Control event module used to receive message requests
end note

note right of command_handler_client
Next-Control event module used to publish message requests
end note

[*] --> GUI 
GUI --> next_bridges : Send json message
next_bridges --> command_handler_client #LightBlue : Handle json payload 
command_handler_server --> player_state_machine #LightBlue : Handle request
command_handler_client --> command_handler_server #LightBlue 
next_bridges --> GUI : Receive response 

@enduml

> 📝 Example of [Player Control events](https://github-am.geo.conti.de/pages/ADAS/Next-GUI/#/./interface/events/player_events/asyncapi):
> \parblock
\code{.json}
 {
  "channel": "player",
  "event": "PlaybackIsPlaying",
  "source": "NextBridge",
  "payload": {
    "startTimeStamp": 5828564174,
    "endTimeStamp": 6069067045,
    "currentTimestamp": 5828564180
  }
}
\endcode
\endparblock

___

# Command-line
Next-Player can be used from the command-line with arguments such as "-r {recording_path}". The parsing is performed using the CmdOptions and ConfigClient modules from the Next SDK.

The state machine will then process the following events: "OPEN(recording_path)," "STEPFORWARD(INT32_MAX)," and "CLOSE."

@startuml
left to right direction
state StateOpening #LightBlue
state StatePlay #LightBlue
state StateClose #LightBlue

state next_control #LightBlue{
    state next_sdk #LightBlue{
        state CmdOption
        state ConfigClient
    }
}

[*] --> CmdOption #LightBlue : arguments
CmdOption --> ConfigClient #LightBlue : parse arguments
ConfigClient --> StateOpening : get recording path
StateOpening --> StatePlay
StatePlay --> StateClose
@enduml

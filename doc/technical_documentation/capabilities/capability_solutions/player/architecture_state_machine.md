Player State Machine Architecture {#player_architecture_state_machine}
=============

[TOC]

# Player
The player provides the following functionalities:
 - open a recording
 - close a recording
 - play action
 - pause action
 - step action
 - jump action

This functionalities are controlled using a state machine implementation described by the diagram below:

State machine Diagram
@startuml
state onError  <<start>>
state onShutdown  <<start>>
[*] -> New
New--> Opening : command::OPEN
Opening--> New : Loading Failed
Opening--> Ready : Recording Loaded
Ready--> Unload_Recording : command::CLOSE
Unload_Recording--> New : recording is closed
Ready--> skipTo : command::JUMP(skipTimestamp)
skipTo--> Ready : [currentTimestamp==skipTimestamp]
Ready--> PLAY : command::STEP_FORWARD(steps)
PLAY-->Ready: command::PAUSE\n||\nstepsToPerform==0\n||\nEOF
onError-->ERROR : OnError
onShutdown -->SHUTDOWN : OnShutdown
ERROR-->[*]
SHUTDOWN-->[*]
@enduml

When started the player will enter the NEW state and will wait for the OPEN command. Once received it will enter the Opening state where the recording will be loaded using the FileReader class and move to the Ready state if it was loaded successfully or back to New.
Receiving a STEP_FORWARD command will move the state machine to the Play state where using the PublisherManager class, data packages from the recording will be published.
Receiving a JUMP command will move the state machine to the skipTo state where it will use the FileReader class to select the requested position in the recording.

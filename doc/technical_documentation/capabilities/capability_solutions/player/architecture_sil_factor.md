Sil Factor Architecture {#player_architecture_sil_factor}
=============

[TOC]
# Sil Factor
The Sil Factor is used to control the playback speed of the player. It can be read from the player configuration file or it can be set from the GUI.
The GUI will send the updated Sil factor to the player using the Next ControlBridge. The interraction is desctibed in the diagram below:

@startuml

participant Player
participant ControlBridge
participant Webserver
participant GUI


' ############################################
== Change Sil factor in GUI ==
' ############################################
Player -> Player ++ #Yellow: Start
Player -> Player: Read Configuration File
Player -> Player: Create hook to receive changes in the Sil factor in ConfigClient
Player -> Player: Read Sil factor from ConfigClient
Player -> Player: Set Sil factor


GUI -> GUI ++ #Maroon: Change Sil factor
GUI -> Webserver: Send Sil factor
Webserver -> ControlBridge: Receive Sil factor
ControlBridge -> ControlBridge:  Try writing Sil factor in ConfigClient

Player -> Player: Hook with new Sil factor is called
Player -> Player: Set Sil factor

Player -> Player --:  Shutdown player

Player -> Player ++ #Yellow: Start
Player -> Player: Read Configuration File
Player -> Player: Read Sil factor from ConfigClient
Player -> Player: Set Sil factor

Player -[#MediumSpringGreen]> ControlBridge ++ #MediumSpringGreen: Send STATE_NEW event
ControlBridge -[#MediumSpringGreen]> ControlBridge: Try reading Sil factor from ConfigClient
ControlBridge -[#MediumSpringGreen]> Webserver --: Send Sil factor
Webserver -> GUI: Forward Sil factor


@enduml
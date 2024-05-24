ControlBridge {#module_controlbridge}
==============

[TOC]

The \named{ControlBridge} is handling the control between nodes and the \named{GUI}.
The \named{ControlBridge} is using a websocket for the communication with the \named{GUI}. The event system of \named{NEXT SDK} is used for the communication with nodes.

## GUI <-> next_player commands 

The \named{ControlBridge} executable is starting a websocket for the communication with the \named{GUI} and is using an instance of the *PlayerControlManager* class for controlling \named{NEXT Player}.

The *PlayerControlManager* receives commands for controlling \named{NEXT Player} in json format from the \named{GUI} via websocket. After decoding of the json message a *PlayerCommandEvent* is published. The \named{NEXT Player} has subscribed to *PlayerCommandEvent* and acts depending of the command in this event.
In addition a response message is sent back to the \named{GUI}. 
The *PlayerEventDistributor* instance is used for collecting the *PlayerEventStates* from the \named{NEXT Player}, like "ON_PLAY", and sending it as json message with the help of the *PlayerControlManager* and websocket to the \named{GUI}.

The following graphic shows the communication between the participants.


@startuml

participant Player
participant PlayerEventDistributor
participant PlayerControlManager
participant ControlBridge
participant websocket
participant GUI


' ############################################
== Startup websocket ==
' ############################################

ControlBridge -> websocket: Start websocket

websocket -[#red]> websocket ++ #red: Wait for connections

ControlBridge -> websocket: Activate listing at sockets
websocket -[#SlateBlue]> websocket ++ #SlateBlue: Wait for incoming messages

' ############################################
== Startup PlayerControlManager ==
' ############################################

ControlBridge -> PlayerControlManager: startUp

PlayerControlManager -> PlayerEventDistributor: startUp
PlayerEventDistributor -> Player: Subscribe to PlayerStateEvent
activate PlayerEventDistributor

Player -> PlayerControlManager: Subscribe to PlayerCommandEvent
activate Player

PlayerControlManager -> websocket: Add PayloadCallback for incoming json messages 
PlayerControlManager -[#salmon]> PlayerControlManager ++ #salmon: Start thread to process queue of\nincoming json messages
PlayerControlManager -[#DeepSkyBlue]> PlayerControlManager ++ #DeepSkyBlue: Start thread to process queue of\n outgoing json messages

' ############################################
== websocket connection ==
' ############################################

ControlBridge -[#MediumSeaGreen]> ControlBridge ++ #MediumSeaGreen: Ask websocket for new connections\nto send out version info

GUI -[#red]> websocket: Connect to websocket
ControlBridge -[#MediumSeaGreen]> websocket: Get number of websocket connections
websocket -[#MediumSeaGreen]> ControlBridge
ControlBridge -[#MediumSeaGreen]> websocket: Send Controlbridge version info
websocket -> GUI: Forward Controlbridge version info

' ############################################
== Player control command ==
' ############################################

group player control command workflow
GUI -[#SlateBlue]> websocket: Send Player control command

websocket -[#salmon]> PlayerControlManager: Call PayloadCallback with incoming json message

PlayerControlManager -[#salmon]> PlayerControlManager:Add to queue of incoming json messages

hnote over PlayerControlManager
Process message of incoming queue
-> Decode json message
endhnote

PlayerControlManager -[#Maroon]> Player ++ #Maroon: Publish PlayerCommandEvent

hnote over Player
Execute PlayerCommandEvent
e.g. Load or playback recording
endhnote

Player -[#Maroon]> PlayerControlManager -- #Maroon: Finish subscription callback

hnote over PlayerControlManager
Create response message to incoming
Player control command message
endhnote

PlayerControlManager -[#DeepSkyBlue]> PlayerControlManager: Add to queue of outgoing json messages

PlayerControlManager -[#DeepSkyBlue]> websocket: Send message of outgoing queue
websocket -> GUI: Send json message

end

' ############################################
== Player status update ==
' ############################################

group player status update workflow

Player -> PlayerEventDistributor: Publish PlayerStateEvent

hnote over PlayerEventDistributor
Analysis PlayerStateEvent
endhnote

PlayerEventDistributor -> PlayerControlManager: Add PlayerStateEvent message\nto outgoing json message queue
PlayerControlManager -[#DeepSkyBlue]> websocket: Send message of outgoing queue
websocket -> GUI: Send json message

end


@enduml

## LifeCycleOrchestrator

The LifeCycleOrchestrator is responsible of the lifecycle management. It analysis the *PlayerStateEvent* and publish a *LifecycleTrigger* event, e.g. to inform other nodes about a reset of the \named{NEXT Player}.

@startuml

participant Player
participant LifeCycleOrchestrator
participant ControlBridge

ControlBridge -> LifeCycleOrchestrator: startUp

LifeCycleOrchestrator -> Player: Subscribe for PlayerStateEvent
activate LifeCycleOrchestrator

Player -> LifeCycleOrchestrator: Publish PlayerStateEvent 

LifeCycleOrchestrator -> LifeCycleOrchestrator: Check PlayerStateEvent

hnote over LifeCycleOrchestrator
Publish a  Lifecycle Trigger Reset Event 
in case the player has loaded a new recording.
endhnote


@enduml

## Version forwarding

\named{ControlBridge} collects version informations from running nodes, e.g. \named{DataBridge}, and provides them to the \named{GUI}.

@startuml

participant Player
participant DataBridge
participant ControlBridge
participant VersionEventDistributor
participant VersionInfo_Event
participant websocket
participant GUI

hnote over Player
startUp
endhnote

Player -> VersionInfo_Event: subscribe

hnote over ControlBridge
startUp
endhnote

ControlBridge -> websocket: startUp
activate websocket

ControlBridge -> VersionEventDistributor: startUp
VersionEventDistributor -> VersionInfo_Event: Subscribe 

group UseCase1: Version info from already running nodes


group get version infos from already running nodes
VersionEventDistributor -> VersionInfo_Event: requestEventUpdate()
VersionInfo_Event -> Player: Trigger callback to send version info
Player -> VersionInfo_Event: publish version info
VersionEventDistributor <- VersionInfo_Event: Call callback with version infos from nodes
end

hnote over VersionEventDistributor
Create json message with version 
infos from nodes
endhnote

VersionEventDistributor -> websocket: Send json message with version info
note right: If no client is not registered,\nmessage isn'treceived by anybody.

end

ControlBridge -> ControlBridge: Check cyclic if websocket has new connections
activate ControlBridge

activate GUI

GUI -> websocket: Connect to websocket

group UseCase2: Send Version info for new websocket connections

ControlBridge -> websocket: Request number of websocket connections
websocket -> ControlBridge

hnote over ControlBridge
Check if number of websocket
connections increased
endhnote

ControlBridge -> VersionEventDistributor: Trigger sending version info of nodes
group get version infos from already running nodes
VersionEventDistributor -> VersionInfo_Event: requestEventUpdate() 
note left : Hint: Version infos from the nodes are\nnot stored in this class. That's why\nit needs to be requested again.
VersionInfo_Event -> Player: Trigger callback to send version info
Player -> VersionInfo_Event: publish version info
VersionEventDistributor <- VersionInfo_Event: Call callback with version infos from nodes
end

hnote over VersionEventDistributor
Create json message with version 
infos from nodes
endhnote

VersionEventDistributor -> websocket: Send version info of nodes
websocket -> GUI: Send version info of nodes

end

hnote over DataBridge
startUp
endhnote

DataBridge -> VersionInfo_Event: subscribe

group UseCase3: Send Version info of late started node

DataBridge -> VersionInfo_Event: publish version info
VersionEventDistributor <- VersionInfo_Event: Call callback with version infos from nodes

hnote over VersionEventDistributor
Create json message with version 
infos from node
endhnote

VersionEventDistributor -> websocket: Send version info of node
websocket -> GUI: Send version info of node

end

@enduml

## Sil Factor Update

\named{ControlBridge} handles the passing of the Sil Factor value between \named{Next Player} and \named{GUI}. It uses the \named{CnfigServer} to send and request the Sil factor values to the Next Player component.

The possible usecases for setting the SIl factor are described in the diabrams below:

@startuml

participant Player
participant ControlBridge
participant Webserver
participant GUI

' ############################################
== UC1: Start ControlBridge before GUI ==
' ############################################

ControlBridge -[#MediumSpringGreen]> ControlBridge: Add hook for STATE_NEW from player

ControlBridge -> Webserver: Start Webserver

Webserver -[#red]> Webserver ++ #red: Wait for connections

ControlBridge -> Webserver: Activate listing at sockets
Webserver -[#SlateBlue]> Webserver ++ #SlateBlue: Wait for incoming messages

Player -> Player ++ #Yellow: Start
Player -> Player: Read Sil Configuration File
Player -> Player: Read Sil factor from ConfigClient
Player -> Player: Set Sil factor

Player -[#MediumSpringGreen]> ControlBridge ++ #MediumSpringGreen: Send STATE_NEW event
ControlBridge -[#MediumSpringGreen]> ControlBridge: Try reading Sil factor from ConfigClient
ControlBridge -[#MediumSpringGreen]> Webserver --: Send Sil factor
Webserver -> Webserver: No connection available

' ############################################
== Send Sil factor ==
' ############################################

Player -[#MediumSeaGreen]> ControlBridge ++ #MediumSeaGreen:   Send STATE event

GUI -> GUI ++ #Maroon: Start
GUI -[#red]> Webserver: Connect to WebServer
ControlBridge -[#MediumSeaGreen]> Webserver: Get number of webserver connections
Webserver -[#MediumSeaGreen]> ControlBridge
ControlBridge -[#MediumSeaGreen]> ControlBridge: Try reading Sil factor from ConfigClient
ControlBridge -[#MediumSeaGreen]> Webserver: Send Sil factor
Webserver -> GUI: Forward Sil factor
@enduml

@startuml

participant Player
participant ControlBridge
participant Webserver
participant GUI

' ############################################
== UC2: Start GUI before Controlbridge==
' ############################################

GUI -> GUI ++ #Maroon: Start

ControlBridge -> Webserver: Start Webserver

Webserver -[#red]> Webserver ++ #red: Wait for connections

ControlBridge -> Webserver: Activate listing at sockets
Webserver -[#SlateBlue]> Webserver ++ #SlateBlue: Wait for incoming messages

' ############################################
== Send Sil factor ==
' ############################################

Player -> Player ++ #Yellow: Start
Player -> Player: Read Sil factor from ConfigClient
Player -> Player: Set Sil factor

Player -[#MediumSpringGreen]> ControlBridge ++ #MediumSpringGreen: Send STATE_NEW event
ControlBridge -[#MediumSpringGreen]> ControlBridge: Try reading Sil factor from ConfigClient
ControlBridge -[#MediumSpringGreen]> Webserver --: Send Sil factor
Webserver -> GUI: Forward Sil factor
@enduml

@startuml

participant Player
participant ControlBridge
participant Webserver
participant GUI

' ############################################
== UC3: Change Sil factor in GUI ==
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

@startuml

participant Player
participant ControlBridge
participant Webserver
participant GUI

' ############################################
== UC4: Change Sil factor in GUI before start of player ==
' ############################################

GUI -> GUI ++ #Maroon: Change Sil factor
GUI -> Webserver: Send Sil factor
Webserver -> ControlBridge: Receive Sil factor
ControlBridge -> ControlBridge: Try writing Sil factor in ConfigClient

Player -> Player ++ #Yellow: Start
Player -> Player: Read Configuration File
Player -> Player: Read Sil factor from ConfigClient
Player -> Player: Set Sil factor

Player -[#MediumSpringGreen]> ControlBridge ++ #MediumSpringGreen: Send STATE_NEW event
ControlBridge -[#MediumSpringGreen]> ControlBridge: Try reading Sil factor from ConfigClient
ControlBridge -[#MediumSpringGreen]> Webserver --: Send Sil factor
Webserver -> GUI: Forward Sil factor
@enduml

# Logging Architecture
Next Logging Architecture is divided into three modules

## LogRouter
LogRouter is the class which is an extension of spd::sink interface .
LogRouter encapsulates the LogEvent which is an event extension from BaseBinaryEvent.
LogRouter receives the logs from the SPDLog. The logs are added to a LogEvent and published as Event to the next framework.

## LogCollector
LogCollector is a class which is used inside the Controlbridge to collect the LogEvents. It subscribes to the "LogEvent"-Event which are published by the LogRouter in the nodes to receive the log messages. It extracts the logs and finally route them to the LogDistributor.

## LogDistributor
LogDistributor is a class used in the Controlbridge to processes the logs collected by the LogCollector and send the data using WebServer to the parties who are clients to the WebServer.
@startuml

struct LogEventData {
+ timestamp
+ thread_id
+ component_name
+ channel_name
+ log_level
+ log_message
}

note left of LogEventData::channel_name
more than one channel possible per component
end note

note left of LogEventData::log_level
log level: info, warning, debug, error, exception
end note

class BaseEventBinary <! LogEventData > {
}

class LogEvent {
+ void publishLogEvent(const std::vector<LogEventData> &)
# serializeEventPayload(...)
# deserializeEventPayload(...)
}

note left of LogEvent::publishLogEvent
  - calls publish(std::vector<LogEventData> &) to publish LogEventData using ECal
end note

LogEventData -down-o BaseEventBinary
BaseEventBinary -down-* LogEvent

class sink

class LogRouter {
# void publishLogEvent()
+ void log(const spdlog::details::log_msg &msg)
+ flush()
- LogEvent LogEvent_
}

note right of LogRouter::publishLogEvent
  - its a reader thread which reads the common buffer (vector<LogEventData>)
  - publishes the LogEventData using LogEvent
end note

note right of LogRouter::log
  - its a writer thread which writes into the common buffer(vector<LogEventData>)
end note

LogEvent -down- LogRouter::LogEvent

class ConsoleSink

class FileSink

sink -down-|> LogRouter
sink -down-|> ConsoleSink
sink -down-|> FileSink



object SPDLog {
init()
LogRouter LogRouter_
ConsoleSink ConsoleSink_
FileSink FileSink_
}

note left of SPDLog::init
  - get sink configuration from ConfigFile via ConfigurationManager
  - create and configure sinks in nextsdk_logger based on ConfigFile
  - configured to flush all log message above a certain log level immediately
end note


LogRouter -down-> SPDLog::LogRouter
ConsoleSink -down-> SPDLog::ConsoleSink
FileSink -down-> SPDLog::FileSink

class ConfigurationManager
note bottom of ConfigurationManager
User can configure logging via ConfigFile
- sinks to be used for logging
  - filtering log message per sink based on logLevel
end note

object ComponentA {
 configureSPDLog()
}
note bottom: calls logger, e.g by command Info()

object ComponentB {
 configureSPDLog()
}

class ControlBridge {
 configureSPDLog()
 LogDistributor LogDistributor_
 LogCollector LogCollector_
}

note left of ControlBridge::LogCollector
- it gets hook to LogDistributor::SendLogEventData
end note

hide ControlBridge circle

SPDLog -down-> ComponentA
SPDLog -down-> ComponentB
SPDLog -down-> ControlBridge

ConfigurationManager -left-> SPDLog


object GUI
note bottom of GUI
  - receives collection of log messages and insert them to existing log based on timestamp
end note

ControlBridge -down-> GUI

class LogCollector {
 +Setup()
 +ShutDown()
 +AddLogEventDataProcessingHook(...)
}

class LogDistributor {
+void SetWebserver(...)
+void SendLogEventData(...)
+void SetWaitTime(...)
}

note left of LogDistributor::SetWebserver
- keeps the pointer to the webserver, and uses it to send data to gui
end note
note left of LogDistributor::SendLogEventData
  - this function is used as hook to the LogCollector in order to receive the data
  - and provides data to send it out to gui using webserver
end note
note left of LogDistributor::SetWaitTime
  - sets the timer interval in milliseconds to periodically send data to gui
end note

LogDistributor -down- ControlBridge::LogDistributor
LogCollector -down- ControlBridge::LogCollector


@enduml

# Logging Flow Chart
@startuml
 
partition "Controlbridge LogDistributing"{
  "LogCollector receives messages via subscription" --> "LogDistributor receives message from LogCollector"
  "LogDistributor receives message from LogCollector" --> "LogDistributor sends data to GUI"
}
 
partition GUI {
  "LogDistributor sends data to GUI" --> "visualize log message"
}
 
 
partition "Component A"{
  "logMessage in A" --> "SPDLogger Component A:\n- route message to activated sinks"
  "SPDLogger Component A:\n- route message to activated sinks" --> "Console Sink: \n- Log message to console"
  "SPDLogger Component A:\n- route message to activated sinks" --> "File Sink: \n- Write message to file"
  "SPDLogger Component A:\n- route message to activated sinks" --> "Log Router Component A: \n- publish message"
  "Log Router Component A: \n- publish message" --> "LogCollector receives messages via subscription"
}
 
partition "Controlbridge"{
  "logMessage in Controlbridge" --> "SPDLogger Controlbridge:\n- route message to activated sinks"
   "SPDLogger Controlbridge:\n- route message to activated sinks" --> "Log Router Controlbridge: \n- publish message"
  "Log Router Controlbridge: \n- publish message" --> "LogCollector receives messages via subscription"
}
 
 
@enduml

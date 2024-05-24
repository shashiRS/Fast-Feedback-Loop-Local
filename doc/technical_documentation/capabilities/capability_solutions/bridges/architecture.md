Architecture {#bridges_architecture}
==============

[TOC]

# DataBridge
The DataBridge is managing the dataflow between nodes and the GUI.

@startuml uml_test

card BackendPC as "Developer PC / HPC <:computer:>" {

  rectangle NextSimSDK as "Next Sim SDK" {
    rectangle SimNodes as "Simulation Nodes"
    rectangle RunAlgos as "Running Algorithms"
    rectangle InsSigForm as "Insert Signal Formats"
    rectangle Algo1 as "Algo 1"
    rectangle Algo2 as "Algo 2"
  }

  storage NextUDex as "Next UDex" {
    rectangle "Signal Formatting"
    rectangle "Network and Data Management"
    cloud "Simulation data (eCAL)"
  }

  database MTSCore as "MTS Core"

  rectangle NextPlayer as "Next Player" {
    rectangle ReadRec as "Read Recordings"
    rectangle PubData as "Publish Data"
    rectangle InsSigFormat as "Insert Signal Format"
  }

  rectangle NextControl as "Next Control" {
    label "Data Orchestration and Scheduling"
  }

  rectangle NextBridges as "Next Bridges" {
    rectangle ControlBridge as "Control Bridge" {
      rectangle control
    }
    rectangle DataBridge as "Data Bridge" {
      rectangle TapSim as "Tap into Simulation"
      rectangle ForwardGUI as "Forward Data to GUI"
      rectangle ProvPlugin as "Provide Plugin framework as entry"
      rectangle Plugin1 as "Plugin 1"
      rectangle Plugin2 as "Plugin 2"    
    }
  }
}

card FrontendPC as "Developer PC / Cloud <:cloud:>" {
  card NextGUI as "Next GUI" {
    rectangle UserInterface as "User Interface"
    rectangle RemoteLocal as "Remote and Local"

    rectangle ControlEndpoint as "Control Endpoint"
    rectangle DataEndpoint as "Data Endpoint"
    rectangle Widget1
    rectangle Widget2
  }
}

label ProjectSpecific as "Project Specific"

MTSCore <-> NextPlayer

Algo1 -> Plugin1 : ProjectSpecific
Algo2 -> Plugin2 : ProjectSpecific
Plugin1 -> Widget1 : ProjectSpecific
Plugin1 -> Widget2 : ProjectSpecific
Plugin2 -> Widget2 : ProjectSpecific

NextUDex <-> NextSimSDK
NextUDex <-> NextBridges
NextUDex <-> NextPlayer

NextBridges -> NextGUI : websocket
@enduml

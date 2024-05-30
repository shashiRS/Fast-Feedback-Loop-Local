DataBridge {#module_databridge}
==============

[TOC]

# Overview {#databridge_overview}

The \named{DataBridge} provides a Plugin SDK that allows developers to test and deploy project-specific plugins during runtime.
<br>
Plugins are shared library packages that can be loaded dynamically during runtime. 
Plugins are able to communicate with \named{NEXT UDex} for data exchange and provide data for the \named{GUI} via websocket. 
Plugins are handled by the PluginManager in the \named{DataBridge} and can be started from the \named{GUI}.
<br>
Details about the plugins and its creation can be found in [plugins](@ref module_plugins).
<br>
An overview of the \named{NEXT} simulation framework is depicted in the following picture. 

<img src="howto_overview_next_framework.PNG" width="640">

# Plugin configuration {#plugin_config}

The following graphic shows simplified the start up of the \named{DataBridge}, focussed on configuring the plugins.
<br>
It consistes of the following participants:
- The \named{DataBridge} executables is setting up the *PluginManager*, *DataClassManager* and *websocket*. It requests the events/tasks from the queues in the *DataClassManager* and forwards it for processing, e.g. to the *PluginManager*.
- The *PluginManager* instance takes care of setting up the plugins.
- The *ProcessManager* instance, created by the *PluginManager*, takes care of parallel execution of different processes while making sure that calls of the same process are running sequential. This functionality is used e.g. for adding plugin configurations and prevents that different plugin configurations of the same plugin are added in parallel.
- The *DataClassManager* contains queues with events/tasks, received e.g. from the \named{GUI}.
- The *DataRequestServer* instance, created by the *DataClassManager*, receives json messages via *websocket*, e.g. from the \named{GUI}. These messages are decoded with the help of the *EventHandlerHelper* class and added to the respective queues of the *DataClassManager*.


The following diagram describes it more in detail:

@startuml

participant DataBridge
participant PluginManager
participant ProcessManager
participant DataClassManager
participant DataRequestServer
participant websocket
participant GUI

' ############################################
== Initialization ==
' ############################################

DataBridge -> websocket: startUp
DataBridge -> DataClassManager: startUp
DataClassManager -> DataRequestServer: startUp
DataRequestServer -> websocket: Register callback to add incoming json message to request queue
DataRequestServer -[#SlateBlue]> DataRequestServer ++ #SlateBlue: start thread to decode incoming json message in request queue

' ############################################
== Plugin setup ==
' ############################################

DataBridge -> PluginManager: Create instance
DataBridge -> PluginManager ++: init()

PluginManager -[#DeepSkyBlue]> PluginManager ++ #DeepSkyBlue: scan directories for plugins

hnote over PluginManager
Store path to folder of plugin dll
Get and store plugin constructor
Get and store plugin output data interface descripton (flatbuffer schema)
endhnote

PluginManager -[#DeepSkyBlue]> PluginManager -- #DeepSkyBlue: finish scan

hnote over PluginManager
Set up eCAL backend
Subscribe to PlayerStateEvent for plugin reset
endhnote

PluginManager -[#salmon]> ProcessManager ++ #salmon: startUp
ProcessManager -[#salmon]> ProcessManager: handleDeferredCalls
deactivate PluginManager

DataBridge -> PluginManager ++: Start: Configure default plugins\n(specified as command-line arguments)

hnote over PluginManager
Setting up default plugins
endhnote

PluginManager -> DataBridge --: Finish: Configuring plugins

DataBridge -> DataBridge ++: Start loop to process config requests, data requests\nand backend requests from DataClassManager

' ############################################
== Start GUI DataBridge Connection==
' ############################################
group Setting up plugins which provide flatbuffer schema "signals" to GUI
GUI -> websocket: set data request for flatbuffer schema "signals"
websocket -> DataRequestServer: Call callback for handling incoming json message
DataRequestServer -[#SlateBlue]> DataRequestServer: Decode incoming json message by EventHandlerHelper
DataRequestServer -[#SlateBlue]> DataClassManager: Add decoded event to data requests queue

DataBridge -> DataClassManager: Request oldest items from data requests queue
DataClassManager -> DataBridge

DataBridge -> PluginManager: Trigger startup compatible plugins to signals flatbuffer schema

hnote over PluginManager
Setting up compatible plugins
endhnote

end

group Setting up backend

GUI -> websocket: set backend configuration
websocket -> DataRequestServer: Call callback for handling incoming json message
DataRequestServer -[#SlateBlue]> DataRequestServer: Decode incoming json message by EventHandlerHelper
DataRequestServer -[#SlateBlue]> DataClassManager: Add decoded event to backend queue

DataBridge -> DataClassManager: Request oldest items from backend queue
DataClassManager -> DataBridge

DataBridge -> PluginManager: Trigger SetSynchronizationMode for the plugins

hnote over PluginManager
SetSynchronizationMode for all running plugins
endhnote

end

' ############################################
== Get Plugin description for 3D View==
' ############################################

group get plugin description

GUI -> websocket: Request plugin descriptions for supported FBS schema files
websocket -> DataRequestServer: Call callback for handling incoming json message
DataRequestServer -[#SlateBlue]> DataRequestServer: Decode incoming json message by EventHandlerHelper
DataRequestServer -[#SlateBlue]> DataClassManager: Add to config request queue

DataBridge -> DataClassManager: Request oldest item from config request queue
DataClassManager -> DataBridge

DataBridge -> PluginManager: Trigger sending plugin descriptions of compatible plugins to FBS schema file

hnote over PluginManager
Get compatible plugins
Create instance and init compatible plugins which are not already running
endhnote

PluginManager -> PluginManager: Get plugin descriptions

PluginManager -> websocket: Send plugin descriptions

websocket -> GUI: Send plugin descriptions

end


' ############################################
== Add Plugin configuration==
' ############################################

group add plugin configuration

GUI -> websocket: Send json message with the selected plugin configurations of all plugins
websocket -> DataRequestServer: Call callback for handling incoming json message
DataRequestServer -[#SlateBlue]> DataRequestServer: Decode incoming json messages by EventHandlerHelper
DataRequestServer -[#SlateBlue]> DataClassManager: Add each plugin config to config request queue

DataBridge -> DataClassManager: Request oldest item from config request queue
DataClassManager -> DataBridge

DataBridge -> PluginManager: Trigger setting plugin configuration

hnote over PluginManager
Call plugin constructor and init function of selected plugin configurations,
if not already running
endhnote

PluginManager -[#salmon]> ProcessManager: Add plugin setConfig() call to processing queue
note right: setConfig checks for changed plugin configurations.\nRemove non existing configurations and add new ones.

ProcessManager -[#salmon]> ProcessManager: Process setConfig() calls
note right: setConfig() of different plugins can be processed in parallel, \nbut form the same plugin only sequential

end

@enduml

# Data exchange between DataBridge and GUI {#flatbuffer}

The data exchange between \named{DataBridge} and \named{GUI} happens through JSON-format messages. The contained data are provided by [Flatbuffers](https://google.github.io/flatbuffers/), converted to JSON-format. More details regaring using the flatbuffers in the plugins can be found [here](@ref plugin_flatbuffer_structure)
<br>
An overview of the data exchange is depicted in the following diagram.

@startuml

participant Next_UDex
participant PluginA
participant PluginDataPublisher
participant websocket
participant GUI

' ############################################
== Initialization ==
' ############################################

PluginA -> Next_UDex: Subscribe callback to a topic

' ############################################
== Data exchange ==
' ############################################

[->Next_UDex: New data are published for a topic

Next_UDex -> PluginA ++: Call calback with new data for this topic

PluginA -> PluginA: Get values from topic data

PluginA -> PluginA: Create Flatbuffer Message with topic data

PluginA -> PluginDataPublisher: Call sendFlatbufferData()

hnote over PluginDataPublisher
Convert flatbuffer message to json string
Create wrapper around json string containing the flatbuffer message
endhnote

PluginDataPublisher -> websocket: Send json message with flatbuffer message

websocket -> GUI: Send json message with flatbuffer message

GUI -> GUI: Process incoming json message, e.g. visualize data in 3D View

@enduml


# Example dataflow between DataBridge, plugins and GUI

The following shows on a practical example the dataflow between the plugins and the \named{GUI}.
<br>
The initialization and start of a plugin is requested via the \named{GUI}.
This happens through the configuration setting of the Widgets.

## Step 1 - Next Gui {#gui_plugin_config}

Each widget configuration has a field for modifying the data stream configuration. The widget configuration can be opened by the **Configuration button**. 

<img src="howto_3dview_gear.png" width="639">

By clicking the search icon next to it the configuration page opens. 

<img src="howto_3dview_glass.png" width="639">

Clicking on the **Refresh button** next to **Available Configurations** requests widget specific data classes from the \named{DataBridge} plugins. The tags of the data classes consist of a name and a version number, e.g. "GroundLine", "0.0.1".

<img src="howto_3dview_config_update.png" width="639">

## Step 2 - DataBridge {#databridge_plugin_init}

The \named{DataBridge} takes this data class request and compares it with every \*.fbs file within the plugin folders that where registered during startup of the bridge. If the plugin data class matches the requested tag, the [init()](@ref plugin_function_init) function followed by the [getDescription()](@ref plugin_function_getDescription) function of the plugin will be called by the \named{DataBridge}. Afterwards the \named{DataBridge} will send the plugin name as well as the description, which consists of all defined configuration options for the plugin, to the widget.


## Step 3 - Next Gui {#gui_plugin_add}


All plugin configuration options, provided by the \named{DataBridge}, will be listed below **Available Configurations** in the Data Class explorer of the widget. By adding a plugin configuration the default properties of the plugin configuration will be shown on the right side at **Selected Configurations** to configure it for the desired purpose.

<img src="howto_widgets_3dview_select_config.png" width="639">

Once the Configuration is set and saved by pressing the **Confirm Button**, the \named{GUI} will send all plugin configurations from all plugins with the plugin name back to the \named{DataBridge}. 


## Step 4 - Data Bridge {#databridge_plugin_add}

The \named{DataBridge} compares the previously plugin configuration against the current configuration. A plugin configuration is different if at least one plugin configuration parameter changed. Non existing plugin configurations are removed by calling [removeConfig()](@ref plugin_function_removeConfig). New plugin configurations are added by the [addConfig()](@ref plugin_function_addConfig) function call. The [addConfig()](@ref plugin_function_addConfig) function can subscribe function calls for topics at \named{NEXT UDex} to receive data from the \named{NEXT} backend. The received data by these function calls can be sent to the GUI via websocket with the [sendFlatbufferData](@ref plugin_send_flatbuffer) function.

## Step 5 - Next Gui {#gui_plugin_data_receive}

The GUI analysis the received data from the \named{DataBridge} and forwards it to the widget(s) which are using the data.

---



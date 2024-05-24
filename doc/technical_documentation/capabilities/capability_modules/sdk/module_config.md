Configuration Management {#sdk_module_config}
==============

Module Overview Contents
* @ref sdk_module_main
* @ref sdk_module_lifecycle
* @ref sdk_module_event_system
* @ref sdk_module_initchain
* @ref sdk_module_ecalsingleton
* @ref sdk_module_tracy
___

# Overview

The configuration management is split up into three parts:
* Configuration clients for basic property management, writing and reading local configuration
* Configuration clients to receive and send config information used by the component to interact with the global configuration state
* Configuration server to manage the global config state, propagate local changes and write and read in global configurations

The configuration system is used for three different use cases (MODES):
* Distributing static information about components (e.g. version)
* Setting up configuration requests for components to be configured (e.g. output port configuration)
* Providing the current dynamic information about components to the whole framework (e.g. recording filename)

Basic concepts:
* A local configuration can be loaded (and will overwrite any possible global configurations - at startup).
* Any data needed by the control flow of a component should be storable and retrievable in the configuration handler.
* Any data of a component is accessible (read only) by any other component.
* All configurations of all component can be written into a single configuration storage.
* Loading partial configuration storages is possible. All configurations will be loaded, filtering every irrelevant data.
* The configuration is structured as a tree - so every level of the tree has it's own responsibility.

Concept:

<img src="sdk_config_overview.png" width="800">


# Requirements / Use Case

## General use cases

- As a developer, I need to ...
  - change port connections on the fly
  - be able to override configurations for single nodes at startup
- As an enabler, I need to ...
  - store my configuration in a single file
  - adapt the configuration (composition, timings, etc) through the GUI
- As a Next developer I need to ...
  - analyse all configurations in a central place
  - create default configuration as templates
  - store transient values between threads inside an executable (node)
  
## Technical implications

The configuration manager should be usable to solve three different needs in the nodes / the framework

1. Every node has some parameters, that are needed to control the way, it works. 
   This goes from the version and the node name, the log level, etc. to the specific information of the node as the rrec filename of the player node or the list of inports including the configuration of the algo nodes.
   All of these parameters should be stored and retrieved centrally in the configuration manager.
2. Every node needs some configuration at startup to work in the proper manner for this specific run.
   This information is normally stored in a configuration file and is read at startup from there. Another way of giving these parameters is the command-line options.
   The configuration manager should collect and filter the given information, to be able to provide the information to the program.
   At any given time it should also be possible to dump the actual state of the configuration, to recreate the configuration by reading the dump in another run.
   Additionally, it should be possible to adapt the configuration in this dump to adapt the next run of a node to the user needs. By that, the user can use any dump as a template to configure the node.
3. As NEXT is a highly distributed system, the configuration should also provide a possibility to distribute the configuration information between nodes.
   This is needed to show the state of a node to the user and to access information between nodes (e.g. rrec name from player accessible in an algo node).
   Also it should be possible to aggregate the different configurations and store them in a single dump. 

# Dependencies

As the configuration handling is a central storage point for all kind of parameters used by the different components in a node, it is used by many components.
Also the configuration handling should be optional for most of the components and the components have to be optional for the configuration.

@startuml

[ConfigClient] -down-> [Boost] : use
[ConfigClient] ..> [events] : use

[logger] ..> [ConfigClient] : get parameters
[version_manager] ..> [ConfigClient] : set version
[profiler] ..> [ConfigClient] : get parameter
[lifecycle] ..> [ConfigClient] : set state
[cmd_options] -down-> [ConfigClient] :set parameter

[ConfigServer] -down-> [Boost] : use
[ConfigServer] -down-> [events] : use

@enduml

This implicates, that the config handling has to be started before most other components. 
And it implicates, that config handling will not be able to use functionalties from other compononents during startup (lifecycle / logger).

# Generic flow

@startuml

== Initialization ==

Process -> ConfigClient : block update to ConfigServer
Process -> ConfigClient : set default data
note left: including static data that is not overwirtten
ConfigClient -> ConfigServer : request data
note right: blocking with timeout (e.g. if no server available)
ConfigServer -> ConfigClient : set global config data
Process -> ConfigClient : set data from config files
note right: filtering of unrelated and static data based on convention
Process -> ConfigClient : unblock update to ConfigServer

== Pushing local changes  ==
Process -> ConfigClient : set data
note left: this can happen everywhere in the executable
ConfigClient -> ConfigServer : publish change
note right: ConfigServer shadows local config

Process -> ConfigClient : request data
ConfigClient -> Process : deliver data

== Receiving global changes ==
ConfigServer -> ConfigClient : deliver new data
ConfigClient -> Process : call reconfig callback

== get data from other component ==
Process -> ConfigClient : request data
ConfigClient -> ConfigServer : request data
ConfigServer -> ConfigClient : deliver data
ConfigClient -> Process : deliver data

@enduml

# Loading sequence of configuration values

The ConfigClient is in the initialization step from the call of next::appsupport::ConfigClient::do_init untill next::appsupport::ConfigClient::finish_init function.
In the initialization step the automatic server-client communication is turned of.
At this point any data loaded from a configuration file or from the config server will be filtered by the ConfigClient using the component name (the name that was provided in the do_init function).

The following shows the initialization steps of the ConfigClient:

@startuml
skinparam defaultTextAlignment center
[<b>First set static hardcoded values</b>\n(calls of put() function of ConfigCompData instance)] -down-> [<b>Load the default config</b>\n(calls of putCfg() function of ConfigDatabaseIO instance)]
[<b>Load the default config</b>\n(calls of putCfg() function of ConfigDatabaseIO instance)] -down->[<b>Load config from config server</b>\n(if available - uses a timeout)]
[<b>Load config from config server</b>\n(if available - uses a timeout)] -down->[<b>Load data from config files added by cmd-parameter</b>\n(if one was provided)]
[<b>Load data from config files added by cmd-parameter</b>\n(if one was provided)]-down->[<b>Last set all parameters given at command-line</b>\n(cmdline parser component)]
@enduml
All parameter are added on top of the previous ones and will overwrite existing values.
After the next::appsupport::ConfigClient::finish_init function is called the initialization is marked done and the server-client communication is enabled.

# Configuration Private Keys
The user has the possibility to create a list of configuration keys that will not be written (or read) to the configuration files and that cannot be updated from the GUI.
This will not be taken into consideration in the initialisation phase if they are loaded from a config file or provided by command argument.
The Private Key list is created using the next::appsupport::ConfigClient::definePrivateConfigKeys function in the initialization step of the Config Client or using next::appsupport::ConfigClient::putPrivateKey at runtime.

# Configuration GENERIC Keys
The user has the possibility to create a configuration component that can influence the values of all other components.
The keys in the GENERIC component will be added to each individual component.
Updating the value of one key from the GENERIC component will update the value of that key in all the components.
When saving a configuration all generic keys from the components that have the same value as the one in the GENERIC components will not be writtent to the file.

# Configuration tree

To store and manage the configurations a configuration tree structure is used.

* a configuration always starts on the first level with the component name
	* next level holds the global data, as version, name
	* on this level there is also a group for every functional group / component
		* logger (loglevel, sinks, ...)
		* profiler (on / off)
		* lifecycle (state)
		* inport (configuration for all input ports)
		* outport (configuration for all output ports)
		* composition (connection of input and output ports)
		* ...
		
For every group the content is designed by the corresponding component but aligned by config architect.

## Key Format

The following describes the format of the configuration keys in a configuration

- Allowed characters for the key name: [A-Za-z][0-9][_-]
  - <b>Hint</b>: Keys must not contain spaces
- Colon ":" is used in the keys to separate the levels in the configuration tree structure
  - \<root>:<child_node>
- Arrays
  - Square brackets "[ ]" can be used at the end of the key to specify an array index for the final child. Arrays at a higher level of the tree are <b>not</b> allowed.
- Examples
  - Supported
    - dummy:child:array<b>[id]</b>
  - Not Supported
    - dummy:child<b>[id]</b>:array
    - dummy:child<b>[id]</b>:array<b>[id]</b>


## Key Structure

### Player Component
```
next_player:orchestrator:config_file
next_player:trigger:<trigger_name>:function
next_player:trigger:<trigger_name>:triggervalue
next_player:stepping:time_step
next_player:realtime_factor
```

### Orchestrator Component
The Orchestrator runs in the player executable, therefor the configuration is associated to that
```
next_player:orchestrator:<flow_name>:type
next_player:orchestrator:<flow_name>:value
next_player:orchestrator:<flow_name>:synch_timestamp_url
next_player:orchestrator:<flow_name>:synch_id_url
next_player:orchestrator:<flow_name>:meta:delay
next_player:orchestrator:<flow_name>:meta:sync
next_player:orchestrator:<flow_name>:clients:name[id]
next_player:orchestrator:<flow_name>:clients:port[id]
```

### Databridge Component
```
next_databridge:web_socket_config:data_broadcast_ip
next_databridge:web_socket_config:data_request_ip
next_databridge:web_socket_config:data_broadcast_port
next_databridge:web_socket_config:data_requester_port
```

### Udex Component
```
<device_name>:udex:size_diff_threshold
```

### Framework Components
```
<device_name>:profiling:level

<device_name>:logging:<log_sink>:active           #valid values: for log_sink: console, file, global
<device_name>:logging:<log_sink>:loglevel         #valid values: debug, info, warn, err
<device_name>:logging:file:filename
<device_name>:logging:global:sendfrequency
next_controlbridge:logging:global:filename

```

### Algo Component
```
<device_name>:in_port:<port_name>:port:version
<device_name>:in_port:<port_name>:port:size
<device_name>:in_port:<port_name>:data:type
<device_name>:in_port:<port_name>:gia:adapter_dll
<device_name>:in_port:<port_name>:sync:algo
<device_name>:in_port:<port_name>:sync:syncref_url
<device_name>:in_port:<port_name>:sync:timestamp:url
<device_name>:in_port:<port_name>:sync:timestamp:name
<device_name>:in_port:<port_name>:sync:sync_id:url
<device_name>:in_port:<port_name>:sync:sync_id:name

<device_name>:out_port:<port_name>:port:version
<device_name>:out_port:<port_name>:port:size
<device_name>:out_port:<port_name>:sdl_file
<device_name>:out_port:<port_name>:url

<device_name>:system_services:timestamp:mode				#valid values: signal
<device_name>:system_services:timestamp:signal_url
<device_name>:system_services:timestamp:signal_unit			#valid values: s/ms/mus/ns

<device_name>:composition:<port_name>[id]
<device_name>:composition:<port_name>:active				#read only status (not loaded from config)
```

# Config Architecture

The following graphic depictes the software architecture of the configuration management.

@startuml

class ConfigCompDataStorage {
 + putDataInDatabaseTemplate(...)
 + getDataFromDatabase(...)
 + getDataListFromDatabase(...)
}

note top of ConfigCompDataStorage
  - storage of the config values for one component
end note

class ConfigCompData {
 + put(...)
 + get(...)
 + getChildrenKeys()
 + getValuesAsStringList()
 + configValueExists()
 + keyInfo()
 + getCompData()
 + isEmpty()
 + getCompName()
 - config_data_storage_
}

note right of ConfigCompData
  - contains the tree structure of the configuration 
    of one single component
  - stores and retrieves the config values in the 
    desired type by using ConfigCompDataStorage 
end note

class ConfigMetaInfo {
}

class ConfigDatabaseIO {
  + putCfg(...)
  + insertJsonString(...)
  + getCompConfigAsJsonString(...)
  + getDifferences(...)
}

note top of ConfigDatabaseIO
  - interface class to interact with ConfigCompData
    - adds config values via config file or formatted string to ConfigCompData 
      (needs to be formatted as described in chapter "Configuration tree") 
    - exports config values in a specific format, e.g. json-format
    - compares configurations and return differences
end note

class ConfigDatabaseManager {
}

class ConfigCommunicationLayer {
}

class ConfigManager {
}

class ConfigEvents {
}

class UserUtils {
}

ConfigCompDataStorage -down-> ConfigCompData::config_data_storage_

ConfigCompData -left-> ConfigDatabaseIO

ConfigCompData -down-> ConfigDatabaseManager
ConfigMetaInfo -down-> ConfigDatabaseManager
ConfigDatabaseIO -down-> ConfigDatabaseManager

ConfigDatabaseManager -down->ConfigEvents

ConfigDatabaseManager -down-> ConfigManager
ConfigCommunicationLayer -down-> ConfigManager
UserUtils -down-> ConfigManager
ConfigEvents -down-> ConfigManager

@enduml

# Onboarding Configuration handling

## Use Case 1: Getting to know the component
* How is a component configured and configurable
* No global requests available
* First startup of node without any config

## Use Case 2: Saving and loading

* Setting some configurations and loading it somewhere else in the component
* Storing configuration in a local configuration storage and loading it again
* Adapting the configuration and load it locally in the component

## Use Case 3: Global configuration handling

* Load information into the global config handler (config_server)
* Transfer the data to the component
* Change configuration of the component at runtime

## Use Case 4: Requesting config of another component in the system

Sometimes configurations from the \named{Next-Player}\glos{player} can be useful in other components. 
To request such data the ConfigClient is able to additionally set up a property tree for other components when receiving the global configuration.

* Load or set configuration in the component
* Read the component in another component
* Change comnfig data
* Reread the data

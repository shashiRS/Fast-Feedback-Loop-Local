DataPublisher {#udex_module_data_publisher}
==============

Module Overview Contents
* @ref udex_module_data_extractor
* @ref udex_module_data_publisher
* @ref udex_module_data_subscriber
* @ref udex_module_rawdata
* @ref udex_module_signal_explorer
* @ref udex_module_signals_description
___

[TOC]

##Player usecase

Player publishes data packets but in order to publish them it has to register the data sources. In the player usecase data description files are embedded directly in the recording and those are used to call the RegisterDataDescription, this creates the publishers that are ready to publish data. Each created publisher has a unique hash mapped to it based on the data description file.
<br>
Each data package contains info that can identify the unique hash, similar to the publisher creation, this hash maps to a publisher name, then data is published using the publisher name. 

@startuml
left to right direction
node "next_player : RegisterDataSources"{

node "next_udex : RegisterDataDescription"{

node "Hash Manager" #grey{
[sourceID]
[instanceNumber]
[cycleID]
[size]
[vaddr]
[URL (composed)]
[hash]
}

component [Data Publisher] #orange
component [pair(Hash,URL)] #grey

[Data Publisher] -d-> [data_source] : RegisterDataDescription

[data_source] -d-> [sourceID]
[data_source] -d-> [instanceNumber]
[data_source] -d-> [cycleID]
[data_source] -d-> [size]
[data_source] -d-> [vaddr]

[data_source] -d-> [signal_storage.sqlite]

[sourceID] --> [hash]
[instanceNumber] --> [hash]
[cycleID] --> [hash]
[size] --> [hash]
[vaddr] --> [hash]

[URL (composed)] -> [pair(Hash,URL)]
[hash] -> [pair(Hash,URL)]
[signal_storage.sqlite] -> [pair(Hash,URL)] : xtypes description
[pair(Hash,URL)] -> [Data Publisher] : CreateDynamicPublisher \n with URL name \n and xtypes description \n and map it to hash
}
}
@enduml

@startuml
left to right direction
node "next_player : PublishPackage"{

node "next_udex : PublishData"{

node "Hash Manager" #grey{
[sourceID]
[instanceNumber]
[cycleID]
[size]
[vaddr]
[URL (composed)]
[hash]
}

component [Data Publisher] #orange

[Data Publisher] -d-> [package]

[package] -d-> [sourceID]
[package] -d-> [instanceNumber]
[package] -d-> [cycleID]
[package] -d-> [size]
[package] -d-> [vaddr]

[sourceID] --> [hash]
[instanceNumber] --> [hash]
[cycleID] --> [hash]
[size] --> [hash]
[vaddr] --> [hash]

[hash] -> [Data Publisher] 

[package] -u-> [PublishData : Hash->Publisher(package.payload)]
[Data Publisher] -u-> [PublishData : Hash->Publisher(package.payload)]
}
}
@enduml

___

##Nodes usecase (generic publishing)

To publish data with help of the `DataPublisher` three steps have to be followed.

<ul>
	<li>
	**Create** `DataPublisher(string node_name)` with a respective node name to allow the \named{NEXT Simulation Framework}\glos{simulation_framework} to distinguish between \named{Nodes}\glos{nodes}. 
	<br> 
	This creates a DataPublisher to be used to register and publish data
	</li>

	<li> 
	**Register** signal descriptions via a (multiple calls are allowed and stack up) 
	<ul>
		<li>File from disk `RegisterDataDescription(string description_filepath)` </li>
		<li>Data description as a binary memory stream `RegisterDataDescription(char *file_name, void *binary_description, size_t content_size)` </li>
		<li>Port name only for a non-interpretable data stream with specified size and name `RegisterDataDescription(string port_name,size_t port_size)` </li>
	</ul>
	this crates multiple dynamic publishers that contain a topic name and a topic description
	@startuml
	left to right direction
	node "next_udex : RegisterDataDescription"{

	node "Hash Manager" #grey{
	[sourceID]
	[instanceNumber]
	[cycleID]
	[size]
	[vaddr]
	[URL (composed)]
	[hash]
	}

	component [Data Publisher] #orange
	component [pair(Hash,URL)] #grey

	[Data Publisher] -d-> [SDL] : RegisterDataDescription

	[SDL] -d-> [sourceID]
	[SDL] -d-> [instanceNumber]
	[SDL] -d-> [cycleID]
	[SDL] -d-> [size]
	[SDL] -d-> [vaddr]

	[SDL] -d-> [signal_storage.sqlite]

	[sourceID] --> [hash]
	[instanceNumber] --> [hash]
	[cycleID] --> [hash]
	[size] --> [hash]
	[vaddr] --> [hash]

	[URL (composed)] -> [pair(Hash,URL)]
	[hash] -> [pair(Hash,URL)]
	[signal_storage.sqlite] -> [pair(Hash,URL)] : xtypes description
	[pair(Hash,URL)] -> [Data Publisher] : CreateDynamicPublisher \n with URL name \n and xtypes description \n and map it to hash
	}
	@enduml

	</li>
	
	<li>
	**Publish** data using `publishData(string port_name, void* binary_data, size_t binary_size)`
	<br>
	this actually sends out the data, based on the publisher name.
	@startuml
	left to right direction
	node "next_udex : PublishData"{

	node "Hash Manager" #grey{
	[sourceID]
	[instanceNumber]
	[cycleID]
	[size]
	[vaddr]
	[hash]
	[pair(Hash,URL)]
	}

	component [Data Publisher] #orange

	[Data Publisher] -d-> [payload]
	[Data Publisher] -d-> [URL]

	[payload] -u-> [PublishData: Hash->Publisher(package.payload)]
	[Data Publisher] -u-> [PublishData: Hash->Publisher(package.payload)]

	[pair(Hash,URL)] -> [hash]
	[URL] -> [pair(Hash,URL)]
	[hash] -> [Data Publisher]
	}

	@enduml
	</li>
</ul>

Other functionality:<br>
After doing all the register data description, the list of available topics can be checked with `getTopicsList`, this will return a list of all the publisher names and a unique hash for each, hash is not really relevant at this point but it is there for historical reasons, the name can and should be used for publishing data.

Examples on how to register different DataDescriptions and how to publish can be found in this test `tests/unit/udex/data_extractor_test/data_extractor_test.cpp`. Most relevant unit test:
'TEST_F(DataPublisherTestFixture, publisher_register_data_from_sdl_file_and_publish)'
To see some publish examples with some signal description tests and tests for `getTopicsList` see `tests/unit/udex/data_publisher_test/data_publisher_test.cpp`


@startuml
partition Publisher #Orange{
"User Init" -->[Data description] "RegisterDataDescription"
"User PublishData" -->[binary blob] "PublishData"
"RequestTopicNames" ->[topic name] "PublishData"
}

PublishData -->[binary blob using correct topic name] "eCAL Publisher"
"eCAL Publisher" -> (*)

partition SignalStorage #grey {
"RegisterDataDescription" --> "Register to database"
"Register to database" ->[Signal description] ===S1===
"Register to database" ->[List of topics] ===S2===
===S2=== -> "RequestTopicNames"
===S1=== ->[topic description] "eCAL Publisher"
}
@enduml

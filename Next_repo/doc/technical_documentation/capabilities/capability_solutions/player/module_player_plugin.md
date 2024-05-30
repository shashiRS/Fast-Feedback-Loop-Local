Player Plugin{#player_module_player_plugin}

Module Overview Contents

* @ref player_module_file_reader
* @ref player_module_state_machine
* @ref player_module_play_manager
* @ref player_module_publisher_manager
* @ref player_module_resource_manager

___

[TOC]

# Overview

To enable project specific files and rapid adaption to custom file types a plugin concept is implemented in the next_player.
This incorporates a clear API to forward filenames and receiver signal description as well as packages.

The player plugin is placed at the part of the FileReader. A short schema can be seen below.

@startuml Player Plugin
hide empty description

state Next_Player {
state readerPlugin
state packageQueue
state publisher
}
state Project #red {
state fileReader #red : get a list of files as input and produces sorted and synchronized packages from all the input files
}

state Component

packageQueue -> publisher
publisher -> Component

readerPlugin --> fileReader : gives file names
fileReader -> package: gives packages
package --> readerPlugin : takes packages
readerPlugin -> packageQueue : pushes packages in same order
@enduml

# Reader Interface

On startup the plugin matching plugin will be loaded by requesting its file extension. Afterwards an open File command
with a list of files will be given. Afterward the cached descriptions are requested form the Plugin and a actual list of publishers is agan provided to the plugin.
In this call missing topics can be added with a name and size
This finalizes the start-up procedure.

The publishers from the data description are created on group level of the respective data description.
The URL Layer in short is:
$Device$.$View$.$Group§.§Subgroups§....

The device is given via the source ID and the name as well as an instance number. Within the descriptions the view and group levels are extracted.
This is done according to the extraction schema provided by the core_lib and matches the extraction levels of MTS2.6

@startuml
Next_Player -> NextFileReader : openFile
NextFileReader -> ReaderPlugin : static getFileExtension
NextFileReader -> ReaderPlugin : createInstance
NextFileReader -> ReaderPlugin : openFile(progress_callback)
NextFileReader -> ReaderPlugin : getCurrentDataDescription()
ReaderPlugin --> NextFileReader : vector<DataDescription>
NextFileReader -> DataPublisher : registerDescriptions
NextFileReader --> DataPublisher : vector<DataDescription>
NextFileReader -> DataPublisher : getCurrentTopics
DataPublisher --> NextFileReader : vector<topic,hash>
NextFileReader -> ReaderPlugin : setCurrentPackageNames
NextFileReader --> ReaderPlugin : vector<topic,hash>
ReaderPlugin --> NextFileReader : vector<missing_topic,size>
@enduml

# Package Interface

To provide a package multiple ways are possible.
Publishing can be handled via

* name
* virtual address
* package meta information

The package meta information is read during description parsing and used to create the publishers.
The meta information needs to be unique for a simulation regarding device, view and group level properties
(Instance_number, Source_id, Cycle_id).

Additionally the publishing can be done via the respective group level URL or via the virtual address.
Those are cached and connected to the respective publishers.

During publishing an IPackage can include multiple subpackages. The IPackage is requested indefinitely to deliver payloads
until it declines the request via PayloadAvailable

@startuml

hide empty description
State Publisher
State PayloadAvailable
State GetCurrentMetaType
State Hash
State GetPayload
State SendIPackage

Publisher --> PayloadAvailable
PayloadAvailable --> GetCurrentMetaType
GetCurrentMetaType --> Hash
Hash --> GetPayload
GetPayload --> SendIPackage
SendIPackage --> PayloadAvailable
SendIPackage --> Publisher
Publisher --> PackageReset
@enduml



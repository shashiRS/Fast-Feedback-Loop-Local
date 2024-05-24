Publisher Manager{#player_module_publisher_manager}
==============

Module Overview Contents
* @ref player_module_file_reader
* @ref player_module_state_machine
* @ref player_module_play_manager
* @ref player_module_player_plugin
* @ref player_module_resource_manager

___

[TOC]


To support the merge of multiple files synchronously the ReaderSynchronizer is able to read multiple files with help of the ReaderInterface and ReaderPlugins.
The ReaderSynchronizer uses the ReaderManager, finds matching reader plugins and then sorts the packages from all readerInterfaces into the publisher queue.

@startuml

hide empty description

state Publisher

state FileReader
state ReaderSynchronizer {
state Files <<entryPoint>>
state ReaderManager
state ReaderPlugins #grey
state PackageBuffer #grey
state PackageSorter
state PublisherQueue <<exitPoint>>
}

FileReader --> Files :  provide file names
Files --> ReaderManager : ask Readers for files
ReaderManager -> ReaderPlugins : create reader instances
ReaderPlugins --> PackageSorter: getPackage
PackageSorter --> PackageBuffer : sort in via PackageTimestamp
PackageBuffer --> PublisherQueue : all Readers have same timestamp
PublisherQueue -> Publisher

@enduml
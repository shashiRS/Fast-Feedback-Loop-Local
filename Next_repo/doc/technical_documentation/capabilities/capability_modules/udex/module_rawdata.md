Raw Data{#udex_module_rawdata}
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

Next-Udex is capable of transforming any data structure (ECU, CAN, ETHERNET) into a understandable cpp-like data structure.
The data evaluation is split up into two parts.

* normalizing any data format into a cpp memory block
* signal requesting reading out some data from the cpp memory block and casting to requested signals

The data normalization is done with the __data_extraction__ which is handled in a background thread to enhance performance. 
This action is done block-wise meaning for a full package received via the \named{DataSubscriber}.

Afterwards the signal requests (block-wise or signal-based) are handled with another module which handles the already parsed 
cpp memory alignment. This module also take care of casting signals into the requested format. Additionally a dynamic strucutre based module is also planned.

@startuml

hide empty description

state data_extraction: transform memory into normalized cpp memory. currently done in "corelib_wrapper". Done by rajesh

state xtypeschema #grey: signal description from corelib.\nTransfered with eCAL monitor layer


state data_extraction {
state rawpackage: raw non interpretable memory blob
state data_extractor #red: extracts the data
state cpu_package: cpp-like memory alignment to just use offsets

xtypeschema --> data_extractor
rawpackage -> data_extractor
data_extractor --> cpu_package :  transform ETH bytes\ninto cpp memory structure
data_extractor --> cpu_package :  transform CAN bytes\ninto cpp memory structure
data_extractor --> cpu_package :  copy little-endian ECU bytes\ninto cpp memory structure
data_extractor -[#red]-> cpu_package :  transform big-endian ECU bytes\ninto cpp memory structure
}
cpu_package --> ===memory===
===memory=== -> byte_array 

state signal_requests {

xtypeschema --> schema_parser #orange :  takes in xtypes returns list of signals with offset, size, array, type
schema_parser --> signal_caster :  takes in xtypes returns list of signals with offset, size, array, type
type_caster #orange -> signal_caster : takes atomic values and transforms them
===memory=== --> signal_caster : takes care of casting float to double or int to byte etc.
signal_caster --> float :  read a atomic value\nform the cpp memory structure
signal_caster --> bool :  read a atomic value\nform the cpp memory structure
signal_caster --> double :  read a atomic value\nform the cpp memory structure
signal_caster --> info :  read a structure info value\nform the cpp memory structure
}


@enduml


Accessing data is a two step process:
* deserialize the raw package with respect to endianness, factors, offsets, compression etc.
* retrieve a offset and a signal type from the core-lib description to cast the memory address into the correct value

Additionally we offer a type-casting to retrieve data in a arbitrary data type and convert it (i.E. double to float)
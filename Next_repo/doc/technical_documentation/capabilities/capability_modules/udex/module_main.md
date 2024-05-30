NEXT UDex Module Overview{#udex_module_main}
==============

Module Overview Contents
* @ref udex_module_data_extractor
* @ref udex_module_data_publisher
* @ref udex_module_data_subscriber
* @ref udex_module_rawdata
* @ref udex_module_signal_explorer
* @ref udex_module_signals_description
* @ref udex_module_signal_watcher
___

[TOC]

The four main modules are described here. For examples on how to use them please check out 
* the component tests ```tests/component```
* the unit tests for the modules 
  * ```tests/unit/udex/data_subscriber_test/data_subscriber_test.cpp```
  * ```tests/unit/udex/data_publisher_test/data_publisher_test.cpp```
  * ```tests/unit/udex/data_extractor_test/data_extractor_test.cpp```
  * ```tests/unit/udex/signal_explorer_test/signal_explorer_test.cpp```

# Publishing Data

To publish data one can use the ```DataSubscriber``` which has two requirements
* Data description (via SDL files, FIBEX files, etc.)
* Data publishing (by port level name and packages)

More details can be found [here](@ref udex_module_datapublisher) 

# Subscribing to data

To subscribe to data two modules are needed
* A ```DataSubscriber``` which takes care of the actual data input from the middleware (eCAL)
* A ```DataExtractor``` to buffer and extract all incoming packages to the required needs

A subscription can be data-driven via a hook / callback system within the extractor or trigger-driven with buffered queues.
Different synchronization methods are available within the ```DataExtractor``` which can be found [here](@ref udex_module_dataextractor)

All middleware specific adaptions are handled within the ```DataSubscriber``` and are abstracted away from the user as seen [here](@ref udex_module_datasubscriber)

# Signal Explorer

The Signal Explorer provides entry points to search for signals and understand the current signals which are available.
Details can be found [here](@ref udex_module_signalexplorer)

Tutorials for custom package processing {#howto_package_processor}
=====

[TOC]

---

# References

* @ref howto_setup_package_processor
* @ref howto_package_processor_example
* @ref howto_load_package_processor
* @ref howto_debug_package_processor

---

# Overview {#howto_package_processor_overview}

Usually all processing of data happens at the receiver side due to:

* fast transmission
* fast package reading
* processing on need to know basis

If special recording packages (hardware data, compressed images) are received an additional injection point is available.
This interface is called \named{Package Processor} and can be loaded into any \named{DataPublisher}\glos{data_publisher}.

The Package Processor implements three different crucial interfaces

* defining what packages to process via MetaInformation
* defining what the resulting structures will look like via DataDescriptions (sdl, cdl, dbc, etc.. (all supported thins -> link))
* processing the incoming package _without_ changing it and providing a new IPackage to be forwarded through the system

@startuml
hide empty description

state NextPlayer : loads recordings and publishes via the DataPublisher
state DataPublisher : puts packages into the NEXT Simulation network
state DataSubscriber : receives packages
state recording

state DataPublisher {

state SignalDescription <<entryPoint>>
state packages <<entryPoint>>
state out <<expansionOutput>>
state DataDescriptionDB <<fork>>
state packagesDB <<join>>

state PackageProcessor : is loaded inside the DataPublisher
state PackageProcessor {
state provideDescription
state provideMetaInfo
state processPackage

}
}

recording -> NextPlayer : DataDescription
NextPlayer --> SignalDescription

SignalDescription -right-> DataDescriptionDB
SignalDescription --[#orange]> provideDescription : trigger \n request
provideDescription --> DataDescriptionDB

DataDescriptionDB -> out

recording -> NextPlayer : packages
NextPlayer --> packages
packages --> packagesDB

packages -[#orange]> provideMetaInfo
provideMetaInfo --> packagesDB : check if \n package matches

packagesDB -[#lightblue]-> processPackage
processPackage -[#lightblue]-> packagesDB
note left of processPackage : only if matched

packagesDB --> out
out --> DataSubscriber
@enduml

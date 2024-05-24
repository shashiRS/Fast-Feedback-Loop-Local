How to setup a PackageProcessor {#howto_setup_package_processor}
=====

[TOC]

---

* @ref howto_package_processor_overview
* @ref howto_setup_package_processor
* @ref howto_package_processor_example
* @ref howto_load_package_processor
* @ref howto_debug_package_processor

---

# Summary

To setup a package processor only the abstract class PackageProcessor from ```src/udex/interface/next/udex/publisher/package_processor.hpp``` needs to be implemented.
As a second step the class needs to be bundled into a shared library and put into the subdirectory ```/plugins/package_processors``` relative to the
__working directory__ of the \named{NEXT-Player}\glos{player}.

In short:

* derive from abstract class ```PackageProcessor``` and implement interface
* create C-API compatible dll exposing the entry function
* put dll into ```/plugins/package_processors```

# Implementing the interface

## CreateNewInstance

This function delivers an interface to create a new instance of the processor. Using shared pointers makes a safe handling a lot easier.
Just create a new instance of you processor here and return it.
Be sure not to rely on global variables here as they will be shared over multiple instances!

## provideDataDescription

This function allows the user to provide data format to process and register it to the \named{NEXT Simulation Framework}\glos{simulation_framework}.
The data descriptions require

* device_name (non-whitespace character with only letters)
* PackageFormat (MTA_SW, CAN, ...)
* instance_number (important if the same device of the original data needs to be used)
* source_id
* DescriptionFormat (sdl, cld,....)

and the actual description into the binary_description buffer.

## provideRequestedPackageInfo

> ❗ If no packages are registered the package processor will not be loaded into the DataPublisher ❗

Here the matching packages which should be processed can be defined.
Each package is defined via

* source_id
* instance_number
* cycle_id
* virtual_address
* service_id
* method_id

Multiple package infos can be registered and all matching packages will be put to the Processing function by the \named{DataPublisher}\glos{data_publisher}

### processPackage

Create a new package which is propagated through the system then.

* make sure to use a memory safe implementation of IPackage.

Access the package to your liking.

> ❗ Make sure _not_ to move the original package ❗

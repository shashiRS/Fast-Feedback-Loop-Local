NEXT SDK in a nutshell {#sdk_main}
==========================
[TOC]

\named{NEXT SDK} is a \named{Capability Module}\glos{capability_module} of the \named{NEXT Simulation Framework}\glos{simulation_framework} which provides the entry functions to interact with the data in the \named{NEXT Simulation Network}\glos{simulation_network}.


The functionalities include
* logging information with different levels and file dumps with help of the @ref sdk_module_logger
* Initalization and finalizing of user defined modules by using the @ref sdk_module_initchain
* Controlled and central initalization and finishing of eCAL by @ref sdk_module_ecalsingleton
* CMake helper function for setting up unit tests: `next_create_test`
* unit test helper class for using eCAL within unit tests: next::sdk::testing::TestUsingEcal

This means that all data related functions are summarized within \named{NEXT SDK} and allow anyone to interact with the \named{NEXT Simulation Network}\glos{simulation_network}.

For more details see [Capabilities](#sdk_capabilities_main)

Typical use-cases are


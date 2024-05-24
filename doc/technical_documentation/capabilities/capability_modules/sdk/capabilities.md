Capabilities {#sdk_capabilities}
==============

# ConfigClient

* support distributed config management
  * global config gathered in the ```next_controlbridge.exe```
  * distributes local configuration and pushes global config to distributed \named{Nodes}\glos{nodes}
* provides multiple use cases
  * loading local configurations and publish them to a global configuration (generating a combined configuration)
  * local information with the possibility to read them globally (i.e. version number)
  * global changed configuration pushed to the local configuration (i.e. file_name)
* properties can be tagged to manage merging of configs
* callback mechanism to receive and react on updated data form global configuration
* All configuration files can be partially or fully. Locally loaded configuration will ignore data that is not relevant.

# CrashDump

# eCAL Singleton

The \named{eCAL Singleton} takes care of initializing and finishing eCAL. It is used via the \named{Init Chain} (see [below](#sdk_capabilities_main-initchain) and its [detailed description](#sdk_module_initchain)).

For a detailed explanation see @ref module_ecalsingleton.

# Eventsystem
The \named{Eventsystem} is a small framework which helps to create events which can be subscribed to or published. More details about the \named{Eventsystem} [can be found here.](#sdk_module_events)

# Init Chain {#sdk_capabilities-initchain}

* supporting initializing and resetting modules in an ordered way
* ensures that modules, which are providing functionalities for other modules, are being initialized first

For a detailed explanation see @ref sdk_module_initchain.

# LifeCycle
The \named{Next SDK} provides a simple state machine for common lifecycle management. See for more [details here](#sdk_module_lifecycle)

# Logger

* logging functionality provided by the shared library `next_sdk.dll` (Windows) or `libnext_sdk.so` (Linux)
* supports logging to console and to log log files
  * log levels `debug`, `info`, `warn` and `error`
  * the logging levels of console and file can be set independently
  * logging can also be disabled
  * log level can be changed during runtime
* log filename can be choosen freely

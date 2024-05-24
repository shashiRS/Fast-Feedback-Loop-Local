Init Chain{#sdk_module_initchain}
==============

Module Overview Contents
* @ref sdk_module_main
* @ref sdk_module_config
* @ref sdk_module_lifecycle
* @ref sdk_module_event_system
* @ref sdk_module_ecalsingleton
* @ref sdk_module_tracy
___

# General description

The \named{Init Chain} provides the possibility to start and stop modules in a specific order.

A typical use case is an application, which uses for example the logger and eCAL. In this case the logger shall be initialized before eCAL is being initialized but shall be closed after eCAL is stopped.

Every module, which provides at least a init function (taking specific arguments), can be used and triggered by the \named{Init Chain}. The reset function is optional.

It is possible to give configuration options to the modules, thus adjusting the behavior of the modules depending on your need.

\note
Calling the run method a second time, after it was already successfully called and the reset function was not called in between, will simply return false. No other action is performed by the run method in this case.

\note
If calling the run more than once, with calling reset in between, only the first run is considering all modules. In later calls the modules without a reset function are not called again (since they were not reset).

[TOC]

# Technical description

To use the \named{Init Chain} the header file next/sdk/init_chain/simple_init_chain.hpp must be included. For every used module, which shall be considered by the \named{Init Chain}, the modules must be added to the chain. Afterwards by calling the static function next::sdk::InitChain::Run the modules can be started up. At any time, e.g. while the application shuts down, the static function next::sdk::InitChain::Reset can be called to stop or reset the modules.

## Registering modules

A module, which shall be used by the \named{Init Chain}, has to offer at least one function for starting it up. The reset function is optional. Both functions are taking a map for configuration options and an integer giving the priority as arguments. The map, the init and reset functions are defined in simple_init_chain.hpp as followed:
```{.cpp}
using ConfigMap = std::map<std::string, std::any>;
typedef bool (*InitFunc)(ConfigMap const &config_map, int level);
typedef void (*ResetFunc)(ConfigMap const &config_map, int level);
```
The map contains key-value pairs, where the key is of type `std::string`, whereas the value is of type `std::any`, thus any type can be used. The `int level` gives the information about the priority. By this it would be possible to call the same initialization function of one module several times, each call having another priority, providing the possibility of a split start up procedure (will not be needed in most cases).

Note that the initialization functions are returning a boolean value, indicating whether the initialization was successful or it failed. The reset function is not allowed to fail. The reason is, that this function mostly will be called while the application is closing, so there might be no possibility by the application to handle failures.

The registering is done by creating an object of type next::sdk::InitChain::El. Its constructor is taking the priority, the name of the init and the name of the reset function. It could look like this:
```{.cpp}
next::sdk::InitChain::El init_el{100, my_init_function, my_reset_function};
```

Using the same priority twice is possible. The object added last will be initialized first (LIFO).

## Using configuration options

To give configuration options to the initialization and reset functions the ConfigMap is used. As written above, it is using `std::map<std::string, std::any>`. Simply put your configuration option(s) into this map, trying to use an unique key where you are quite sure other modules will not use. Good practice is naming the module, for example the logger is using `logger-` as prefix within the keys (e.g. `logger-filename` for the filename of the log file).

Following the example code of setting one value of the map.
```{.cpp}
ConfigMap config;
config["myModule-int"] = 5;
```

Here an example for checking the existence of a non-optional configuration option of type int, reading the value of the given option:
```{.cpp}
bool init(const ConfigMap &map, int) {
  cit = map.find("myModule-int");
  if (map.end() != cit) {
    if (cit->second.type() == typeid(int)) {
	  my_int = std::any_cast<int>(cit->second);
	  return true;
	}
  }
  return false;
}
```

## Initializing and resetting registered modules

Once all chain elements are created you are good to run the `Run` function of the \named{Init Chain} and the `Reset` function (if needed).
```{.cpp}
next::sdk::InitChain::Run(my_config_map)
[...]
next::sdk::InitChain::Reset();
```
Note that in the short example the return value of the `Run` function is not checked, in your real application you have to do that.

\note
Giving the configuration map to the Run and Reset function is optional. When you give no configuration map, each init and reset function will be called with an empty map.

## Priority

The order of initializing the modules is given by its priority, which is basically an `int`. The lower the number, the earlier it gets initialized. Please only use values from 0 to 999. For resetting the order is in the other way. The module with the highest number is reset first.

For the logger we are using priority 100, eCAL is initialized with priority 120. By this we are ensuring that the logger is initialized first, before trying to use eCAL. Typically choosing something in range from 200 to 400 is fine. To make sure that your module is being the last one initialized choose 999.

# Usage within NEXT

Within NEXT the init chain is used to initialize the logger and eCAL (\ref sdk_module_initchain). Depending on the use case also further functionalities.

The modules are providing helper functions for getting the config keys and also for registering the modules in the init chain using the default priorities.

# Example {#sdk_module_initchain-example}

The following example shows how to use the \named{Init Chain} for initialization and resetting. Following modules are used:
* logger
* eCAL Singleton
* profiler

The required options are provided via the configuration map. See the following example.

```{.cpp}
#include <next/sdk/sdk.hpp>

int main(int, char**) {
  next::sdk::ConfigMap config;
  config.emplace(next::sdk::logger::getFilenameCfgKey(), "my_application.log");
  config.emplace(next::sdk::logger::getSentinelProductNameCfgKey(), "my_application");
  config.emplace(next::sdk::logger::getFileLoglevelCfgKey(), logger::LOGLEVEL::DEBUG);
  config.emplace(next::sdk::logger::getConsoleLoglevelCfgKey(), logger::LOGLEVEL::INFO);
  config.emplace(next::sdk::EcalSingleton::getInstancenameCfgKey(), "my_application");
  config.emplace(next_profiling::getProfileLevelCfgKey(), next_profiling::ProfilingLevel::OVERVIEW);
  
  next::sdk::logger::register_to_init_chain();
  next::sdk::EcalSingleton::register_to_init_chain();
  next::sdk::profiler::register_to_init_chain();
  next::sdk::version_manager::register_to_init_chain();
  
  next::sdk::InitChain::El myModule_init_element{UserLevelBase() + 50, MyModule::init, MyModule::deinit};
  
  if(!next::sdk::InitChain::Run(config)) {
    std::cerr << "Initialization failed!" << std::endl;
	return -1;
  }
  
  [...]
  
  next::sdk::InitChain::Reset();
  return 0;
}
```

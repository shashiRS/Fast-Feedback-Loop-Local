eCAL Singleton {#sdk_module_ecalsingleton}
=============

Module Overview Contents
* @ref sdk_module_main
* @ref sdk_module_config
* @ref sdk_module_lifecycle
* @ref sdk_module_initchain
* @ref sdk_module_event_system
* @ref sdk_module_tracy
___

# General description

The \named{eCAL Singleton} helps the developer to initialize the eCAL communication layer during application start up and also finishing it during shutdown of the application.

It ist meant to be used via the @ref module_initchain. The [Init Chain example](#module_initchain-example) is showing how to use it.

\note
Use a unique name for the application, especially when the application runs in parallel to other application. Saying that means do not simply name your unit test application "test" or "test2".

[TOC]

# Technical description

The function next::sdk::EcalSingleton::do_init expects the instance name which shall be used within the configuration map. Use next::sdk::EcalSingleton::getInstancenameCfgKey for getting the option name. The init function then calls the initialize function of eCAL.

As the name says, it is a singleton. This means by closing the application the corresponding object will be destroyed. Within its destructor the \named{eCAL Singleton} will call the finalize function of eCAL.

For unit testing or controlled shut down the function next::sdk::EcalSingleton::do_reset can be called. This function allows destroying the singleton object, thus calling the eCAL finalize function, also during runtime.

# Unit testing

As written above, for unit testing it is possible to finalize eCAL during application runtime. This is for example possible for every test case within one test application. To make it even easier the class next::sdk::testing::TestUsingEcal is provided. This class is subclassing testing::Test provided by GoogleTest. By default this class is initializing \named{eCAL singleton} (thus eCAL itself) and the logger (setting the log level to next::sdk::logger::LOGLEVEL:OFF by default).

Making your own tests, using this class, you would subclass from next::sdk::testing::TestUsingEcal. Within its constructor you have to set the member variable `instance_name_` to the eCAL instance name. It is advisable that this name is unique, thus not interfering with other tests. A good choice is having the test application name within the instance name.

Please include the file next/sdk/sdk.hpp if you want to use it. As already written next::sdk::testing::TestUsingEcal is using the \named{Init Chain} and the \named{logger}. If you include the test_using_ecal.hpp directly an error message during compilation will be raised.

A simple test application can then look like this:
```{.cpp}
#include <next/sdk/sdk.hpp>

class my_test_class : public next::sdk::testing::TestUsingEcal {
public:
  my_test_class() {
    instance_name_ = "MyTestClass";
	// we want to see log messages, so we set the console log level to DEBUG
	initmap_[next::sdk::logger::getConsoleLoglevelCfgKey()] = next::sdk::logger::LOGLEVEL::DEBUG;
  }
};

TEST_F(my_test_class, exampleTest) {
  // put your code here
}
```

For detailed information about GoogleTest see [GoogleTest Primer](http://google.github.io/googletest/primer.html) and [GoogleTest Testing Reference](http://google.github.io/googletest/reference/testing.html).

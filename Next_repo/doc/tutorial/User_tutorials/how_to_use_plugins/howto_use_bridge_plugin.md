How to use Bridge Plugins {#howto_use_bridge_plugin}
=====

[TOC]

---

# Overview {#howto_use_bridge_plugin_overview}

This overview answers the following questions?

* How do I get my plugin registered with the databridge?
* How does my loaded plugin get interacted with?

Follow ups on how to visualize data can be found in @ref howto_use_bridge_plugin_visualize_points which will use some of the addons explained here: @ref howto_use_bridge_plugin_addons

# How do i get my Plugin registered {#howto_bridge_plugin_registratoin}

A Plugin is loaded from the \named{NEXT-Databridge}\glos{databridge} if its located as a folder within the subdirectory ```plugins```.
This subfolder is search from the current working directory by ```next_databridge.exe```.
Within the subdirectory ```plugins``` all folders are evaluated and accepted as a plugin if they contain:

* a ```.fbs``` to describe the data provided
* a ```.dll``` \ ```.so``` file having the proper function calls from ```plugin.h```

An example for a plugin __myTpPlugin__ providing traffic participants to the UI would have following folder structure

```
bin 
   |
    -- databridge.exe
    -- plugins
        |
         -- myTpPlugin
            |
             -- traficparticipatn.fbs // provides the data description
             -- traficparticipatn.bfbs // required for visualization
             -- myTpPlugin.dll // actual implementation using plugin.h 
         -- other Plugins
```

When searching through the subdirectories of ```plugins``` the databridge reads out the datatypes of the ```.fbs``` and associates those with the requests from the NEXT-GUI.
Make sure to handle here a proper datatype so the NEXT-GUI is able to be connected to your plugin.
If the datatype is proper a request on the NEXT-GUI for example for possible traffic participants or camera pictures should result in interface calls to your plugins.

# How does my Plugin gets interacted with {#howto_bridge_plugin_interaction}

The Bridges Plugins (```src/bridges/bridgesdk/interface/next/bridgesdk/plugin.h```) interface introduces two main functionalities:

* Entry functions which are called based on the current lifecycle of the simulation or interactions of the user
* Addons which allow for interaction with the NEXT Simulation system itself

> 📝 Each configuration is set to a own instance of the Plugin. All plugin entry functions are managed to be called threadsafe. If two widgets are started in the UI the requests are split into seperate plugins. Therefore plugins only need to manage a single configuration.

## Init

The starting point for any interaction it the ```init``` call as soon as the dll is loaded and a plugin instance is created.
This function is called only once when the plugin is loaded and created. Usually addons are created here. Do basic required initialization at that point.

## getDescription

The next contact is usually a call to  ```getDescription``` which gives the plugin the opportunity to describe in more detail what it can do and what can be confired by the user.
Mostly this concerns color, sensor name, visualization URLs etc.

## addConfig / removeConfig

When a user now selects a description and fills the required parameters the respective configuration is send back to the plugin via the ```addConfig``` call.
This is the moment to set up all the simulation dependent connects and structures and setting up callbacks to send stuff to the UI.
When the configuration is removed on the UI the ```removeConfig``` is called.

## enterReset / exitReset

Is a reset event triggered in the Simulation Network (via recording being loaded or manual triggers in the UI) the plugin is notified via the ```enterReset``` and ```exitReset``` call.
The ```enterReset``` should remove all subscriptions and network dependencies and set up dependencies for other members on the Network (i.E. a DataPublisher (@ref udex_module_data_publisher) to a SimNode).
On ```exitReset``` it can be assumed that all network dependencies are set up and subscriptions can be started again.
> 📝 Especially if a configuration failed before because of lacking data this is the proper place to try again!


@startuml
hide empty description

state Plugin {
state LifeCycle {
state init #red : Gets called once at instance creation. \n Can be used to set up addons.
state enterReset #blue : Gets called when the Simulation Network goes into reset. \n Used to destroy member variables. \n Set up dependencies for other plugins here (i.E DataPublishers).
state exitReset #blue : Gets called after all reset actions were taken. \n All Publishers can be expected to be available here.

init -[#FFFFFF]-> enterReset
enterReset -[#FFFFFF]-> exitReset

}
state UserInteraction {
state update #green : Gets called when a user interaction is reported on the widget. \n Used for export start for example. \n WHEN: interaction in the widget itself (start export)
state getDescription #green : Request the possible configurations and options to configure your plugin. \n WHEN: user adds a configuration to a widget (traffic participant config)
state addConfig #green : Hands over the user selected configuration from the UI. \n WHEN: user adds removes a configuration
state removeConfig #green: Is called when the user removes the respective datasource in the UI \n The Plugin is not needed anymore. \n WHEN: user looks at possible visualzations

update -[#FFFFFF]-> getDescription
getDescription -[#FFFFFF]-> addConfig
addConfig -[#FFFFFF]-> removeConfig

}
}

NextPlayer#orange -> enterReset : recording is loaded
NextPlayer --> SimulationFramework#orange : trigger reset for all Publishers
SimulationFramework --> exitReset : Publishers are set up

NextGUI#orange --> update
NextGUI --> addConfig
NextGUI --> removeConfig
NextGUI --> getDescription

@enduml

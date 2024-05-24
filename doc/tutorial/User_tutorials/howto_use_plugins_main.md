Tutorials for Plugins {#howto_use_plugins_main}
=====

[TOC]

---

# References {#howto_export_main_references}

* @ref howto_use_bridge_plugin
* @ref howto_use_bridge_plugin_addons
* @ref howto_use_bridge_plugin_visualize_points

---

# Overview {#howto_use_plugins_main_overview}

Plugins are a shared library based concept to give clearly defined API into next solutions to expand functionality for project specific task.
Right now there are two Plugin-Interfaces implemented

* Reader Plugin ```ReaderInterface``` to enable custom file reading into the \named{NEXT-Player}\glos{player} component.
  * interface can be found here ```src/player/interface/next/player/plugin/reader_interface.hpp```
  * dlls need to be put into a folder in the working directory of \named{NEXT-Player}\glos{player} under ```/plugin/```
* Bridges Plugin ```Plugin``` to enable custom post processing and visualization into \named{NEXT-Databridge}\glos{databridge} component.
  * interface can be found here ```src/bridges/bridgesdk/interface/next/bridgesdk/plugin.h```
  * dlls need to be put into a folder in the working directory of \named{NEXT-Databridge}\glos{databridge} under ```/plugin/```

@startuml
hide empty description

state UserPlugins #orange : can be created and loaded \n on runtime by the users of next
state NextFramework #green : are delivered with the \n framework as a basic functionality

state recordings {
state rrec #green
state myformat #orange
}
state NEXT_Player {
state coreReader #green
state ReaderPlugin #orange
}
state NEXT_GUI
state NEXT_Databridge {
state Plugin #orange
}

rrec --> coreReader
myformat --> ReaderPlugin
state data <<fork>>
NEXT_Player -right> data
data -> NEXT_Databridge
Plugin -> NEXT_GUI : send visualizton data

@enduml
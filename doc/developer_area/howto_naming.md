NEXT namespaces {#howto_naming}
======

[TOC]

# Repository naming

`next_<repo>`

* All repos are namend in lowercase
* All repos begin with a "next_" prefix. The integration repository is an exception to that.

\named{next_sdk} -> core functionality

\named{next_udex} -> data extraction and data management

\named{next_control} -> controlflow

\named{next_player} -> replaying application for open loop

\named{next_bridges} -> output applications to connect to GUI or extract to a file

\named{next_plugins} -> project specific code used by framework

\named{next} -> main integration repository, including interface

# Bricks package naming

`next_<repo>`

* The Bricks packages are always consistent to the repository naming.

# CMake naming

`next_<repo>::next_<package>`

* Each repository is providing 1+ CMake packages
* the namespace of the package is always the repository name.
* the package name is prefixed with "next_". All packages have to be lowercase.

\named{next_sdk::next_sdk}

\named{next_sdk::next_appsupport}

\named{next_udex::next_udex} (this is the current state - should be split up)
	- \named{next_udex::next_publisher}
	- \named{next_udex::next_subscriber}
	- \named{next_udex::next_extractor}
	- \named{next_udex::next_explorer}
	
\named{next_control::next_events}

\named{next_control::next_orchestrator}

\named{next_player::next_player}

\named{next_bridges::next_bridgesdk}

\named{next_bridges::next_controlbridge}

\named{next_bridges::next_databridge}

\named{next_bridges::next_plugins}

\named{next_plugins::next_<pluginname>}

\named{next::next}

# c++ namespaces

`next::<package>[::<function>]`

* All namespaces are in the namespace "::next::"
* For each package, a namespace is generated.
* As an exception, all plugins are combined in the plugins namespace.

\named{next::sdk}
	- \named{next::sdk::logger}
	- ...

\named{next::appsupport}
	- \named{next::appsupport::config}
	- \named{next::appsupport::cmdoptions}
	- ...

\named{next::udex} (this is the current state - should be split up)
	- \named{next::publisher}
	- \named{next::subscriber}
	- \named{next::extractor}
	- \named{next::explorer}
	
\named{next::events}

\named{next::orchestrator}

\named{next::player}

\named{next::bridgesdk}

\named{next::controlbridge}

\named{next::databridge}

\named{next::plugins}

\named{next::plugins::<pluginname>}

# include paths

`next/<package>[/<function>]`

# Binaries

`next_<package>[.*]`

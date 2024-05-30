setup.pytest {#create_test_setup_pytest}
==============

[TOC]

<-- @ref integration_test_run_contest

# Available Setup Files

* setup.pytest
* setup_without_subprocess.pytest

The ``setup.pytest`` file will automatically start all framework executables in a subprocess.<br>
    → the package and the test package from the NEXT repository must be built-in advance.

The ``setup_without_subprocess.pytest`` will not start the executables automatically!<br>
    → executables must be started manually

In each setup file, the web sockets are automatically created and stored in a global parameter.<br>
The global parameters can be called in the tests like this:<br>

Controlbridge<br>
     → get_parameter("controlbridge_websocket")<br>

Databridge<br>
     → get_parameter("databridge_websocket")<br>
     → get_parameter("databridge_broadcast_websocket")<br>

# Setup Functions

The setup.pytest consists of the following functions: <br>

|    Functions    |                               Explanation                              |
|:---------------:|:----------------------------------------------------------------------:|
| global_setup    | Function will be only executed once before the first test case starts  |
| setup           | Function will be executed once before each test case starts            |
| teardown        | Function will be executed once after each test case finishes           |
| global_teardown | Function will be executed once after the last test case ends           |

In the graphic below is a flowchart of the execution order:

<img src="setup_contest_flow_dia.png" width="644">
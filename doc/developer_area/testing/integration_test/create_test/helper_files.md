Helperfiles {#create_test_helper_files}
==============

[TOC]

<-- @ref integration_test_create_test

# Signaltree of the helper_files

``` bash
helper_files
├───ControlBridge
│   └─── controlbridge_Variables.py
├───DataBridge
│   ├─── 2022.02.14_at_11.41.29_radar-mi_663_ext_00.05.01.454.005_00.08.56.193.112.csv
│   ├─── databridge_Variables.py
│   ├─── sdl_generator.py
│   ├─── test_signal.sdl
│   └─── test_signal.csv
├───General
│   ├─── eCal_handler.py
│   ├─── global_Variables.py
│   └─── precondition.py
├───mock_component
│   ├─── mock_component.cpp
│   └─── mock_component.h 
└───Websocket
    └─── websocket_sync.py
```
# Control Bridge

|                                      file                                     |                                                    information                                                    |
|:-----------------------------------------------------------------------------:|:-----------------------------------------------------------------------------------------------------------------:|
| controlbridge_Variables.py                                                    | contains the different variables and functions which are used in the integration test for the controlbridge       |

# Data Bridge

|                                      file                                     |                                                    information                                                    |
|:-----------------------------------------------------------------------------:|:-----------------------------------------------------------------------------------------------------------------:|
| 2022.02.14_at_11.41.29_radar-mi_663_ext_00.05.01.454.005_00.08.56.193.112.csv | containes the the results for the validation in the SWT_VALIDATE_SIGNAL_WITH_RECORDING                            |
| databridge_Variables.py                                                       | contains the different variables and functions which are used in the integration test for the databridge          |
| sdl_generator.py                                                              | creates a sdl file with the initialization function                                                               |
| test_signal.sdl                                                               | contains the signal structure for the SWT_VALIDATE_SIGNAL_WITHOUT_RECORDING                                       |
| test_signal.csv                                                               | contains the the results for the validation in the SWT_VALIDATE_SIGNAL_WITHOUT_RECORDING                          |

# General

|                                      file                                     |                                                    information                                                                  |
|:-----------------------------------------------------------------------------:|:-------------------------------------------------------------------------------------------------------------------------------:|
| eCal_handler.py                                                               | handles the publishing of data with the mock_component                                                                          |
| global_Variables.py                                                           | contains the different variables and functions which are used in the integration test for the controlbridge and the databridge  |
| precondition.py                                                               | contains the precondition which is called in each test                                                                          |

# Websocket

|                                      file                                     |                                                    information                                                    |
|:-----------------------------------------------------------------------------:|:-----------------------------------------------------------------------------------------------------------------:|
| websocket_sync.py                                                            | contains the handler for the communication between ConTest and the databridge, controlbridge                       |
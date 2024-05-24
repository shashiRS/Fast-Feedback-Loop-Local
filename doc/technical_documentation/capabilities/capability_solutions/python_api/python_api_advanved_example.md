Next API Advanced Example {#next_python_api_example_advanced}
===

Available configurations with use-cases:

| Use Case                                             | Link                                                     |
|------------------------------------------------------|----------------------------------------------------------|
| Loading one recording and exporting values into bsig | [Simple Export](@ref next_python_api_example_basic)      |
| Multi stage simulation with simulation Nodes         | [Multi Stage Sim](@ref next_python_api_example_advanced) |
|                                                      |                                                          |

This config file can be used to control the Next Python API.
Values that need to be modified are enclosed by double exclamation marks (!!).

``` json
{
    "SimulationInput": {
        "sim_input": {
            "RecordingLocation": "!!INDEX_RECORDING_FILENAME!!",
            "TimeInterval": {
                "StartTimeUs": !!START_TIME!!,
                "EndTimeUs": !!END_TIME!!,
                "Relative": !!TRUE_FALSE!!
            }
        }
    },
    "SimulationExport": {
        "stage1_export": {
            "ExportSignals": {
                "Signals": [
                    "!!SIGNAL1!!",
                    "!!SIGNAL2!!",
                    "!!SIGNAL3!!"
                ],
                "Trigger": "!!TRIGGER_SIGNAL!!",
                "ExportType": "!!REC_BSIG_CSV!!"
            },
            "Output": {
                "OutputPath": "!!OUTPUT_DATA_PATH!!",
                "OutputName": "!!OUTPUT_FILE_NAME!!",
                "Overwrite": !!TRUE_FALSE!!
            }
        },
        "stage2_export": {
            "ExportSignals": [
                {
                    "Signals": [
                        "!!SIGNAL1!!",
                        "!!SIGNAL2!!",
                        "!!SIGNAL3!!"
                    ],
                    "Trigger": "!!TRIGGER_SIGNAL!!",
                    "ExportType": "!!REC_BSIG_CSV!!",
                    "ExplicitName": "!!ADDITIONAL_NAMING_1!!"
                },
                {
                    "Signals": [
                        "!!SIGNAL1!!",
                        "!!SIGNAL4!!",
                        "!!SIGNAL5!!"
                    ],
                    "Trigger": "!!TRIGGER_SIGNAL!!",
                    "ExportType": "!!REC_BSIG_CSV!!",
                    "ExplicitName": "!!ADDITIONAL_NAMING_2!!"
                }
            ],
            "Output": {
                "OutputPath": "!!OUTPUT_DATA_PATH!!",
                "OutputName": "!!OUTPUT_FILE_NAME!!",
                "Overwrite": !!TRUE_FALSE!!
            }
        }
    },
    "SimulationNodes": {
        "stage1_sim_nodes": {
            "Nodes": {
                "Executable": "ecal_swc_sicoregeneric",
                "ExecutablePath": "!!NEXT_API_BIN_PATH!!",
                "EnvVariables": "LD_LIBRARY_PATH=!!NEXT_LIB_PATH!!",
                "Arguments": "-n"
            }
        },
        "stage2_sim_nodes": {
            "Nodes": {
                "Executable": "ecal_swc_sicoregeneric",
                "ExecutablePath": "!!NEXT_API_BIN_PATH!!",
                "EnvVariables": "LD_LIBRARY_PATH=!!NEXT_LIB_PATH!!",
                "Arguments": "-n"
            }
        }
    },
    "SimulationRun": {
        "stage1_execute_sim": {
            "Setup": {
                "start_nodes": "SimulationNodes",
                "load_backend_config": "!!NEXT_API_BIN_PATH!!"
            },
            "Run": {
                "load_recording": "SimulationInput",
                "start_export": "SimulationExport",
                "play_recording": true,
                "wait_until_end_of_file": !!TIMEOUT!!,
                "stop_export": true
            }
        },
        "stage2_execute_sim": {
            "Setup": {
                "start_nodes": "SimulationNodes",
                "load_backend_config": "!!NEXT_API_BIN_PATH!!"
            },
            "Run": {
                "load_recording": "SimulationInput",
                "start_export": "SimulationExport",
                "play_recording": true,
                "wait_until_end_of_file": !!TIMEOUT!!,
                "stop_export": true
            }
        }
    },
    "SimulationFlows": {
        "stage1": {
            "SimulationInput": "sim_input",
            "SimulationExport": "stage1_export",
            "SimulationRun": "stage1_execute_sim",
            "SimulationNodes": "stage1_sim_nodes",
            "BackendSettings": {
                "BackendDirectory": "!!NEXT_API_BIN_PATH!!",
                "BackendConfig": "!!NEXT_API_BIN_PATH!!"
            }
        },
        "stage2": {
            "SimulationInput": "sim_input",
            "SimulationExport": "stage2_export",
            "SimulationRun": "stage2_execute_sim",
            "SimulationNodes": "stage2_sim_nodes",
            "BackendSettings": {
                "BackendDirectory": "!!NEXT_API_BIN_PATH!!",
                "BackendConfig": "!!NEXT_API_BIN_PATH!!"
            }
        }
    },
    "GlobalSettings": {
        "Logging": {
            "LogFileLevel": "DEBUG",
            "LogConsoleLevel": "DEBUG"
        }
    }
}
``` 

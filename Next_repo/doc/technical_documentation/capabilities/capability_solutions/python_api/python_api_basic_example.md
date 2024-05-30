Next API Basic Example {#next_python_api_example_basic}
===

Available configurations with use-cases:

| Use Case                                             | Link                                                     |
|------------------------------------------------------|----------------------------------------------------------|
| Loading one recording and exporting values into bsig | [Simple Export](@ref next_python_api_example_basic)      |
| Multi stage simulation with simulation Nodes         | [Multi Stage Sim](@ref next_python_api_example_advanced) |
|                                                      |                                                          |

This config file can be used to control the Next Python API.
Paths, Signals and Trigger need to be adapted to match own folder structure.

``` json
{
  "SimulationInput": {
    "radar_mi_663": {
      "RecordingLocation": "!!INDEX_RECORDING_FILENAME!!",
    }
  },
  "SimulationExport": {
    "simpleKPI_fromFirstSIL": {
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
    }
  },
  "SimulationRun": {
    "exportReplayNoSimulation": {
      "Run": {
        "load_recording": "SimulationInput",
        "start_export": "runExport",
        "play_recording": "EOF",
        "wait_until_end_of_file": !!TIMEOUT!!,
        "stop_export": true
      }
    }
  },
  "SimulationFlows": {
    "runKPI": {
      "SimulationInput": "radar_mi_663",
      "SimulationExport": "simpleKPI_fromFirstSIL",
      "SimulationRun": "exportReplayNoSimulation",
      "BackendSettings": {
        "BackendDirectory": "!!NEXT_API_BIN_PATH!!"
      }
    }
  },
  "GlobalSettings": {
    "RootDir": "!!NEXT_API_BIN_PATH!!"
  }
}
```

Next API Config {#next_python_api_config}
===

[TOC]

The **Next Python API** can be controlled via a config file.
This page will show, how the config is build up and how to use the different **Settings**.

The config file is a json type file that contains the configuration for running the Next backend.
The config consists of 5 different **Categories**, that can be set up.
These include:

- SimulationFlows
- SimulationRun
- SimulationInput
- SimulationExport
- SimulationNodes

Each **Category** can contain one or multiple **Entries**. **Entries** are named sections of a **Category** that contain
the specific **Settings** that the API shall use when processing the simulation.

---

# Simulation Flows

The simulation flows define how the simulation will run.
Here you can select any combination of defined setups from the config-categories.

If you provide multiple setups in one category, the provided execution will be run multiple times, to satisfy all
possible combinations.

**Warning:** Combining different setups scales up to a lot of runs. (2 different configs for all 4 categories create
2^4 = 16 runs)

The execution of the runs that is handled via the config can also be used in your code when importing the Next API as a
python module.
More information on this can be found [here (TBD)](@ref next_python_api_module)

## Simulation Input

Select, which SimulationInput configuration shall be used. If multiple are given they will be executed in separate runs
one after another.

## SimulationExport

Select, which SimulationExport configuration shall be used. If multiple are given they will be executed in separate runs
one after another.

## SimulationNodes

Select, which SimulationNodes configuration shall be used. If multiple are given they will be executed in separate runs
one after another.

## SimulationRun

Select, which SimulationRun shall be used. If multiple are given they will be executed in separate runs one after
another.

## Backend Settings

### Ports

#### Controlbridge

| Key           | Type | Mandatory | Description                                           | Default | 
|---------------|------|-----------|-------------------------------------------------------|---------|
| Controlbridge | int  | no        | Websocket port to communicate with the controlbridge. | 8200    |

#### Databridge

| Key        | Type | Mandatory | Description                                        | Default | 
|------------|------|-----------|----------------------------------------------------|---------|
| Databridge | int  | no        | Websocket port to communicate with the databridge. | 8080    |

#### Databridge Broadcast

| Key                 | Type | Mandatory | Description                                                            | Default | 
|---------------------|------|-----------|------------------------------------------------------------------------|---------|
| DatabridgeBroadcast | int  | no        | Websocket port to communicate with the databridge (receiving channel). | 8082    |

### eCal Network

| Key         | Type   | Mandatory | Description                                         | Default           | 
|-------------|--------|-----------|-----------------------------------------------------|-------------------|
| EcalNetwork | String | no        | Defines the name of the network that shall be used. | "ecal_monitoring" |

### Backend Directory

| Key              | Type   | Mandatory | Description                                                                       | 
|------------------|--------|-----------|-----------------------------------------------------------------------------------|
| BackendDirectory | String | yes       | Folder where the next executables are located (controlbridge, databridge, player) |

### Backend Config

| Key           | Type   | Mandatory | Description                                          | 
|---------------|--------|-----------|------------------------------------------------------|
| BackendConfig | String | no        | Config file that shall be used for the Next backend. |

---

# Simulation Run

Simulation run provide a step-by-step instruction on how the system behaves.

There are some predefined **Executions** like play_recording, reset_nodes, etc. (Full list
at [Available Executions](@ref next_python_api_available_executions)), and custom executions can be added (@ref
next_python_api_executions <- TBD).

A **Simulation Run** consist of the **Phases** _startup_, _run_ and _teardown_.
All **Executions** can be selected in any of these **Phases**.

The commands that are mapped to an execution can also be used when importing the API as a python module.

## Startup

All Commands that will be executed once in the beginning of the flow.

## Run

All commands that will be executed for each batch of recordings.

## Teardown

All commands that will be executed once at the end of the flow.

### Available Executions {#next_python_api_available_executions}

Supported executions can be found in next_executions.py.
Currently available are:

| command                | arguments                          |
|------------------------|------------------------------------|
| load_recording         | run_input                          |
| play_recording         | - (use 'true' to match the syntax) |
| wait_until_end_of_file | timeout                            |
| pause_recording        | - (use 'true' to match the syntax) |
| jump_to_beginning      | - (use 'true' to match the syntax) |
| start_exporter         | run_export                         |
| stop_exporter          | - (use 'true' to match the syntax) |
| jump_to_timestamp      | (int) timestamp in ms              |
| close_recording        | - (use 'true' to match the syntax) |
| check_signals          | List of signals                    |

### Custom Executions

Custom code can be added in _src/py_api/custom_executions/custom_executions.py_. In this file a custom class can be
created, that derives from **ExecutionStep**.
This class should have 3 functions:

- **init**: The init should use the init of execution step and give a string as additional argument.
  This string will be used in the config file to call the execution. Example:

```python
    def __init__(self):


ExecutionStep.__init__(self, "custom_log_if_connected")
```

- **invoke_step**: This function will be calles in the actual run. Here the custom code can be written
- **invoke_dryrun**: This function will be called in the dryrun. Here it can be printed, what arguments the function
  gets.
  This function is optional. If this function is not used, the parent class function will be used and print the name of
  the class.

Both invoke_dryrun and invoke_step need to have the following function arguments

```python 
(self, args, recording_batch, sim_nodes, export_settings, run_input_settings)
```

- args: This value contains the arguments from the config file (single value or list possible)
- recording_batch: This value contains the recording that is currently selected for the simulation
- sim_nodes: This value contains the simulation nodes settings that are currently selected for the simulation
- export_settings: This value contains the export_settings that are currently used in the simulation
- run_input_settings: This value contain the SimulationInput settings that are currently used in the simulation

---

# Simulation Input

In the SimulationInput option you can define the recordings that shall be used in the simulation run.
The SimulationInput stores all Input settings for your runs.
Each input setting can be configured with a unique name, that will be used when a flow is defined later in the config.
The input settings contain the following fields:

## Recording Location

| Key               | Type              | Mandatory | Description                                                            | Supported Values                 |
|-------------------|-------------------|-----------|------------------------------------------------------------------------|----------------------------------|
| RecordingLocation | (Array of) String | yes       | The recording location defines which files shall be used during a run. | a single filename                |
|                   |                   |           |                                                                        | a list of filenames              |
|                   |                   |           |                                                                        | a single folder                  |
|                   |                   |           |                                                                        | a list of folders                |
|                   |                   |           |                                                                        | any combination of these options |

By default, all given files will be loaded in parallel, meaning all signals from all files will be available at once.
If you want to have the files run one after the other, you need to specify this with the "split" option.

## Split

| Key   | Type   | Mandatory | Description                                                                                                                    | Supported Values   | Explanation                                                                          |
|-------|--------|-----------|--------------------------------------------------------------------------------------------------------------------------------|--------------------|--------------------------------------------------------------------------------------|
| Split | String | no        | When you provide a recording location that contains multiple recordings, <br/>you can specify how these files shall be loaded. | no_split (Default) | all files loaded in parallel                                                         |
|       |        |           |                                                                                                                                | files              | all given files loaded one after another                                             |
|       |        |           |                                                                                                                                | folder             | all files of a folder loaded in parallel,<br/>all folders loaded one after the other |

## Recording Type

| Key           | Type              | Mandatory | Description                                                                       | Supported Values    | Explanation                   |
|---------------|-------------------|-----------|-----------------------------------------------------------------------------------|---------------------|-------------------------------|
| RecordingType | (Array of) String | no        | When a folder is provided as input, this option defines which files shall be used | .* (Default )       | all files loaded              |
|               |                   |           |                                                                                   | single file type    | files with given type loaded  |
|               |                   |           |                                                                                   | array of file types | files with given types loaded |

When you only provide a single files this option will be ignored.

## Time Interval

| Key         | Type | Mandatory | Description                                                                                     | 
|-------------|------|-----------|-------------------------------------------------------------------------------------------------|
| StartTimeUs | int  | no        | Specify, from which point the recording shall be started.                                       |
| EndTimeUs   | int  | no        | Specify, until which point the recording shall be played.                                       | 
| Relative    | bool | no        | If True, the timestamp will be handled as relative paths. Default is false (Absolute Timestamp) |

## Simulation Input File

| Key                 | Type   | Mandatory                           | Description                                                               | Supported Values | 
|---------------------|--------|-------------------------------------|---------------------------------------------------------------------------|------------------|
| SimulationInputFile | String | no                                  | A file containing all input batches and time intervals that shall be use. | Valid path       |
|                     |        | yes (if no signal list is provided) |                                                                           |                  |

---

# Simulation Export

In the SimulationExport options you can set up, how the export shall work and where the files will be stored.

## Export Signals

### Signals

| Key     | Type            | Mandatory                           | Description                                          | Supported Values | 
|---------|-----------------|-------------------------------------|------------------------------------------------------|------------------|
| Signals | Array of String | no                                  | The list of all signals that the Exporter shall use. | array of signals |
|         |                 | yes (if no signal file is provided) |                                                      |                  |

### Trigger

| Key     | Type   | Mandatory                           | Description                                                    | Supported Values | 
|---------|--------|-------------------------------------|----------------------------------------------------------------|------------------|
| Trigger | String | no                                  | The Signal that shall be used as the trigger for the Exporter. | Single signals   |
|         |        | yes (if no signal file is provided) |                                                                |                  |

### Simulation Export File

| Key                  | Type   | Mandatory                           | Description                                                          | Supported Values | 
|----------------------|--------|-------------------------------------|----------------------------------------------------------------------|------------------|
| SimulationExportFile | String | no                                  | A file containing all signals and trigger that the Export shall use. | Valid path       |
|                      |        | yes (if no signal list is provided) |                                                                      |                  |

### Export Type

| Key        | Type              | Mandatory | Description                                                                  | Supported Values | 
|------------|-------------------|-----------|------------------------------------------------------------------------------|------------------|
| ExportType | (Array of) String | yes       | Define the file type that the exporter shall use to store the exported data. | .csv             |
|            |                   |           |                                                                              | .bsig            |
|            |                   |           |                                                                              | .rec             |

If multiple extensions are given a separate exporter will be started for each export type.

### Explicit Name

| Key          | Type   | Mandatory | Description                                                                                                      | Supported Values                 | 
|--------------|--------|-----------|------------------------------------------------------------------------------------------------------------------|----------------------------------|
| ExplicitName | String | no        | If this option is set, each exported file will get this additional string added at the end of the exported file. | String with predefined variables |

More information on the naming at the section [naming the export](@ref next_python_api_export_naming).

## Output

### Output Path

| Key        | Type   | Mandatory | Description                                        | Supported Values | 
|------------|--------|-----------|----------------------------------------------------|------------------|
| OutputPath | String | yes       | Location where the exported files shall be stored. | Valid path       |

If you want to store the exports in separate folders, you can define this in the output name option.

### Output Name

| Key        | Type   | Mandatory | Description                                                                                                               | Supported Values                 | 
|------------|--------|-----------|---------------------------------------------------------------------------------------------------------------------------|----------------------------------|
| OutputName | String | no        | Name that the output files shall have. If they should be split into extra folders, you can separate the folders with '/'. | String with predefined variables |

More information on the naming at the section [naming the export](@ref next_python_api_export_naming).

### Overwrite

| Key       | Type | Mandatory | Description                                                                                               | 
|-----------|------|-----------|-----------------------------------------------------------------------------------------------------------|
| Overwrite | bool | no        | When this option is set to true, the export will overwrite files with the same given namen in the folder. |

## Naming the export {#next_python_api_export_naming}

The name is concatenated of the raw string that is given plus some predefined variables.
When a forward-slash is given in the output name, a new folder will be created with this name.

Predefined Variables are:

| Variable           | Description                                                                                          |
|--------------------|------------------------------------------------------------------------------------------------------|
| $INPUT$            | take the same name that the recording was used                                                       |
| $RUN_INPUT$        | Uses the selected run_input names for the export name                                                |
| $RUN_EXPORT$       | Uses the selected run_export names for the export name                                               |
| $SIM_NODES$        | Uses the selected sim_nodes names for the export name                                                |
| $EXECUTIONS$       | Uses the selected executions names for the export name                                               |
| $FLOW$             | Name of the flow                                                                                     |
| $FLOW_COMBINATION$ | combines the names of used run_input, run_export, sim_nodes and execution                            |
| $FLOW_INDEX$       | Number of flows that are executed                                                                    |
| $RUN_INDEX$        | Counts up after one run (could be multiple times in one execution. E.g. Multiple files after another |
| $EXECUTION_INDEX$  | Counts up after one full execution (one combination of settings)                                     |
| $FILE_INDEX$       | Counts up if the file exists already                                                                 |
| $DATE$             | adds date of simulation run                                                                          |
| $TIME$             | adds time of simulation run                                                                          |

### Examples

**Example 1: Simple Single Simulation**

One recording is replayed and stored in one file given at output_path+output_name.

| Selected Flow                               | Output Options                                      |
|---------------------------------------------|-----------------------------------------------------|
| SimulationNodes: "si_core"                  | OutputPath: "D:/exports"                            |
| SimulationInput: "parking_scenario_dryroad" | OutputName: "$FLOW_COMBINATION$/cem_outputs_$DATE$" |
| SimulationExport: "all_in_one"              |                                                     |
| SimulationRun: "simple_replay"              |                                                     |

**Exported Files:**

```
D:/exports/parking_scenario_dryroad_all_in_one_sicore_simple_replay/cem_outputs_20231026
```

**Example 2: Compare Versions with Multiple Scenarios**

Create exports for different software versions over multiple scenarios.
All Exports are sorted by software versions.

| Selected Flow                                                               | Output Options                                                    |
|-----------------------------------------------------------------------------|-------------------------------------------------------------------|
| SimulationNodes: ["si_core_0.1.2", "si_core_0.2.0"]                         | OutputPath: "D:/exports"                                          |
| SimulationInput: <br/>["parking_scenario_dryroad", "parking_scenario_snow"] | OutputName: "$SIM_NODES$/$RUN_INPUT$\_kpi_measures\_$FILE_COUNT$" |
| SimulationExport: "ExportCycleSeparate"                                     |                                                                   |
| SimulationRun: "simple_replay"                                              |                                                                   |

**Exported Files:**

```
D:/exports/si_core_0.1.2/parking_scenario_dryroad_kpi_measures
D:/exports/si_core_0.1.2/parking_scenario_dryroad_kpi_measures_1
D:/exports/si_core_0.1.2/parking_scenario_dryroad_kpi_measures_2
D:/exports/si_core_0.1.2/parking_scenario_snow_kpi_measures

D:/exports/si_core_0.2.0/parking_scenario_dryroad_kpi_measures
D:/exports/si_core_0.2.0/parking_scenario_dryroad_kpi_measures_1
D:/exports/si_core_0.2.0/parking_scenario_dryroad_kpi_measures_2
D:/exports/si_core_0.2.0/parking_scenario_snow_kpi_measures
```

**Example 3: Compare different Versions on different Scenarios ordered by Execution**

Run Input "parking_scenario_dryroad" contains 3 files, that are loaded with "split=file" option.

Filenames: Memmingen_dryroad_crash, Memmingen_dryroad_sharpCorner, Memmingen_dryroad_high_speed

| Selected Flow                                                               | Output Options                                                               |
|-----------------------------------------------------------------------------|------------------------------------------------------------------------------|
| SimulationNodes: ["si_core_0.1.2", "si_core_0.2.0"]                         | OutputPath: "D:/exports"                                                     |
| SimulationInput: <br/>["parking_scenario_dryroad", "parking_scenario_snow"] | OutputName: "kpi_measures_$EXECUTION_INDEX$/$SIM_NODES$/$INPUT$_$RUN_INDEX$" |
| SimulationExport: "all_in_one"                                              |                                                                              |
| SimulationRun: "simple_replay"                                              |                                                                              |

**Exported Files:**

```
D:/exports/kpi_measures_0/si_core_0.1.2/Memmingen_dryroad_crash_0
D:/exports/kpi_measures_0/si_core_0.1.2/Memmingen_dryroad_sharpCorner_1
D:/exports/kpi_measures_0/si_core_0.1.2/Memmingen_dryroad_high_speed_2

D:/exports/kpi_measures_1/si_core_0.2.0/Memmingen_dryroad_crash_0
D:/exports/kpi_measures_1/si_core_0.2.0/Memmingen_dryroad_sharpCorner_1
D:/exports/kpi_measures_1/si_core_0.2.0/Memmingen_dryroad_high_speed_2

D:/exports/kpi_measures_2/si_core_0.1.2/Memmingen_snow_break_0

D:/exports/kpi_measures_3/si_core_0.2.0/Memmingen_snow_break_0
```

**Example 4: KPI Comparison per recording**

This enables easy bsig comparison in each folder.
Explicit name is given as array to show, that multiple export_signals are provided.
Different Exports are split into different folders for easy comparison of software versions.

| Selected Flow                                         | Output Options                                                    |
|-------------------------------------------------------|-------------------------------------------------------------------|
| SimulationNodes: ["full_system_v1", "full_system_v2"] | OutputPath: "D:/exports"                                          |
| SimulationInput: "parking_scenario_dryroad"           | OutputName: "$RUN_INPUT$/"                                        |
| SimulationExport: "all_in_one"                        | ExplicitName: ["raw_cycle/$SIM_NODE$", "driver_cycle/$SIM_NODE$"] |
| SimulationRun: "simple_replay"                        |                                                                   |

**Exported Files:**

```
D:/exports/parking_scenario_dryroad/raw_cycle/full_system_v1.bsig
D:/exports/parking_scenario_dryroad/raw_cycle/full_system_v2.bsig

D:/exports/parking_scenario_dryroad/driver_cycle/full_system_v1.bsig
D:/exports/parking_scenario_dryroad/driver_cycle/full_system_v2.bsig
```

---

# Simulation Nodes

## Nodes

### Executable

| Key        | Type   | Mandatory | Description                                   | 
|------------|--------|-----------|-----------------------------------------------|
| Executable | String | no        | Name of the executable that shall be started. |

### Executable Path

| Key            | Type   | Mandatory | Description                                  | 
|----------------|--------|-----------|----------------------------------------------|
| ExecutablePath | String | no        | Path to the executable that shall be started |

### Arguments

| Key       | Type   | Mandatory | Description                                | Default | 
|-----------|--------|-----------|--------------------------------------------|---------|
| Arguments | String | no        | Command-line arguments for the executable. | "-n"    |

### Environment Variables

| Key          | Type             | Mandatory | Description                                                                     | Example                                    |
|--------------|------------------|-----------|---------------------------------------------------------------------------------|--------------------------------------------|
| EnvVariables | (List of) string | no        | Can be used to set environment variables. Useful if lib path is needed on Linux | "EnvVariables":"LD_LIBRARY_PATH="./../lib" |

## Node Config

| Key        | Type   | Mandatory | Description                                           | 
|------------|--------|-----------|-------------------------------------------------------|
| NodeConfig | string | no        | Config file that shall be loaded with the executable. |

---

# Example Configurations

Available configurations with use-cases:

| Use Case                                             | Link                                                     |
|------------------------------------------------------|----------------------------------------------------------|
| Loading one recording and exporting values into bsig | [Simple Export](@ref next_python_api_example_basic)      |
| Multi stage simulation with simulation Nodes         | [Multi Stage Sim](@ref next_python_api_example_advanced) |
|                                                      |                                                          |

---

# Command-Line Arguments

Setting up the simulation runs can also be done via command-line arguments.

Available arguments are:

* **-c** : provide a config file that shall be used. By default, all Flows defined in the config will be executed.
* **-f** : Select which flow(s) shall be executed. Only the provided flow(s) will be executed.<br>
  The value should be a string (CASE SENSITIVE) and can be a single name, a regular expression
  or a list of names/regular expressions (the separator is empty space " ").<br>
  It's using the regular expression to match the selected flows (-f) and config SimulationFlow entries <br>
  e.g. simulationFlow entry = ["example", "some_example", "example_the_second"]<br>
    1) -f "example" -> execute: ["example", "some_example", "example_the_second"]<br>
    2) -f "^example" -> execute: ["example", "example_the_second"], not ["some_example"]<br>
    3) -f "example$" -> execute: ["example", "some_example"], not ["example_the_second"]<br>
    4) -f "^example$" -> execute: ["example"], not ["some_example", "example_the_second"]<br>
    5) -f "^example$ some_example" -> execute: ["example", "some_example"], not ["example_the_second"]<br>
* **-rf** : Select which flow(s) from the config file (provided by the -c option) shall be EXCLUDED from execution.<br>
  If executed_flows (-f) argument is present,
  first will select the flow(s) that will be executed and after will exclude the flows from current command (-rf)<br>
  Only the provided flow(s) will be excluded from flow(s) execution list <br>
  The value should be a string (CASE SENSITIVE) and can be a single name, a regular expression
  or a list of names/regular expressions the separator will be an empty space (' ')<br>
  It's using the regular expression to match the selected flows (-f) and config SimulationFlow entries <br>
  e.g. simulationFlow entry = ["run_kpi", "kpi_tests", "test_kpi_duplicate"]<br>
    1) -rf "kpi" -> execute: [] (no execution)<br>
    2) -rf "^kpi" -> execute: ["run_kpi", "test_kpi_duplicate"], not ["kpi_tests"]<br>
    3) -rf "kpi$" -> execute: ["kpi_tests", "test_kpi_duplicate"], not ["run_kpi"]<br>
    4) -rf "^kpi$" -> execute: ["run_kpi", "kpi_tests", "test_kpi_duplicate"]<br>
    5) -rf "^run test_kpi_duplicate" -> execute: ["kpi_tests"], not ["run_kpi", "test_kpi_duplicate"]<br>
* **-i** : Enter a path of Simulation Input configuration file.
  The provided configuration file will overwrite all Simulation Input provided in the config file provided by the -c
  option.
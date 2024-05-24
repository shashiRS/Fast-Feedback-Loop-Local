Signal Exporter Plugin Overview {#plugins_system_signal_exporter}
=====

[TOC]

# Overview
The Exporter plugin allows users to select multiple signals and write their values to a file based on a trigger signal. A detailed tutorial on how to use this feature can be found here: @ref howto_export_algo_data.

# Supported Formats
The file format options include CSV or Bsig.

# Signal List
The signal list is where users define which signals are to be exported.

# Trigger
The trigger is the event used to initiate data writing for each data sample, **for all the signals**.

## Signal
The trigger signal can be selected out of the signal list. Whenever this specific signal is received, a data sample is written to the file for each of the signals. 

> **Note**: If this trigger signal is more frequent than the other signals, duplicate values will be written. If the trigger signal is less frequent, values for other signals will be skipped, only the last received value will be written.

## Time
TODO

## Cycle
TODO

## Port
TODO

# Output File
Users can choose a folder for export, and the default file naming convention will be applied.

- Default filename: `exported_file__$DATE$_$TIME$` -> `exported_file__DYYYY_MM_DD_THH_MM_SS.ext`

Users can also select an existing file, and the same name will be used for the output according to the selected format. Additionally, users have the option to add or edit the filename.

For more flexibility in naming, the following predefined variables can be used:

- `$INPUT$` - Takes the same name as the recording.
- `$TIME$` -  Adds the time of the simulation run (based on GMT).
- `$DATE$` - Adds the date of the simulation run (based on GMT).
- `$FILE_INDEX$` - Counts up if the file already exists.
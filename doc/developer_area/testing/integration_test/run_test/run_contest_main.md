Run ConTest {#integration_test_run_contest}
==============

[TOC]

* @ref integration_test_main
* @ref setup_test_environment
* @ref integration_test_create_test
 
# On Windows:

Run the following commands in the command prompt to start ConTest:

```cmd
cd <contest_release_folder>
C:\LegacyApp\Python36\python.exe main.py
```
# On Ubuntu:

Run the following commands in the command prompt to start ConTest:

```cmd
cd <contest_release_folder>
python main.py
```

# Run Tests

<img src="contest.png" width="644">

## Select and Run

Select the tests which should run.<br>
In some tests you can select parameter (SWT_REC_STEP_FORWARD_AFTER_LOAD_PAYLOAD_TEST), by default all parameter will run each at a time.<br>
With a right click you can open the parameter view and select the parameter which should run.

## Used Setup

In the Next repository two @ref create_test_setup_pytest files are available.<br>
To select the setup you can go to the setup.pytest tab in the menu bar.

## Filter Tags

Here you can search for tests with the defined tags in the tests.

## Run multiple times

Here you can select how often the selected test should run, to control that for example the test have no temporary errors.

## Execution parameters

Here you can randomize the test, to control, that the tests are not dependent on each other.

## Output

After the run, ConTest creates reports in four different formats:

* JSON
* HTML
* TXT
* XML

The reports will be created in the folder mentioned in the config.<br>
To access the reports you can go to the Reports tab in the menu bar.<br>

# Debug with ConTest

Select the tests which should run.<br>
Setup the system as described here: @ref integration_test_run_contest_debug
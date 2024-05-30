Integration Test {#integration_test_main}
==============

[TOC]

* @ref setup_test_environment
* @ref integration_test_create_test
* @ref integration_test_run_contest

# BLACK-BOX testing

# Description

The integration tests are for testing the functionality between the **Control Bridge**, **Data Bridge** and the **Next Player** with the **Next GUI**.<br>
The test mocks the **Next GUI** by exchanging messages with the **Control Bridge**, the **Data Brige** and checks if the responses are correct.<br>
[ConTest](https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.2.0/index.html) is used as the testing framework.<br>

# Pre-requisites

Download the ConTest framework from the following site and follow the installation guide:<br>
<https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.2.0/installations.html><br>
<br>
**in the same environment** where the main ConTest GUI will be run, to make sure the test suite has all the dependencies.

# Setup

Launch the ConTest GUI to setup the Next Integration tests. Typically this is done by running the command <i>python main.py</i> in the ConTest main folder.<br>
In the ConTest GUI, click <i>Menu -> Create Config</i> or use the shortcut <i>Ctrl+N</i> to create a new config file.<br>
In the <i>Create ConTest Configuration</i> popup, enter the following paths:<br>
  * **Base Path:**  <i><Next>/tests</i>
  * **Python Tests:** <i><Next>/tests/integration</i>
  * **Report Path:** <i><Next>/tests/Report</i>
<br>
Click <i>Create</i> and save it preferably to <i><Next>/tests/cfg</i>.<br>
Close the configuration window to go back to the main ConTest GUI. Here select <i>Menu -> Load Config</i> or use the shortcut <i>Ctrl+L</i>, and select the configuration file you just created. It's an <i>.ini</i> file.<br>
After this, the tests are available in the main window, ready to be run.<br>
They can be enabled or disabled via the checkbox in front of the test.<br>
If you save the config while tests are enabled, this information will also be added to the config and the tests will be selected once the config is loaded again.<br>


# Run Tests

In the ConTest GUI, select the tests which should run.<br>
In some tests, you can select parameters (e.g. SWT_REC_STEP_FORWARD_AFTER_LOAD_PAYLOAD_TEST).<br>
By default all parameters will run each at a time.<br>
With a right click you can open the parameter view and select the parameter which should run.<br>
On the home view you can select if the test should run multiple times.<br>
To run the tests in a randomized execution order, select the last option.
<br>
For more information, refer to the **ConTest** documentation.<br>
<https://cip-docs.cmo.conti.de/ConTest/latest>

## Recording location

Some tests require a recording for execution.<br>
The location can be defined in two ways:<br>
First we check for the environment variable: NEXT_CONTEST_RECORDING_PATH<br>
If not available we expect the recording to be located in the <i><Next>/tests/Recordings</i> folder.<br>
The used recording in the tests which you need to include there can be found here: <br>
\\\cw01.contiwan.com\Root\Loc\ulm2\didt6448\Recordings\sim_team_share\Next Feature Integration Test\Automated Integration Test<br>
<br>
For Jenkins integration, this needs to be adapted.<br>
<br>
The tests use the [global_Variables.py](@ref create_test_helper_files) for specifying the recording .<br>

# Reports

Every test run generates a report under the folder <i><Next>/tests/Report</i>.

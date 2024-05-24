# Integration Test

## BLACK-BOX testing

## Description

The integration tests are for testing the functionality between the **Control Bridge**, **Data Bridge** and the **Next Player** with the **Next GUI**.

The test mocks up the **Next GUI** by exchanging messages with the **Control Bridge**, the **Data Brige** and check if the responses are correct.

[ConTest](https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.2.0/index.html) is used as the testing environment.


## Pre-requisites

Download the ConTest framework from the following site and follow the installation guide:

<https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.2.0/installations.html>


**in the same environment** where the main ConTest GUI will be run, to make sure the test suite has all the dependencies.

> !! TODO: fix this step for CI/CD: [ConTest Bricks](https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.2.0/ci_integration/bricks.html) !!

## Setup

Launch the ConTest GUI to setup the Next Integration tests. Typically this is done by running the command `python main.py` in the ConTest main folder.

In the ConTest GUI, click `Menu -> Create Config` or in Windows `Ctrl+N` to create a new config file.

In the `Create ConTest Configuration` popup, enter the following paths:

* **Base Path:** `<Next>/tests`
* **Python Tests:** `<Next>/tests/integration`
* **Report Path:** `<Next>/tests/Report`

Click `Create` and save it preferably to `<Next>/tests/cfg`.

Finally, back in the main ConTest GUI, select `Menu -> Load Config` or `Ctrl+L` in Windows, and select the configuration file you just created. It's an `.ini` file.

After this, the tests are available as checkboxes, ready to be run.


## Run Tests

* In the ConTest GUI, select the tests which should run
* In some tests, you can select parameters (e.g. SWT_REC_STEP_FORWARD_AFTER_LOAD_PAYLOAD_TEST) .
  By default all parameters will run each at a time.
  With a right click you can open the parameter view and select the parameter which should run.
* On the home view you can select if the test should run multiple times
* To run the tests in a randomized execution order, select the last option

For more information, refer to the **ConTest** documentation.

### Recording location

The tests require a recording for specific tests.

For now, this is located in the `<Next>/tests/Recordings` folder.

For Jenkins integration, this needs to be adapted.

The tests use the [global_Variables.py](integration/helper_files/global_Variables.py#16) for specifying the test file.


## Reports

Every test run generates a report under the folder `<Next>/tests/Report`.


## Testcases disabled
|     Bridge    |      Test             |                   Testcase               |          OS         |                                                    reason disabled                                                    |
|:-------------:|:---------------------:|:----------------------------------------:|:-------------------:|:---------------------------------------------------------------------------------------------------------------------:|
| controlbridge | swt_end_of_file       | SWT_END_OF_FILE_PAYLOAD_TEST             | [x] WS<br>[x] Linux | the endTimeStamp is not equal to the currentTimeStamp from the<br>PlaybackIsPaused message after reaching end of file |
| controlbridge | swt_playing_recording | SWT_REC_PLAYING_PAYLOAD_TEST             | [x] WS<br>[x] Linux | The currentTimeStamp is not between startTimeStamp and <br>endTimeStamp of the event PlaybackIsPlaying                |
| databridge    | swt_update_signals    | SWT_PAYLOAD_UPDATE_BUTTON_WITH_RECORDING | [] WS<br>[x] Linux  | the CAN date is missing --> error in mts_core_lib                                                                     |

## Responsible
* @uidr9189 <jannik.fritz@continental-corporation.com>
* @uie80129 <achim.groner@continental-corporation.com>
* @uif41320 <fabian.janker@continental-corporation.com>

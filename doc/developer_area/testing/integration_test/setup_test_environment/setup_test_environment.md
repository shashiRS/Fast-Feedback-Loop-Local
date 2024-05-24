Setup Test Environment {#setup_test_environment}
==============

[TOC]

* @ref integration_test_main
* @ref integration_test_create_test
* @ref integration_test_run_contest


# Prerequisites

Python 3.6 or Python 3.9 has been installed on the machine<br>

# Releases ConTest

Download the latest release version from the following side:

* via Artifactory: <https://eu.artifactory.conti.de/artifactory/c_adas_astt_generic_prod_eu_l/ConTest/>

The Release Notes can be found on Github: <br>
* <https://cip-docs.cmo.conti.de/static/docfiles/ConTest/v2.1.0/release/latest>

# On Windows:

Run the following commands in the command prompt to install the required Python modules
```cmd
cd <contest_release_folder>
install_pip_user_dependencies.bat C:\LegacyApp\Python36
```

# On Linux:

Run the following commands in the terminal to install required Python modules

```cmd
cd <contest_release_folder>
./install_pip_user_dependencies.sh
```

# Additionally Packages:

## websocket-client:

Run the following command to install the websocket-client package

```cmd
py -m pip install websocket-client
```

## eCal

Download from the following web page the eCal whl file: <https://eclipse-ecal.github.io/ecal/_download_archive/download_archive_ecal_5_10_0.html><br>
For Windows: ecal5-5.10.0-win_amd64.whl<br>
For Linux: ecal5-1focal-cp38-cp38-linux_x86_64.whl<br>
<br>
Run the following command to install the eCal5 package<br>
<br>
Windows:
```cmd
cd ~/Download
py -m pip install ecal5-5.10.0-win_amd64.whl
```
<br>
Linux:
```cmd
cd ~/Download
pip install ecal5-1focal-cp38-cp38-linux_x86_64.whl
```

# Configure ConTest

Download and build the latest master from Next repository.<br>
Use Bricks to build the test package for the integration tests:<br>

```cmd
py -3.6 scripts/cip.py build --platform vs2017 --test-type GENERIC
```

Start ConTest as explained in @ref integration_test_run_contest.<br>
Create a new config-file by pressing <i>Ctrl+N</i> or from the menu bar <i>Menu → Create Config</i>.<br>
Enter following paths:
  * **Base Path:**  <i><Next>/tests</i>
  * **Python Tests:** <i><Next>/tests/integration</i>
  * **Report Path:** <i><Next>/tests/Report</i>
Save the config-file in the cfg folder inside the tests folder.<br>

<img src="contest_setup.png" width="644">

# Recording

The used recording in the tests can be found here: 

\\\cw01.contiwan.com\Root\Loc\ulm2\didt6448\Recordings\sim_team_share\Next Feature Integration Test\Automated Integration Test

# Setup Visual Studio Code 

Information on how to setup VS Code for the codestyle and the file type is shown on the page: @ref setup_environment_vscode.
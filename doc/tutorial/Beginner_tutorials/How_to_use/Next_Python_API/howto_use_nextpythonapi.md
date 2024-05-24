How to use NEXT-Python API {#howto_use_python_api}
=====

---

# Overview {#next_python_api_overview}

This tutorial will show step by step how to use <B>Next Python API</B>.

---

# Steps {#howto_use_python_api_steps}

## 1. Prepare the environment
The **Next Python API** requires **Python 3.7 or higher** to function properly.
The Next Repository contains all needed scripts to use the python API.
After building the Next Bricks package a folder containing those scripts will be available at: 
_next\conan_workarea\package.next.generic.0.0.0-fallback.vs2017\next_python_

This also includes the installation scripts.
On Windows, use the _install.bat_.
On Linux, use the _install.sh_.

Those scripts will create a virtual environment and install needed packages there.
Afterwards, the wheel package containing the Python API will be available in the _dist_ folder.

Hint: The created virtual environment does not have the Python API installed yet. If you want to use it you need to install it with
venv\Scripts\python.exe -m pip install dist\next_api-0.1.0-py3-none-any.whl

Currently, it is not possible to include the eCal package in the Python API wheel package.
This package needs to be installed as well into your python environment.


The eCal releases are located here: https://eclipse-ecal.github.io/ecal/_download_archive/download_archive.html#download-archive.<br>
After selecting the version that you want to use, you find the python wheel packages in the **Download section**. 
Select the file that is matching your OS and Python version.
The wheel file can then be installed with **pip**.

## 2. Set up the configuration
When you want to use the Next Python API in **stand-alone mode**, you need to set up the config in a proper manner.
To do this, take a look at this page: @ref next_python_api_config

## 3. Start Next API run via Command Lines

### Start the Simulation

After creating a configuration file, the API can be started via the command-line with this command:
```shell
py next_api.py -c config_file.json
```
This will run the full simulation as it is configured in the config file.

### Dryrun

To verify, that the configuration has the right values and steps, it is also possible to just start the Python API in a dryrun.
During the dryrun, the API is not starting any components for the simulation. It only prints the commands and arguments that would be used.
The command for the dryrun is:
```shell
py next_api.py -c config_file.json --dryrun
```



---

# Tips and known problems {#next_python_api_tips}


---

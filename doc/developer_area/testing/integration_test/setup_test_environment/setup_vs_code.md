Setup VScode for creating tests {#setup_environment_vscode}
==============

[TOC]

<-- @ref setup_test_environment

# Associations

* open the settings of Visual Studio Code and search for:<br>
    Files:Associations<br>

* add a new item called:<br>
    Item: *.pytest<br>
    Value: python<br>

<img src="vs_code_setup.png" width="644">

# Environment File
create a .env file in your workspace<br>
content of the .env file:<br>
    PYTHONPATH={package_folder}/{revision}-ConTest

# Code-Style

## Linter {#setup_linter}

To enable Linter, open the command palette of Visual Studio Code **(Ctrl+Shift+P)** and type:

1. Python: Enable/Disable Linting
2. Enable

## Pylint

To enable and install Pylint, open the command palette of Visual Studio Code **(Ctrl+Shift+P)** and type:

1. Python: Select Linter
2. pylint

If pylint is not installed a pop up window will appear.

## Pycodestyle (pep8)

To enable and install pycodestyle, open the command palette of Visual Studio Code **(Ctrl+Shift+P)** and type:<br>

1. Python: Select Linter
2. pycodestyle

If pycodestyle is not installed a pop up window will appear.

# settings.json

After the steps befor a .vscode folder will be available in the workspace with a settings.json file in it.<br>
The settings.json for lint should look like in the image below:

<img src="vs_code_json_file.png" width="644">
#!/bin/sh
cd "$(dirname "$0")" || exit

python3 -m venv venv
venv/bin/python -m pip install pyyaml
venv/bin/python -m pip install wheel
venv/bin/python install/wheel_installation_setup.py -i install/next_py_api_wheel_info.yml
venv/bin/python install/wheel_installation_setup.py -i install/next_py_api_linux_wheel_info.yml

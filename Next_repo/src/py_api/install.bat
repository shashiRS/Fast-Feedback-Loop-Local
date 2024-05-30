cd /D "%~dp0" || exit

py -m venv venv
venv\Scripts\python.exe -m pip install pyyaml
venv\Scripts\python.exe -m pip install wheel

venv\Scripts\python.exe install\wheel_installation_setup.py -i install\next_py_api_wheel_info.yml
venv\Scripts\python.exe install\wheel_installation_setup.py -i install\next_py_api_win_wheel_info.yml

@echo off

echo "creating venv.."
call %1"\next\src/py_api/install.bat"
echo "successful"


echo "activate venv.."
call %1"\next\src/py_api/venv/Scripts/activate"
echo "successful"

py -m pip install src\py_api\dist\ecal5-5.13.0-cp39-cp39-win_amd64.whl
py -m pip install src\py_api\dist\next_py_api-0.14.0-py3-none-any.whl
pip install jsonschema
pip install bs4
pip install websocket-client==1.3.1

set ECAL_DATA=conan_workarea\package.next.generic.0.0.0-fallback.vs2017\bin
set PYTHONPATH=%PYTHONPATH%;%1"\next\src\py_api"
py next_py_api\next_api.py -c %2
popd

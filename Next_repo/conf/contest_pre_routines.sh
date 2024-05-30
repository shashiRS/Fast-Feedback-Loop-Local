#!/bin/sh
echo "Script executed from: ${PWD}"

NEXT_API_VERSION=0.14.0

# build the wheel package
echo "Create the wheel package"
sh "${PWD}"/src/py_api/install.sh

echo "Install the wheel package created"
/usr/bin/python3 -m pip install "${PWD}"/src/py_api/dist/next_py_api_linux-${NEXT_API_VERSION}-py3-none-any.whl

echo "Download the recording"
/usr/bin/python3 "${PWD}"/tests/integration/helper_files/General/download_recordings.py

echo "Contest pre routines finished"

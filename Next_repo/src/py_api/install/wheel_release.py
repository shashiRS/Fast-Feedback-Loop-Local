"""
    Copyright 2023 Continental Corporation
    :file: whl_release.py
    :platform: Windows, Linux
    :synopsis:
        Script to be used in Jenkins pipeline to identify utils to be build and uploaded to artifactory
    :author:
        - M. Shan Ur Rehman <Muhammad.Shan.ur.Rehman@continental-corporation.com>
"""

import logging
import os
import re
import shutil
import subprocess
import sys
import urllib.error
import urllib.request
from http import HTTPStatus

import wheel_installation_helper as helper
from bs4 import BeautifulSoup
from tabulate import tabulate

# directory variables
CWD = os.path.dirname(os.path.realpath(__file__))
UTILS_DIR = os.path.join(CWD, "..")
INFO_YML_DIR = os.path.join(UTILS_DIR, "install")
SETUP_FILE = os.path.join(CWD, "wheel_installation_setup.py")
DIST_DIR = os.path.join(UTILS_DIR, "dist")

# url and python command variables
INDEX_BASE_URL = "https://artifactory.geo.conti.de/artifactory/api/pypi/c_adas_cip_pypi_l"
INDEX_URL = "{}/simple/".format(INDEX_BASE_URL)
NEXT_API_REGEX = r"^next_py_api_(?P<platform>win|linux)-(?P<version>\d+\.\d+\.\d+)-[\w-]+\.whl$"
WHEEL_MAKE_CMD = "{} {} -i ".format(sys.executable, SETUP_FILE)
TWINE_UPLOAD_CMD = "{} -m twine upload --verbose -r artifactory ".format(sys.executable)
PIP_INSTALL_CMD = "{} -m pip install {}/".format(sys.executable, INDEX_BASE_URL)
PIP_UTIL_CHECK_CMD = "{} -m pip show ".format(sys.executable)

SUCCESS = 0
ERROR = 1
TABLE_GRID = "pretty"

sys.path.append(UTILS_DIR)

logging.basicConfig(level=logging.INFO, format="[%(levelname)s %(asctime)s] %(message)s")


def artifactory_upload(wheel_file: str) -> str:
    """
    Function to upload a particular wheel package to artifactory via twine module
    :param string wheel_file: A particular util wheel file name e.g. ``<util-name>-<version>-py3-none-any.whl``
    :return: returns the status
    :rtype: string
    """
    return_status = "OK"
    try:
        ws = os.getenv("WORKSPACE")
        if not ws:
            raise RuntimeError("NOK: WORKSPACE env_var NOT-FOUND (need to run in Jenkins)")
        whl_path = os.path.join(ws, "src", "py_api", "dist", wheel_file)
        if not os.path.exists(whl_path):
            raise RuntimeError(f"NOK: {whl_path} doesn't exist")
        logging.info(f"Trying to upload '{whl_path}' via twine")
        twine_cmd = TWINE_UPLOAD_CMD + whl_path
        logging.info(f"Twine CMD: {twine_cmd}")
        ret = subprocess.run(twine_cmd)
        if ret.returncode != 0:
            raise RuntimeError("NOK: Twine upload cmd return code != 0")
    except Exception as e:
        logging.info(f"Problem in uploading '{wheel_file}' ERR: {str(e)}")
        return_status = str(e)
    return return_status


def main() -> int:
    """
    Main function
    :return: returns the exit code (0 for all ok, 1 for some error)
    :rtype: int
    """
    return_code = SUCCESS
    # delete dist folder and meta-data files
    logging.info(f"{'*' * 30} Removing all meta-data files for wheel builds (if exists) at {UTILS_DIR} {'*' * 30}")
    helper.delete_wheel_build_metadata_files(UTILS_DIR)
    if os.path.exists(DIST_DIR):
        logging.info(f"Deleting 'dist' folder at {DIST_DIR}")
        shutil.rmtree(DIST_DIR)
    logging.info(f"Deleted meta-data files and dist folder from {UTILS_DIR} directory for fresh builds")
    logging.info(f"{'*' * 100}")

    logging.info(f"{'*'*30} Now fetching the info of all utils {'*'*30}")
    # get names of all utils and their respective information
    local_utils_info = dict()
    table_list = list()
    for info_yaml in os.listdir(INFO_YML_DIR):
        if info_yaml.endswith(".yml"):
            info_yml_path = os.path.join(INFO_YML_DIR, info_yaml)
            if os.path.exists(info_yml_path):
                logging.info(f"Found info.yml: '{info_yaml}'")
                version = helper.get_info_yml_content(info_yml_path)["version"]
                name = helper.get_info_yml_content(info_yml_path)["name"]
                local_utils_info[name] = {"path": info_yml_path, "version": version}
            else:
                version = None
                local_utils_info[name] = {"path": None, "version": None}
            table_list.append([name, version])
    logging.info(f"{'*' * 30} Info fetching done {'*' * 30}")

    # print an info/status table
    col_names = ["Wheel Info", "Local Version"]
    print(tabulate(table_list, headers=col_names, tablefmt=TABLE_GRID))

    # get version of utils from remote https://eu.artifactory.conti.de/artifactory/api/pypi/c_adas_cip_pypi_l/simple/
    logging.info(f"{'*' * 30} Getting version of utils from remote and filtering which util to release {'*' * 30}")
    remote_utils_version_info = dict()
    utils_to_release = list()
    for util in local_utils_info.keys():
        url = INDEX_URL + util
        to_release = False
        try:
            logging.info(f"Fetching Info from URL: {url}")
            # url open timeout is increased to 20 seconds in-case there is load on artifactory
            file = urllib.request.urlopen(url, timeout=20).read()
            soup = BeautifulSoup(file, features="lxml")
            versions = list(soup.stripped_strings)
            version_list = list()
            for version in versions:
                match = re.search(NEXT_API_REGEX, version)
                if match:
                    version_list.append(tuple(map(int, (match.group("version").split(".")))))
            if version_list:
                remote_latest_version = max(version_list, key=lambda t: t[:])
                remote_latest_version_str = ".".join(str(x) for x in remote_latest_version)
                remote_utils_version_info[util] = [version_list, remote_latest_version]
                if local_utils_info[util]["version"]:
                    local_version = tuple(map(int, (local_utils_info[util]["version"].split("."))))
                    if local_version > remote_latest_version:
                        to_release = True
                        # this case when we need build and upload util wheel as remote version is < local version
                        utils_to_release.append(
                            [util, to_release, True, remote_latest_version_str, local_utils_info[util]["version"]]
                        )
                    else:
                        # this case we DON'T need build and upload util wheel as local_version !> remote_latest_version
                        utils_to_release.append(
                            [util, to_release, True, remote_latest_version_str, local_utils_info[util]["version"]]
                        )
                else:
                    # this case when info.yml file for a util is not found
                    utils_to_release.append([util, to_release, True, remote_latest_version_str, "info.yml NOT FOUND"])
            else:
                utils_to_release.append(
                    [
                        util,
                        to_release,
                        False,
                        "ERR: Remote version regex match issue",
                        local_utils_info[util]["version"],
                    ]
                )
        except urllib.error.HTTPError as e:
            if e.code == HTTPStatus.NOT_FOUND:
                if local_utils_info[util]["version"]:
                    to_release = True
                    # this case when we want to upload a complete new util as we didn't find it remotely
                    utils_to_release.append([util, to_release, True, "NOT-FOUND", local_utils_info[util]["version"]])
                else:
                    # this case when info.yml file for a util is not found
                    utils_to_release.append([util, to_release, True, "NOT-FOUND", "info.yml NOT FOUND"])
            else:
                if local_utils_info[util]["version"]:
                    # this case when we didn't get remote version of a util due to any network issue
                    utils_to_release.append(
                        [util, to_release, False, "ERR In Getting Version", local_utils_info[util]["version"]]
                    )
                else:
                    # this case when we didn't get remote version and also info.yml not found
                    utils_to_release.append([util, to_release, False, "ERR In Getting Version", "info.yml NOT FOUND"])
        except Exception as e:
            logging.info(f"Generic problem in fetching versions for '{util}' package from remote")
            logging.info(f"ERR: {str(e)}")
            if local_utils_info[util]["version"]:
                utils_to_release.append(
                    [util, to_release, False, f"General ERR: {str(e)}", local_utils_info[util]["version"]]
                )
            else:
                utils_to_release.append([util, to_release, False, f"General ERR: {str(e)}", "info.yml NOT FOUND"])
    logging.info(f"{'*' * 100}")

    # print an info/status table
    col_names = ["To Release", "Remote Status", "Latest Remote Version", "Local Version"]
    print(tabulate(utils_to_release, headers=col_names, tablefmt=TABLE_GRID))

    logging.info(f"{'*' * 30} Building and uploading wheels for util which are filtered to be released {'*' * 30}")
    # changing the current working directory to make it easy to find target package
    # !! THIS STEP IS IMPORTANT !!
    os.chdir(UTILS_DIR)
    logging.info("Changed CWD to '{}'".format(UTILS_DIR))

    err_list = list()
    # use setup.py and twine to build & upload wheels of all identified utils and record utils whose failed
    for util_list in utils_to_release:
        to_release = util_list[1]
        util_name = util_list[0]
        remote_status = util_list[2]
        version_to_upload = util_list[4]
        logging.info(f"{'-' * 50} '{util_name}' To Release: {to_release} {'-' * 50}")
        if not remote_status:
            logging.info(f"SKIPPING build and upload for '{util_name}' as it's remote status is {remote_status}")
            err_list.append(ERROR)
            continue
        if to_release:
            info_yml_file = local_utils_info[util_name]["path"]
            logging.info(f"Starting to build '{util_name}' wheel package using '{info_yml_file}'")
            cmd = WHEEL_MAKE_CMD + info_yml_file
            ret = subprocess.run(cmd)
            if ret.returncode != 0:
                logging.info(f"Error in building wheel for '{util_name}' utility")
                # appending Wheel Build Status, Artifactory Upload and Installation Verification
                util_list.extend(["NOK", "NA", "NA"])
                err_list.append(ERROR)
            else:
                util_wheel_file_name = "{}-{}-py3-none-any.whl".format(util_name, version_to_upload)
                upload_status = artifactory_upload(util_wheel_file_name)
                if upload_status != "OK":
                    err_list.append(ERROR)
        else:
            logging.info(f"SKIPPING build and upload for '{util_name}' as it's not required")
            # appending Build Status, Artifactory Upload and Installation Verification
            util_list.extend(["NA", "NA", "NA"])
    logging.info(f"{'*' * 100}")

    # print an info/status table
    col_names = [
        "To Release",
        "Remote Status",
        "Latest Remote Version",
        "Local Version",
        "Wheel Build Status",
        "Artifactory Upload",
        "Installation Verification",
    ]
    print(tabulate(utils_to_release, headers=col_names, tablefmt=TABLE_GRID))

    if len(err_list) != 0:
        return_code = ERROR

    return return_code


if __name__ == "__main__":
    ret_code = main()
    logging.info(f"Return Code: {ret_code}")
    sys.exit(ret_code)

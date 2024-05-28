"""
    Copyright 2023 Continental Corporation
    :file: wheel_installation_setup.py
    :platform: Windows, Linux
    :synopsis:
        Generic setup script to build wheel package of a contest utility via it's info.yml file
        How To Use:
            ``python wheel_installation_setup.py -i <path_to_contest_util_info_yml>``
    :author:
        - M. Shan Ur Rehman <Muhammad.Shan.ur.Rehman@continental-corporation.com>
"""

import argparse
import logging
import os
import sys

import wheel_installation_helper
import yaml  # pylint: disable=import-error
from setuptools import setup  # pylint: disable=import-error

# global variables space
PACKAGE_DIR = os.path.join(os.path.dirname(os.path.realpath(__file__)), "..")
logging.basicConfig(level=logging.INFO, format="[%(levelname)s %(asctime)s] %(message)s")


# To be used later. Currently, issue when running setup twice.
def build_wheels(input_location):
    """
    Checks the input location and builds packages for all valid files

    :param input_location: file or folder to create inputs
    :type input_location: str or list
    """
    info_files = []
    if not os.path.exists(input_location):
        raise RuntimeError(f"Info file '{input_location}' does not exist")
    if os.path.isdir(input_location):
        info_files = [
            os.path.join(input_location, f)
            for f in os.listdir(input_location)
            if os.path.isfile(os.path.join(input_location, f)) and f.endswith(".yml")
        ]
    elif os.path.isfile(input_location):
        info_files.append(input_location)
    for file in info_files:
        try:
            build_wheel(file)
        except Exception as err:  # pylint: disable=broad-exception-caught
            print(err)
            continue


def build_wheel(info_file):
    """
    Function to build wheel package based on the information in given info.yml file
    :param string info_file: Path to a particular utility info.yml file
    """
    if not os.path.exists(info_file):
        raise RuntimeError(f"Info file '{info_file}' does not exist")
    # replacing customized arg '-i' with wheel_installation_setup.py original arg 'bdist_wheel' to build wheel package
    sys.argv = [sys.argv[0], "bdist_wheel"]
    logging.info("Loading '%s' file ...", info_file)
    with open(info_file, encoding="utf-8") as f:
        config = yaml.safe_load(f)
    logging.info("Info file loaded successfully")
    # check the folders from packages directory list, if not exist created them
    wheel_installation_helper.check_folders_and_create(config["packages"])
    # changing the current working directory to make it easy to find target package
    os.chdir(PACKAGE_DIR)
    logging.info("Changed CWD to '%s'", PACKAGE_DIR)
    setup(
        name=config["name"],
        version=config["version"],
        author=config["author"],
        author_email=config["email"],
        description=config["description"],
        license=config["license"],
        url=config["url"],
        packages=config["packages"],
        package_data=config["package_data"],
        python_requires=config["python_req"],
        classifiers=config["classifiers"],
        install_requires=config["dependencies"],
        entry_points={
            "console_scripts": [
                "next-api = next_py_api.next_api:main",
            ]
        },
    )
    logging.info("Deleting meta-data files")
    wheel_installation_helper.delete_wheel_build_metadata_files(PACKAGE_DIR)


if __name__ == "__main__":
    try:
        parser = argparse.ArgumentParser(formatter_class=argparse.RawTextHelpFormatter)
        parser.add_argument("-i", type=str, help="Wheel package info.yml file", required=True, metavar="info_file")
        args = parser.parse_args()
        build_wheel(args.i)
    except Exception as e:  # pylint: disable=broad-exception-caught
        logging.error("Error in building wheel for '%s': %s", args.i, e)
        sys.exit(1)

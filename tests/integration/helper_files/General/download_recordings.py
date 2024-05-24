"""
    Copyright 2024 Continental Corporation

    This file contains a function to download from the artifactory the recording

    :file: download_recordings.py

    :author:
        - Octavian Pavel <octavian.pavel@continental-corporation.com>
        - Alexandru Buraga <alexandru.2.buraga@continental-corporation.com>
"""

# mypy: ignore-errors
import hashlib
import os
from datetime import timedelta
from pathlib import Path
from timeit import default_timer as timer

import requests  # pylint: disable=import-error  #
from requests.packages.urllib3.exceptions import (  # pylint: disable=import-error  #
    InsecureRequestWarning,
)

requests.packages.urllib3.disable_warnings(InsecureRequestWarning)  # pylint: disable=no-member  #

ARTIFACTORY_PATH = (
    r"https://artifactory.geo.conti.de/artifactory/c_adas_customer_toolchain_generic_l/"
    r"Tools_external/Development_Framework/next/Recordings"
)
RREC_NAME = rf"{ARTIFACTORY_PATH}/4_parallel_park_in_succ_extract_ext_00.00.20.204.418_00.00.25.115.625.rrec"
RREC_HASH = "83f60f476ee53ab3025d5d27bd41e548eac004db8f2eb387a8c8c1580e0281d3"
ZREC_NAME = rf"{ARTIFACTORY_PATH}/4_parallel_park_in_succ_extract_ext_00.00.20.204.418_00.00.25.115.625.zrec"
ZREC_HASH = "71fc6b093d76b0af2e53c5d2c714174119135c7ee37b21214732033f7b75fc66"


def download_recording(recording_path, local_path_recording, artifactory_username, artifactory_api_key):
    """
    Function to download recording from the Artifactory
    Args:
        recording_path (str): Artifactory path to the recording
        local_path_recording (str): Local path where the recording will be saved
        artifactory_username (str): username used for download
        artifactory_api_key (str): Artifactory credentials
    Returns:
        None
    """
    print(f"Start downloading of following recording : {recording_path}")
    r = requests.get(recording_path, auth=(artifactory_username, artifactory_api_key), timeout=120)
    print(f"End downloading of following recording : {recording_path}")
    # Create folder in case is not created
    folder_path = os.path.dirname(local_path_recording)
    os.makedirs(folder_path, exist_ok=True)
    # Write recording file
    with open(local_path_recording, "wb") as f:
        f.write(r.content)
    # Print some information for user
    print(
        f"Size of the recording file {os.path.basename(recording_path)}"
        f" is {Path(local_path_recording).stat().st_size / 10**3} kb."
    )
    print(f"Code returned for downloading rrec file : {r.status_code}")


def get_recording_hashkey(recording_path):
    """
    Function to get the hash key of the local recording file
    Args:
        recording_path (str): path to the recording file
    Returns
        str: the sha256 hash of the file
    """
    sha256_hash = hashlib.sha256()
    with open(recording_path, "rb") as f:
        # Read and update hash string value in blocks of 4K
        for byte_block in iter(lambda: f.read(4096), b""):
            sha256_hash.update(byte_block)
    return sha256_hash.hexdigest()


def check_hash_for_file(expected_hashkey, local_file_hashkey):
    """
    Function to compare 2 hash keys of 2 files
    Args:
        expected_hashkey (str): expected hash key
        local_file_hashkey (str): hash key of the local file
    Returns
        return True if the hash key are identical
        return False if hash key are NOT identical
    """
    if expected_hashkey == local_file_hashkey:
        return True
    return False


def get_credentials_from_conan_build():
    """
    Function to return the Artifactory credentials from conan build
    It will search on the build.xxx folder
    Args:

    Returns
        return the username and key from conan_build
    """
    artifactory_username = ""
    key = ""
    script_folder = os.path.dirname(os.path.abspath(__file__))
    conan_workarea_folder = os.path.abspath(os.path.join(script_folder, "..", "..", "..", "..", "conan_workarea"))
    conan_build_file = ""
    folders = os.listdir(conan_workarea_folder)
    for folder_name in folders:
        if "build." in folder_name and os.path.isdir(os.path.join(conan_workarea_folder, folder_name)):
            conan_build_file = os.path.join(conan_workarea_folder, folder_name, "conan_build.ps1")
            break
    if conan_build_file == "":
        print("conan_build.ps1 script not found in the conan_workarea folder")
        return None, None
    with open(conan_build_file, "r", encoding="utf-8") as f:
        conan_build_file_content = f.read()

    conan_build_file_content = conan_build_file_content.split("\n")

    user_found = False
    key_found = False
    for line in conan_build_file_content:
        if "CONAN_LOGIN_USERNAME_CONTI_CENTER" in line:
            artifactory_username = line.split("=")[-1].replace('"', "")
            user_found = True
        elif "CONAN_PASSWORD_CONTI_CENTER" in line:
            key = line.split("=")[-1].replace('"', "")
            key_found = True

        # Break the loop if username and key were found
        if user_found and key_found:
            break

    return artifactory_username, key


if __name__ == "__main__":
    # Get conan user and password from Environment variables
    username = os.environ.get("CONAN_LOGIN_USERNAME_CONTI_CENTER")
    artifactoryApiKey = os.environ.get("CONAN_PASSWORD_CONTI_CENTER")

    # Check if username and artifactoryApiKey is None. If yes, then use current user and generic conan password
    if username is None or artifactoryApiKey is None:
        username, artifactoryApiKey = get_credentials_from_conan_build()

    recordings = {ZREC_NAME: ZREC_HASH, RREC_NAME: RREC_HASH}
    NO_OF_RETRY = 3

    for rec in recordings.items():
        recording = rec[0]
        recording_name = os.path.basename(recording)
        RETRY_NUMBER = 0
        recording_local_path = os.path.join(
            str(Path(__file__).absolute().parent), "..", "..", "..", "Recordings", recording_name
        )

        while RETRY_NUMBER < NO_OF_RETRY:
            # Check if recording file already exists
            if os.path.exists(recording_local_path):
                RECORDING_HASHKEY = get_recording_hashkey(recording_local_path)
                if check_hash_for_file(recordings[recording], RECORDING_HASHKEY):
                    print("Recording {recording_local_path} file already exists.")
                    break

            start = timer()
            download_recording(recording, recording_local_path, username, artifactoryApiKey)
            end = timer()
            print("Download recording time: ", timedelta(seconds=end - start))
            if check_hash_for_file(recordings[recording], get_recording_hashkey(recording_local_path)):
                print(f"Recording {recording_name} downloaded successfully.")
                break
            if RETRY_NUMBER == NO_OF_RETRY - 1:
                if os.path.isfile(recording_local_path):
                    os.remove(recording_local_path)
                raise Exception(  # pylint: disable=broad-exception-raised
                    f"Download of the recording {recording_name} failed, the hashes are not the same"
                )
            RETRY_NUMBER += 1

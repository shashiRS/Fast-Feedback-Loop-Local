#! /usr/bin/env python3
'''
This little script will download the bootstrap script which in
turn will install the cip_build_system.

Consult the following URL for the location of an up-to-date version of this
script:
https://cip-config.cmo.conti.de/v2/configuration/bricks/bootstrap/1.0/configurations/bootstrap.json # nopep8
'''

###############################################################################
# START: Common code section for cip.py and bootstrap.py
###############################################################################

import abc
import hashlib
import http
import json
import os.path
import pathlib
import random
import time
import typing
from urllib.error import URLError
import urllib.request

http.client.HTTPConnection.debuglevel = 5

DEFAULT_DOWNLOAD_RETRIES = 9
MIN_SECONDS_BETWEEN_DOWNLOAD_RETRIES = 3
MAX_SECONDS_BETWEEN_DOWNLOAD_RETRIES = 10

class NetworkAccess(abc.ABC):
    """Abstract interface for network access"""
    def read(self, url: str) -> bytes:
        """Fetch the resource at the given URL and deliver its content as bytes"""

class HttpNetworkAccess(NetworkAccess):
    """HTTP implementation of the network access interface"""
    def read(self, url: str) -> bytes:
        """Fetch the resource at the given URL and deliver its content as bytes"""
        with urllib.request.urlopen(url) as response:
            return response.read()

class FilesystemAccess(abc.ABC):
    """Abstract interface for filesystem access"""
    def read(self, path: pathlib.Path) -> bytes:
        """Read the file at the given path and deliver its content as bytes"""

    def write(self, path: pathlib.Path, content: bytes):
        """Write the binary content to the specified path. The parent 
           directories for "path" are created if needed."""

    def exists(self, path: pathlib.Path) -> bool:
        """Check whether the given path exists"""

    def getmtime(self, path: pathlib.Path) -> float:
        """Return the time of last modification of path. The return value is a 
        floating point number giving the number of seconds since the epoch."""

class PhysicalFilesystemAccess(FilesystemAccess):
    """Physical implementation of the filesystem access interface"""
    def read(self, path: pathlib.Path) -> bytes:
        """Read the file at the given path and deliver its content as bytes"""
        with path.open("rb") as f:
            return f.read()

    def write(self, path: pathlib.Path, content: bytes):
        """Write the binary content to the specified path. The parent 
           directories for "path" are created if needed."""
        parent_dir = path.parent

        if not parent_dir.exists():
            parent_dir.mkdir(parents=True)

        with path.open('wb') as output_stream:
            output_stream.write(content)

    def exists(self, path: pathlib.Path) -> bool:
        """Check whether the given path exists"""
        return path.exists()

    def getmtime(self, path: pathlib.Path) -> float:
        """Return the time of last modification of path. The return value is a 
        floating point number giving the number of seconds since the epoch."""
        return os.path.getmtime(str(path))

# Stores the location and the checksum of a Bricks bootstrap file (cip.py or bootstrap.py).
BootstrapFileInfo = typing.NamedTuple("BootstrapFileInfo", [
       ("href", str),  # Bootstrap file URL
       ("sha256", str) # Bootstrap file SHA256 hash
])

# Stores file information about cip.py and bootstrap.py
BootstrapFiles = typing.NamedTuple("BootstrapFiles", [
    ("cip_py", BootstrapFileInfo),
    ("bootstrap_py", BootstrapFileInfo)
])

# Stores pip config properties
PipConfig = typing.NamedTuple("PipConfig", [
    ("pypi_repo", str)
])

# Stores bootstrap info, in particular the URLs and
# checksums of the latest versions of cip.py and bootstrap.py
BootstrapInfo = typing.NamedTuple("BootstrapInfo", [
    ("bootstrap_files", BootstrapFiles),
    ("pip_config", PipConfig)
])

def calculate_sha256_for_url(url: str, network_access: NetworkAccess) -> str:
    """Calculate the SHA256 checksum for the resource under the given url.
       The resource is considered to be a text file in UTF-8 encoding.
       The function strips out all end-of-line charachters to avoid differences
       between the operating systems.
       The checksum is returned as an upper case hex string.
    """
    binary_content = network_access.read(url)
    text_content = binary_content.decode("utf-8")

    h = hashlib.sha256()
    for line in text_content.splitlines():
        h.update(bytes(line, "utf-8"))

    return h.hexdigest().upper()

class BootstrapFileInfoFactory:
    """Factory for the creation of BootstrapFileInfo instances"""
    def __init__(self, network_access: NetworkAccess):
        self.__network_access = network_access

    def create(self, bootstrap_file_info_dict: typing.Dict[str, typing.Dict]) -> BootstrapFileInfo:
        """Create a BootstrapFileInfo instance.
        :param typing.Dict[str, typing.Dict] bootstrap_file_info_dict:
            Dictionary describing the bootstrap file info, has the following format:
            {
                "href": "...",
                "sha256": "..."
            }
            Note that the "sha256" property is optional.
        :return: BootstrapFileInfo instance
        """
        href = bootstrap_file_info_dict["href"]
        
        if "sha256" in bootstrap_file_info_dict:
            sha256 = bootstrap_file_info_dict["sha256"]
        else:
            sha256 = calculate_sha256_for_url(href, self.__network_access)

        return BootstrapFileInfo(href, sha256)

class BootstrapFilesFactory:
    """Factory for the creation of RepositoryFiles instances"""
    def __init__(self, network_access: NetworkAccess):
        self.__network_access = network_access

    def create(self, bootstrap_files: typing.Dict[str, typing.Dict]) -> BootstrapFiles:
        """Create a BootstrapFiles instance from a dictionary.
        
        :param typing.Dict[str, typing.Dict] bootstrap_files:
            Dictionary describing the bootstrap files, has the following format:
            {
              "bootstrap_py": {
                  <bootstrap.py file info>
              },
              "cip_py": {
                  <cip.py file info>
              }
            }
        :return: BootstrapFiles instance
        """
        bootstrap_file_info_factory = BootstrapFileInfoFactory(self.__network_access)
        cip_py_file_info = bootstrap_file_info_factory.create(bootstrap_files["cip_py"])
        bootstrap_py_file_info = bootstrap_file_info_factory.create(bootstrap_files["bootstrap_py"])
        
        return BootstrapFiles(cip_py_file_info, bootstrap_py_file_info)

class PipConfigFactory:
    """Factory for the creation of PipConfig instances"""
    def create(self, pip_config_dict: typing.Dict[str, str]) -> PipConfig:
        """Create a PipConfig instance from a dictionary.
        
        :param typing.Dict[str, typing.Dict] pip_config_dict:
            Dictionary describing the pip config, has the following format:
            {
                "pypi_repo": <PyPI repo URL>
            }
        :return: PipConfig instance
        """
        pypi_repo = pip_config_dict["pypi_repo"]

        return PipConfig(pypi_repo)

def get_cip_config_dir() -> pathlib.Path:
    return pathlib.Path.home() / "cip_config_dir"

def get_bootstrap_config_dir() -> pathlib.Path:
    return get_cip_config_dir() / "bootstrap"

class BootstrapInfoFactory:
    """Factory for the creation of BootstrapInfo instances"""

    CENTRAL_CONFIGURATION_ENDPOINT = "https://cip-config.cmo.conti.de/v2/configuration/bricks/bootstrap/1.0/configurations/bootstrap.json" # nopep8
    BOOTSTRAP_INFO_PATH = get_bootstrap_config_dir() / "bootstrap.json"

    def __init__(self, network_access: NetworkAccess = None,
                 central_configuration_endpoint: str = None):
        if network_access:
            self.__network_access = network_access
        else:
            self.__network_access = HttpNetworkAccess()

        if central_configuration_endpoint:
            self.__central_configuration_endpoint = central_configuration_endpoint
        else:
            self.__central_configuration_endpoint = self.CENTRAL_CONFIGURATION_ENDPOINT

    def is_local_bootstrap_info_file_up_to_date(self, bootstrap_local_config, 
                                     filesystem_access: FilesystemAccess = None) -> bool:
        return filesystem_access.exists(self.BOOTSTRAP_INFO_PATH)\
               and (time.time() - filesystem_access.getmtime(self.BOOTSTRAP_INFO_PATH)
                    < bootstrap_local_config.max_bootstrap_info_age)

    def create_from_file(self, bootstrap_local_config, filesystem_access: FilesystemAccess) -> BootstrapInfo:
        if not self.is_local_bootstrap_info_file_up_to_date(bootstrap_local_config, filesystem_access):
            download_file(self.__central_configuration_endpoint, self.BOOTSTRAP_INFO_PATH, False,
                          self.__network_access, filesystem_access)

        json_bytes = filesystem_access.read(self.BOOTSTRAP_INFO_PATH)
        json_str = json_bytes.decode("UTF-8")
        bootstrap_info_root_dict = json.loads(json_str)
        bootstrap_info_dict = bootstrap_info_root_dict["bootstrap_info"]
        
        bootstrap_files_factory = BootstrapFilesFactory(self.__network_access)
        bootstrap_files = bootstrap_files_factory.create(bootstrap_info_dict["bootstrap_files"])

        pip_config_factory = PipConfigFactory()
        pip_config = pip_config_factory.create(bootstrap_info_dict["pip_config"])

        return BootstrapInfo(bootstrap_files, pip_config)

def calculate_sha256_for_file(path: pathlib.Path, 
                              filesystem_access: FilesystemAccess = None) -> str:
    """Calculate the SHA256 checksum for the resource under the given path.
       The resource is considered to be a text file in UTF-8 encoding.
       The function strips out all end-of-line charachters to avoid differences
       between the operating systems.
       The checksum is returned as an upper case hex string.
    """
    if not filesystem_access:
        filesystem_access = PhysicalFilesystemAccess()

    binary_content = filesystem_access.read(path)
    text_content = binary_content.decode("utf-8")

    h = hashlib.sha256()
    for line in text_content.splitlines():
        h.update(bytes(line, "utf-8"))

    return h.hexdigest().upper()

def is_file_up_to_date(path: pathlib.Path, 
                       bootstrap_file_info: BootstrapFileInfo, 
                       filesystem_access: FilesystemAccess = None)\
                       -> bool:
    """Check whether the SHA256 hash of the file under "path" is equal to the
    SHA256 hash specified in "bootstrap_file_info"."""
    return calculate_sha256_for_file(path, filesystem_access) == bootstrap_file_info.sha256.upper()

def download_file(url: str, path: pathlib.Path, 
                  dont_fail_on_download_error_if_file_already_exists: bool = False,
                  network_access: NetworkAccess = None, 
                  filesystem_access: FilesystemAccess = None,
                  retries = DEFAULT_DOWNLOAD_RETRIES) -> bool:
    """Download the file from the given url and store it under path.
    Return True if download was successful and False otherwise."""
    if not network_access:
        network_access = HttpNetworkAccess()

    if not filesystem_access:
        filesystem_access = PhysicalFilesystemAccess()

    try:
        binary_content = network_access.read(url)
        filesystem_access.write(path, binary_content)
        return True
    except URLError:
        if dont_fail_on_download_error_if_file_already_exists and filesystem_access.exists(path):
            return False
        else:
            if retries < 1:
                raise
            else:
                seconds_to_sleep = random.randint(MIN_SECONDS_BETWEEN_DOWNLOAD_RETRIES, MAX_SECONDS_BETWEEN_DOWNLOAD_RETRIES)

                if retries > 0:
                    retry_and_attempts_msg = " Retrying in {seconds} seconds ({attempts} attempts remaining)..."\
                        .format(seconds=seconds_to_sleep, attempts=retries)
                else:
                    retry_and_attempts_msg = ""
                
                print("\nUnable to download {href}.{retry_and_attempts_msg}"
                    .format(href=url, retry_and_attempts_msg=retry_and_attempts_msg))
                time.sleep(seconds_to_sleep)
                return download_file(url, path, dont_fail_on_download_error_if_file_already_exists, network_access, filesystem_access, retries - 1)

def update_file_if_needed(path: pathlib.Path,
                          bootstrap_file_info: BootstrapFileInfo,
                          dont_fail_on_download_error_if_file_already_exists: bool,
                          suppress_message_if_update_not_needed: bool,
                          network_access: NetworkAccess = None, 
                          filesystem_access: FilesystemAccess = None,
                          retries = DEFAULT_DOWNLOAD_RETRIES):
    """If the SHA256 hash of the local file under "path" differs from the
       SHA256 specified in "bootstrap_file_info", download the latest version
       of the file from the URL specified in "bootstrap_file_info" and store it
       under "path".
    """
    if not network_access:
        network_access = HttpNetworkAccess()

    if not filesystem_access:
        filesystem_access = PhysicalFilesystemAccess()

    if not filesystem_access.exists(path) \
       or not is_file_up_to_date(path, bootstrap_file_info, filesystem_access):
        try:
            success = download_file(bootstrap_file_info.href, path, dont_fail_on_download_error_if_file_already_exists, network_access, filesystem_access, retries)
            if success:
                print("\nReplaced \"{path}\" with the latest version.\n".format(path=str(path)))
            elif filesystem_access.exists(path):
                print("\nUnable to download {href}. Will try to proceed with the existing {path}".format(href=bootstrap_file_info.href, path=str(path)))
        except URLError:
            if dont_fail_on_download_error_if_file_already_exists and filesystem_access.exists(path):
                print("\nUnable to download {href}. Will try to proceed with the existing {path}".format(href=bootstrap_file_info.href, path=str(path)))
            else:
                raise
    elif not suppress_message_if_update_not_needed:
        print("\n\"{path}\" is already up-to-date.\n    -> No update performed.\n".format(path=str(path)))

class BootstrapLocalConfig:
    VERSION = 1
    DEFAULT_MAX_LOCAL_BOOTSTRAP_INFO_AGE_IN_SECONDS = 6 * 3600
    BOOTSTRAP_LOCAL_CONFIG_PATH = get_bootstrap_config_dir() / "config.json"

    def __init__(self, config_dict):
        """Create an instance of BootstrapLocalConfig from the given dictionary"""
        self._version = int(config_dict["version"])
        self._max_bootstrap_info_age = int(config_dict["max_bootstrap_info_age_in_seconds"])

    @classmethod
    def create(cls, filesystem_access: FilesystemAccess = None):
        """Create an instance of BootstrapLocalConfig by reading the default
        bootstrap local config file"""
        if not filesystem_access:
            filesystem_access = PhysicalFilesystemAccess()

        if not filesystem_access.exists(cls.BOOTSTRAP_LOCAL_CONFIG_PATH):
            cls.write_default_bootstrap_local_config(filesystem_access)

        config_bytes = filesystem_access.read(cls.BOOTSTRAP_LOCAL_CONFIG_PATH)
        config_str = config_bytes.decode("utf-8")
        config_dict = json.loads(config_str)

        if int(config_dict["version"]) != cls.VERSION:
            cls.write_default_bootstrap_local_config(filesystem_access)
            config_bytes = filesystem_access.read(cls.BOOTSTRAP_LOCAL_CONFIG_PATH)
            config_str = config_bytes.decode("utf-8")
            config_dict = json.loads(config_str)

        return BootstrapLocalConfig(config_dict)

    @classmethod
    def write_default_bootstrap_local_config(cls, filesystem_access: FilesystemAccess = None):
        """Create a default boostrap local config file"""
        print("writing default Bricks bootstrap config file: {}".format(cls.BOOTSTRAP_LOCAL_CONFIG_PATH))
        config_dict = {
            "version": cls.VERSION,
            "max_bootstrap_info_age_in_seconds": cls.DEFAULT_MAX_LOCAL_BOOTSTRAP_INFO_AGE_IN_SECONDS
        }
        
        config_str = json.dumps(config_dict, indent=4)
        filesystem_access.write(cls.BOOTSTRAP_LOCAL_CONFIG_PATH, bytes(config_str, "utf-8"))

    @property
    def version(self) -> int:
        """Return file format version"""
        return self._version

    @property
    def max_bootstrap_info_age(self) -> int:
        """Return maximum age of the local bootstrap info (in seconds) before 
        it is considered out-of-date."""
        return self._max_bootstrap_info_age

class BootstrapSession:
    """Provides access to the local config and the central configuration bootstrap info"""
    def __init__(self, filesystem_access: FilesystemAccess = None, network_access: NetworkAccess = None):
        if not filesystem_access:
            filesystem_access = PhysicalFilesystemAccess()
        if not network_access:
            network_access = HttpNetworkAccess()
        
        self.__bootstrap_local_config = BootstrapLocalConfig.create(filesystem_access)
        
        bootstrap_info_factory = BootstrapInfoFactory(network_access)
        self.__bootstrap_info\
            = bootstrap_info_factory.create_from_file(self.__bootstrap_local_config, filesystem_access)

    @property
    def bootstrap_local_config(self) -> BootstrapLocalConfig:
        return self.__bootstrap_local_config

    @property
    def bootstrap_info(self) -> BootstrapInfo:
        return self.__bootstrap_info


###############################################################################
# END: Common code section for cip.py and bootstrap.py
###############################################################################


import argparse
import os
import subprocess
import sys

__DEFAULT_VIRTUAL_ENV_PATH = pathlib.Path(__file__).parent.parent / "venv"

def bootstrap_py(venv_dir: pathlib.Path) -> pathlib.Path:
    """get location of bootstrap.py"""
    return venv_dir / "_bootstrap.py"

def main(args) -> int:
    """Download the bootstrap script
    which is then used to download
    and install the rest of Bricks

    :return: exit code of the boostrap process
    :rtype: integer
    """
    parser = argparse.ArgumentParser(
        description='Bootstrap for CIP Build System 4.x',
        add_help=False
    )

    parser.add_argument(
        '--venv-directory',
        dest='venv_dir',
        type=pathlib.Path,
        default=__DEFAULT_VIRTUAL_ENV_PATH,
        help='Specify location of virtual environment location'
    )

    parsed_args, passed_args = parser.parse_known_args(args)
    boostrap_script = bootstrap_py(parsed_args.venv_dir)

    bootstrap_session = BootstrapSession()
    bootstrap_info = bootstrap_session.bootstrap_info
    bootstrap_py_file_info = bootstrap_info.bootstrap_files.bootstrap_py

    update_file_if_needed(boostrap_script, bootstrap_py_file_info, True, True)

    exec_args = [
        sys.executable,
        "-u",
        str(boostrap_script),
        "--venv-directory", str(parsed_args.venv_dir),
    ]

    exec_args.extend(passed_args)

    # this is for windows etc
    res = subprocess.run(
        exec_args,
        env=os.environ,
        stdout=None,
        stderr=None,
        check=False,
    )
    if res.returncode != 0:
        sys.stderr.write('Command failed\n')
    return res.returncode

if __name__ == '__main__':
    # exit process with same
    # exit code as child did
    exit(main(sys.argv[1:]))

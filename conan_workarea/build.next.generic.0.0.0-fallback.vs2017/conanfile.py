'''
This is automatically generated file by Continental CIP tools

The idea is that users do not have to write complicated conanfile
all by themselves but merely modify configuration for their module
and build scripts do proper conanfile.py

The second, bit more draconian, reason is that this way conanfile.py
is always matching our current workflow so users do not have to make
too many modifications to their build script if our processes or workflow
changes later on

'''

import base64
import os
import re
import sys
import shutil
import pathlib
import json
import hashlib
import zipfile
import subprocess
import typing
import traceback
import random
from datetime import (
    datetime,
    timezone,
)

from conans import ConanFile, CMake, tools
from conans.errors import ConanException, ConanInvalidConfiguration


class nextPackage(ConanFile):
    ''' generic prefilled template for dependencies

        Define variables needed by conan:
        https://docs.conan.io/en/latest/reference/conanfile/attributes.html

        This script uses the following environment variables:
          -  CIP_PACKAGE_OVERRIDE
          -  CMAKE_BUILD_PARALLEL_LEVEL
          -  JENKINS_HOME
          -  GIT_USER
          -  GIT_PASSWORD
          -  JENKINS_URL
          -  CONAN_RECIPE_HIDE_CUSTOMEVENTS

        For fingerprint
          -  JENKINS_URL
          -  JOB_NAME
          -  BUILD_TAG
          -  BUILD_NUMBER
          -  JENKINS_URL
          -  BRANCH_NAME
          -  TAG_NAME
          -  JOB_NAME
          -  BUILD_TAG
          -  BUILD_NUMBER
          -  USERNAME
          -  USER
          -  COMPUTERNAME
          -  HOSTNAME
    '''

    # -- conan attributes
    name = "next.generic" # we use the binary key as conan package name
    version = "0.0.0-fallback"
    license = "ADC GmbH; all rights reserved"
    url = "https://github-am.geo.conti.de/ADAS/next.git"
    description = "Next integration for the SIMone project"

    default_user = 'cip'
    default_channel = 'releases'


    CIP_BUILD_SYSTEM_VERSION = "4.25.0"

    # this is true if package _output_ is crossplatform
    # good examples are
    # * java class files
    # * python, perl and shell scripts
    # * documentation
    # * generated source files
    variant_target_any_platform = False
    cip_is_tool = False

    settings = tuple(['os', 'arch', 'compiler', 'build_type', 'bricks_platform_name'])

    cip_package_name = "next"

    variant_to_build = "generic"
    build_platform = "vs2017"
    target_platform = "vs2017"

    # define options for variant
    variant_options = {'BUILDDOCUMENTATION': {'default': False, 'description': 'documentation build', 'type': 'boolean', 'value': False}, 'OPTIMIZEDDEBUG': {'default': False, 'description': 'performance optimized debug build', 'type': 'boolean', 'value': True}}

    cmake_toolchain_file = 'conan/conan_toolchain.cmake'

    # txt generator is only one which we need since
    # since handcrafter CMake routines take care of rest
    # See: PMTJSD-217335 and
    # https://docs.conan.io/2.0/reference/tools/microsoft/vcvars.html
    generators = "VCVars"
    no_copy_source = True
    short_paths = True

    # binary overrides for this conan package
    cip_binary_overrides = {}

    cip_groups_to_build = ['PACKAGE_INFO', 'NEXT_COMPONENT', 'PLAYER_COMPONENT', 'BRIDGES_COMPONENT', 'CONTROL_COMPONENT', 'UDEX_COMPONENT', 'SDK_COMPONENT']
    cip_groups_to_install = ['PACKAGE_INFO', 'NEXT_COMPONENT', 'PLAYER_COMPONENT', 'BRIDGES_COMPONENT', 'CONTROL_COMPONENT', 'UDEX_COMPONENT', 'SDK_COMPONENT', '__BRICKS_IMPLICIT_DEPENDENCIES']

    cip_execution_method = "build"

    should_use_bricks_core = True

    event_id = random.randint(0, 9999999) # some random number for the event ids

    # list of files which are exported for recipe file itself
    exports = [
        "conanfile.py",
        "entities.json",
        "selected_entity.json"
    ]

    compressed_package_file = "".replace(
        '/',
        os.path.sep
    )

    def __bricks_init(self):
        """old, incompatible initialize method tweaked
        to be compatible with newer conan versions"""


        if not hasattr(self, '__cip_binary_dependencies'):


            # binary dependencies for this conan package
            # we have to use ordered tuples so conanfile
            # is not modified with new regeneration
            binary_dependencies = {'ubuntu_20_04_x86_64-release': {'build': ['VFR_Utilities.generic', 'bricks_cmake_checks.__system', 'bricks_cmake_core.__system', 'bricks_cmake_groups.__system', 'cip_adaption_boost.generic', 'cip_build_system_platforms.__system', 'cmake', 'flatbuffers', 'jsoncpp.generic', 'make', 'opencv.generic', 'protobuf', 'spdlog.generic'], 'production': ['VFR_Utilities.generic', 'asio.generic', 'cip_adaption_boost.generic', 'cmake', 'ecal.measurement', 'flatbuffers', 'hdf5.generic', 'jsoncpp.generic', 'make', 'mts-core-lib', 'mts-sentinel', 'opencv.generic', 'protobuf', 'recycle.generic', 'simpleini.generic', 'spdlog.generic', 'tclap.generic', 'tcp_pubsub.generic', 'tracy'], 'system': ['bricks_cmake_checks.__system', 'bricks_cmake_core.__system', 'bricks_cmake_groups.__system'], 'platform': 'cip_build_system_platforms.__system', 'deploy': ['asio.generic', 'ecal.measurement', 'hdf5.generic', 'mts-core-lib', 'mts-sentinel', 'recycle.generic', 'simpleini.generic', 'tclap.generic', 'tcp_pubsub.generic', 'tracy'], 'all': ['VFR_Utilities.generic', 'asio.generic', 'bricks_cmake_checks.__system', 'bricks_cmake_core.__system', 'bricks_cmake_groups.__system', 'cip_adaption_boost.generic', 'cip_build_system_platforms.__system', 'cmake', 'ecal.measurement', 'flatbuffers', 'hdf5.generic', 'jsoncpp.generic', 'make', 'mts-core-lib', 'mts-sentinel', 'opencv.generic', 'protobuf', 'recycle.generic', 'simpleini.generic', 'spdlog.generic', 'tclap.generic', 'tcp_pubsub.generic', 'tracy']}, 'linux64_x86_gcc_7-v2-release': {'build': ['VFR_Utilities.generic', 'bricks_cmake_checks.__system', 'bricks_cmake_core.__system', 'bricks_cmake_groups.__system', 'cip_adaption_boost.generic', 'cip_build_system_platforms.__system', 'cmake', 'flatbuffers', 'jsoncpp.generic', 'make', 'opencv.generic', 'protobuf', 'spdlog.generic'], 'production': ['VFR_Utilities.generic', 'asio.generic', 'cip_adaption_boost.generic', 'cmake', 'ecal.measurement', 'flatbuffers', 'hdf5.generic', 'jsoncpp.generic', 'make', 'mts-core-lib', 'mts-sentinel', 'opencv.generic', 'protobuf', 'recycle.generic', 'simpleini.generic', 'spdlog.generic', 'tclap.generic', 'tcp_pubsub.generic', 'tracy'], 'system': ['bricks_cmake_checks.__system', 'bricks_cmake_core.__system', 'bricks_cmake_groups.__system'], 'platform': 'cip_build_system_platforms.__system', 'deploy': ['asio.generic', 'ecal.measurement', 'hdf5.generic', 'mts-core-lib', 'mts-sentinel', 'recycle.generic', 'simpleini.generic', 'tclap.generic', 'tcp_pubsub.generic', 'tracy'], 'all': ['VFR_Utilities.generic', 'asio.generic', 'bricks_cmake_checks.__system', 'bricks_cmake_core.__system', 'bricks_cmake_groups.__system', 'cip_adaption_boost.generic', 'cip_build_system_platforms.__system', 'cmake', 'ecal.measurement', 'flatbuffers', 'hdf5.generic', 'jsoncpp.generic', 'make', 'mts-core-lib', 'mts-sentinel', 'opencv.generic', 'protobuf', 'recycle.generic', 'simpleini.generic', 'spdlog.generic', 'tclap.generic', 'tcp_pubsub.generic', 'tracy']}, 'ubuntu_20_04_x86_64': {'deploy': ['asio.generic', 'ecal.measurement', 'hdf5.generic', 'mts-core-lib', 'mts-sentinel', 'recycle.generic', 'simpleini.generic', 'tclap.generic', 'tcp_pubsub.generic', 'tracy'], 'production': ['VFR_Utilities.generic', 'asio.generic', 'cip_adaption_boost.generic', 'cmake', 'ecal.measurement', 'flatbuffers', 'hdf5.generic', 'jsoncpp.generic', 'make', 'mts-core-lib', 'mts-sentinel', 'opencv.generic', 'protobuf', 'recycle.generic', 'simpleini.generic', 'spdlog.generic', 'tclap.generic', 'tcp_pubsub.generic', 'tracy'], 'build': ['VFR_Utilities.generic', 'bricks_cmake_checks.__system', 'bricks_cmake_core.__system', 'bricks_cmake_groups.__system', 'cip_adaption_boost.generic', 'cip_build_system_platforms.__system', 'cmake', 'flatbuffers', 'jsoncpp.generic', 'make', 'opencv.generic', 'protobuf', 'spdlog.generic'], 'system': ['bricks_cmake_checks.__system', 'bricks_cmake_core.__system', 'bricks_cmake_groups.__system'], 'platform': 'cip_build_system_platforms.__system', 'all': ['VFR_Utilities.generic', 'asio.generic', 'bricks_cmake_checks.__system', 'bricks_cmake_core.__system', 'bricks_cmake_groups.__system', 'cip_adaption_boost.generic', 'cip_build_system_platforms.__system', 'cmake', 'ecal.measurement', 'flatbuffers', 'hdf5.generic', 'jsoncpp.generic', 'make', 'mts-core-lib', 'mts-sentinel', 'opencv.generic', 'protobuf', 'recycle.generic', 'simpleini.generic', 'spdlog.generic', 'tclap.generic', 'tcp_pubsub.generic', 'tracy']}, 'vs2017_debug': {'build': ['VFR_Utilities.generic', 'bricks_cmake_checks.__system', 'bricks_cmake_core.__system', 'bricks_cmake_groups.__system', 'cip_adaption_boost.generic', 'cip_build_system_platforms.__system', 'cmake', 'flatbuffers', 'jsoncpp.generic', 'make', 'opencv.generic', 'protobuf', 'spdlog.generic'], 'production': ['VFR_Utilities.generic', 'asio.generic', 'cip_adaption_boost.generic', 'cmake', 'ecal.measurement', 'flatbuffers', 'hdf5.generic', 'jsoncpp.generic', 'make', 'mts-core-lib', 'mts-sentinel', 'opencv.generic', 'protobuf', 'recycle.generic', 'simpleini.generic', 'spdlog.generic', 'tclap.generic', 'tcp_pubsub.generic', 'tracy'], 'system': ['bricks_cmake_checks.__system', 'bricks_cmake_core.__system', 'bricks_cmake_groups.__system'], 'platform': 'cip_build_system_platforms.__system', 'deploy': ['asio.generic', 'ecal.measurement', 'hdf5.generic', 'mts-core-lib', 'mts-sentinel', 'recycle.generic', 'simpleini.generic', 'tclap.generic', 'tcp_pubsub.generic', 'tracy'], 'all': ['VFR_Utilities.generic', 'asio.generic', 'bricks_cmake_checks.__system', 'bricks_cmake_core.__system', 'bricks_cmake_groups.__system', 'cip_adaption_boost.generic', 'cip_build_system_platforms.__system', 'cmake', 'ecal.measurement', 'flatbuffers', 'hdf5.generic', 'jsoncpp.generic', 'make', 'mts-core-lib', 'mts-sentinel', 'opencv.generic', 'protobuf', 'recycle.generic', 'simpleini.generic', 'spdlog.generic', 'tclap.generic', 'tcp_pubsub.generic', 'tracy']}, 'linux64_x86_gcc_7-v2': {'deploy': ['asio.generic', 'ecal.measurement', 'hdf5.generic', 'mts-core-lib', 'mts-sentinel', 'recycle.generic', 'simpleini.generic', 'tclap.generic', 'tcp_pubsub.generic', 'tracy'], 'production': ['VFR_Utilities.generic', 'asio.generic', 'cip_adaption_boost.generic', 'cmake', 'ecal.measurement', 'flatbuffers', 'hdf5.generic', 'jsoncpp.generic', 'make', 'mts-core-lib', 'mts-sentinel', 'opencv.generic', 'protobuf', 'recycle.generic', 'simpleini.generic', 'spdlog.generic', 'tclap.generic', 'tcp_pubsub.generic', 'tracy'], 'build': ['VFR_Utilities.generic', 'bricks_cmake_checks.__system', 'bricks_cmake_core.__system', 'bricks_cmake_groups.__system', 'cip_adaption_boost.generic', 'cip_build_system_platforms.__system', 'cmake', 'flatbuffers', 'jsoncpp.generic', 'make', 'opencv.generic', 'protobuf', 'spdlog.generic'], 'system': ['bricks_cmake_checks.__system', 'bricks_cmake_core.__system', 'bricks_cmake_groups.__system'], 'platform': 'cip_build_system_platforms.__system', 'all': ['VFR_Utilities.generic', 'asio.generic', 'bricks_cmake_checks.__system', 'bricks_cmake_core.__system', 'bricks_cmake_groups.__system', 'cip_adaption_boost.generic', 'cip_build_system_platforms.__system', 'cmake', 'ecal.measurement', 'flatbuffers', 'hdf5.generic', 'jsoncpp.generic', 'make', 'mts-core-lib', 'mts-sentinel', 'opencv.generic', 'protobuf', 'recycle.generic', 'simpleini.generic', 'spdlog.generic', 'tclap.generic', 'tcp_pubsub.generic', 'tracy']}, 'vs2017': {'deploy': ['asio.generic', 'ecal.measurement', 'hdf5.generic', 'mts-core-lib', 'mts-sentinel', 'recycle.generic', 'simpleini.generic', 'tclap.generic', 'tcp_pubsub.generic', 'tracy'], 'production': ['VFR_Utilities.generic', 'asio.generic', 'cip_adaption_boost.generic', 'cmake', 'ecal.measurement', 'flatbuffers', 'hdf5.generic', 'jsoncpp.generic', 'make', 'mts-core-lib', 'mts-sentinel', 'opencv.generic', 'protobuf', 'recycle.generic', 'simpleini.generic', 'spdlog.generic', 'tclap.generic', 'tcp_pubsub.generic', 'tracy'], 'build': ['VFR_Utilities.generic', 'bricks_cmake_checks.__system', 'bricks_cmake_core.__system', 'bricks_cmake_groups.__system', 'cip_adaption_boost.generic', 'cip_build_system_platforms.__system', 'cmake', 'flatbuffers', 'jsoncpp.generic', 'make', 'opencv.generic', 'protobuf', 'spdlog.generic'], 'system': ['bricks_cmake_checks.__system', 'bricks_cmake_core.__system', 'bricks_cmake_groups.__system'], 'platform': 'cip_build_system_platforms.__system', 'all': ['VFR_Utilities.generic', 'asio.generic', 'bricks_cmake_checks.__system', 'bricks_cmake_core.__system', 'bricks_cmake_groups.__system', 'cip_adaption_boost.generic', 'cip_build_system_platforms.__system', 'cmake', 'ecal.measurement', 'flatbuffers', 'hdf5.generic', 'jsoncpp.generic', 'make', 'mts-core-lib', 'mts-sentinel', 'opencv.generic', 'protobuf', 'recycle.generic', 'simpleini.generic', 'spdlog.generic', 'tclap.generic', 'tcp_pubsub.generic', 'tracy']}}
            requirements = {'ubuntu_20_04_x86_64-release': {'build': {'VFR_Utilities.generic': 'VFR_Utilities.generic/1.2.0@cip/releases', 'protobuf': 'protobuf/3.11.4-tb8@3rdparty/releases', 'jsoncpp.generic': 'jsoncpp.generic/1.9.5-1@cip/releases', 'spdlog.generic': 'spdlog.generic/1.12.0@cip/releases', 'cip_adaption_boost.generic': 'cip_adaption_boost.generic/1.80.0-release3@cip/releases', 'flatbuffers': 'flatbuffers/2.0.6-tb2@3rdparty/releases', 'opencv.generic': 'opencv.generic/4.5.5-4@cip/releases', 'bricks_cmake_groups.__system': 'bricks_cmake_groups.__system/1.12.1@cip/releases', 'bricks_cmake_core.__system': 'bricks_cmake_core.__system/1.4.0@cip/releases', 'cmake': 'cmake/3.25.0@3rdparty/releases', 'cip_build_system_platforms.__system': 'cip_build_system_platforms.__system/3.55.5@cip/releases', 'make': 'make/4.4.1@3rdparty/releases', 'bricks_cmake_checks.__system': 'bricks_cmake_checks.__system/1.8.0@cip/releases'}, 'deploy': {'ecal.measurement': 'ecal.measurement/5.13.0-rc6-c72f8b7@cip/releases', 'asio.generic': 'asio.generic/1.28.1@cip/releases', 'tcp_pubsub.generic': 'tcp_pubsub.generic/1.0.4@cip/releases', 'recycle.generic': 'recycle.generic/7.0.0@cip/releases', 'simpleini.generic': 'simpleini.generic/4.20.0@cip/releases', 'hdf5.generic': 'hdf5.generic/1.14.1@cip/releases', 'tclap.generic': 'tclap.generic/1.2.5@cip/releases', 'mts-core-lib': 'mts-core-lib/0.9.8@3rdparty/releases', 'tracy': 'tracy/0.9.0-tb4@3rdparty/releases', 'mts-sentinel': 'mts-sentinel/0.2.2@3rdparty/releases'}}, 'linux64_x86_gcc_7-v2-release': {'build': {'cip_adaption_boost.generic': 'cip_adaption_boost.generic/1.80.0-release3@cip/releases', 'opencv.generic': 'opencv.generic/4.5.5-4@cip/releases', 'VFR_Utilities.generic': 'VFR_Utilities.generic/1.2.0@cip/releases', 'protobuf': 'protobuf/3.11.4-tb8@3rdparty/releases', 'flatbuffers': 'flatbuffers/2.0.6-tb2@3rdparty/releases', 'jsoncpp.generic': 'jsoncpp.generic/1.9.5-1@cip/releases', 'spdlog.generic': 'spdlog.generic/1.12.0@cip/releases', 'bricks_cmake_groups.__system': 'bricks_cmake_groups.__system/1.12.1@cip/releases', 'bricks_cmake_core.__system': 'bricks_cmake_core.__system/1.4.0@cip/releases', 'cmake': 'cmake/3.25.0@3rdparty/releases', 'cip_build_system_platforms.__system': 'cip_build_system_platforms.__system/3.55.5@cip/releases', 'make': 'make/4.4.1@3rdparty/releases', 'bricks_cmake_checks.__system': 'bricks_cmake_checks.__system/1.8.0@cip/releases'}, 'deploy': {'mts-core-lib': 'mts-core-lib/0.9.8@3rdparty/releases', 'tracy': 'tracy/0.9.0-tb4@3rdparty/releases', 'ecal.measurement': 'ecal.measurement/5.13.0-rc6-c72f8b7@cip/releases', 'recycle.generic': 'recycle.generic/7.0.0@cip/releases', 'asio.generic': 'asio.generic/1.28.1@cip/releases', 'tcp_pubsub.generic': 'tcp_pubsub.generic/1.0.4@cip/releases', 'simpleini.generic': 'simpleini.generic/4.20.0@cip/releases', 'hdf5.generic': 'hdf5.generic/1.14.1@cip/releases', 'tclap.generic': 'tclap.generic/1.2.5@cip/releases', 'mts-sentinel': 'mts-sentinel/0.2.2@3rdparty/releases'}}, 'ubuntu_20_04_x86_64': {'build': {'protobuf': 'protobuf/3.11.4-tb8@3rdparty/releases', 'spdlog.generic': 'spdlog.generic/1.12.0@cip/releases', 'cip_adaption_boost.generic': 'cip_adaption_boost.generic/1.80.0-release3@cip/releases', 'flatbuffers': 'flatbuffers/2.0.6-tb2@3rdparty/releases', 'opencv.generic': 'opencv.generic/4.5.5-4@cip/releases', 'VFR_Utilities.generic': 'VFR_Utilities.generic/1.2.0@cip/releases', 'jsoncpp.generic': 'jsoncpp.generic/1.9.5-1@cip/releases', 'bricks_cmake_groups.__system': 'bricks_cmake_groups.__system/1.12.1@cip/releases', 'bricks_cmake_core.__system': 'bricks_cmake_core.__system/1.4.0@cip/releases', 'cmake': 'cmake/3.25.0@3rdparty/releases', 'cip_build_system_platforms.__system': 'cip_build_system_platforms.__system/3.55.5@cip/releases', 'make': 'make/4.4.1@3rdparty/releases', 'bricks_cmake_checks.__system': 'bricks_cmake_checks.__system/1.8.0@cip/releases'}, 'deploy': {'mts-core-lib': 'mts-core-lib/0.9.8@3rdparty/releases', 'tracy': 'tracy/0.9.0-tb4@3rdparty/releases', 'mts-sentinel': 'mts-sentinel/0.2.2@3rdparty/releases', 'ecal.measurement': 'ecal.measurement/5.13.0-rc6-c72f8b7@cip/releases', 'tclap.generic': 'tclap.generic/1.2.5@cip/releases', 'asio.generic': 'asio.generic/1.28.1@cip/releases', 'recycle.generic': 'recycle.generic/7.0.0@cip/releases', 'simpleini.generic': 'simpleini.generic/4.20.0@cip/releases', 'tcp_pubsub.generic': 'tcp_pubsub.generic/1.0.4@cip/releases', 'hdf5.generic': 'hdf5.generic/1.14.1@cip/releases'}}, 'vs2017_debug': {'build': {'flatbuffers': 'flatbuffers/2.0.6-tb2@3rdparty/releases', 'jsoncpp.generic': 'jsoncpp.generic/1.9.5-1@cip/releases', 'spdlog.generic': 'spdlog.generic/1.12.0@cip/releases', 'cip_adaption_boost.generic': 'cip_adaption_boost.generic/1.80.0-release3@cip/releases', 'protobuf': 'protobuf/3.11.4-tb8@3rdparty/releases', 'opencv.generic': 'opencv.generic/4.5.5-4@cip/releases', 'VFR_Utilities.generic': 'VFR_Utilities.generic/1.2.0@cip/releases', 'bricks_cmake_groups.__system': 'bricks_cmake_groups.__system/1.12.1@cip/releases', 'bricks_cmake_core.__system': 'bricks_cmake_core.__system/1.4.0@cip/releases', 'cmake': 'cmake/3.25.0@3rdparty/releases', 'cip_build_system_platforms.__system': 'cip_build_system_platforms.__system/3.55.5@cip/releases', 'make': 'make/4.4.1@3rdparty/releases', 'bricks_cmake_checks.__system': 'bricks_cmake_checks.__system/1.8.0@cip/releases'}, 'deploy': {'mts-core-lib': 'mts-core-lib/0.9.8@3rdparty/releases', 'tracy': 'tracy/0.9.0-tb4@3rdparty/releases', 'ecal.measurement': 'ecal.measurement/5.13.0-rc6-c72f8b7@cip/releases', 'recycle.generic': 'recycle.generic/7.0.0@cip/releases', 'asio.generic': 'asio.generic/1.28.1@cip/releases', 'simpleini.generic': 'simpleini.generic/4.20.0@cip/releases', 'tclap.generic': 'tclap.generic/1.2.5@cip/releases', 'hdf5.generic': 'hdf5.generic/1.14.1@cip/releases', 'tcp_pubsub.generic': 'tcp_pubsub.generic/1.0.4@cip/releases', 'mts-sentinel': 'mts-sentinel/0.2.2@3rdparty/releases'}}, 'linux64_x86_gcc_7-v2': {'build': {'spdlog.generic': 'spdlog.generic/1.12.0@cip/releases', 'protobuf': 'protobuf/3.11.4-tb8@3rdparty/releases', 'flatbuffers': 'flatbuffers/2.0.6-tb2@3rdparty/releases', 'cip_adaption_boost.generic': 'cip_adaption_boost.generic/1.80.0-release3@cip/releases', 'opencv.generic': 'opencv.generic/4.5.5-4@cip/releases', 'VFR_Utilities.generic': 'VFR_Utilities.generic/1.2.0@cip/releases', 'jsoncpp.generic': 'jsoncpp.generic/1.9.5-1@cip/releases', 'bricks_cmake_groups.__system': 'bricks_cmake_groups.__system/1.12.1@cip/releases', 'bricks_cmake_core.__system': 'bricks_cmake_core.__system/1.4.0@cip/releases', 'cmake': 'cmake/3.25.0@3rdparty/releases', 'cip_build_system_platforms.__system': 'cip_build_system_platforms.__system/3.55.5@cip/releases', 'make': 'make/4.4.1@3rdparty/releases', 'bricks_cmake_checks.__system': 'bricks_cmake_checks.__system/1.8.0@cip/releases'}, 'deploy': {'mts-core-lib': 'mts-core-lib/0.9.8@3rdparty/releases', 'tracy': 'tracy/0.9.0-tb4@3rdparty/releases', 'mts-sentinel': 'mts-sentinel/0.2.2@3rdparty/releases', 'ecal.measurement': 'ecal.measurement/5.13.0-rc6-c72f8b7@cip/releases', 'tcp_pubsub.generic': 'tcp_pubsub.generic/1.0.4@cip/releases', 'recycle.generic': 'recycle.generic/7.0.0@cip/releases', 'asio.generic': 'asio.generic/1.28.1@cip/releases', 'hdf5.generic': 'hdf5.generic/1.14.1@cip/releases', 'simpleini.generic': 'simpleini.generic/4.20.0@cip/releases', 'tclap.generic': 'tclap.generic/1.2.5@cip/releases'}}, 'vs2017': {'build': {'protobuf': 'protobuf/3.11.4-tb8@3rdparty/releases', 'spdlog.generic': 'spdlog.generic/1.12.0@cip/releases', 'VFR_Utilities.generic': 'VFR_Utilities.generic/1.2.0@cip/releases', 'flatbuffers': 'flatbuffers/2.0.6-tb2@3rdparty/releases', 'opencv.generic': 'opencv.generic/4.5.5-4@cip/releases', 'cip_adaption_boost.generic': 'cip_adaption_boost.generic/1.80.0-release3@cip/releases', 'jsoncpp.generic': 'jsoncpp.generic/1.9.5-1@cip/releases', 'bricks_cmake_groups.__system': 'bricks_cmake_groups.__system/1.12.1@cip/releases', 'bricks_cmake_core.__system': 'bricks_cmake_core.__system/1.4.0@cip/releases', 'cmake': 'cmake/3.25.0@3rdparty/releases', 'cip_build_system_platforms.__system': 'cip_build_system_platforms.__system/3.55.5@cip/releases', 'make': 'make/4.4.1@3rdparty/releases', 'bricks_cmake_checks.__system': 'bricks_cmake_checks.__system/1.8.0@cip/releases'}, 'deploy': {'mts-core-lib': 'mts-core-lib/0.9.8@3rdparty/releases', 'tracy': 'tracy/0.9.0-tb4@3rdparty/releases', 'ecal.measurement': 'ecal.measurement/5.13.0-rc6-c72f8b7@cip/releases', 'tclap.generic': 'tclap.generic/1.2.5@cip/releases', 'recycle.generic': 'recycle.generic/7.0.0@cip/releases', 'hdf5.generic': 'hdf5.generic/1.14.1@cip/releases', 'simpleini.generic': 'simpleini.generic/4.20.0@cip/releases', 'asio.generic': 'asio.generic/1.28.1@cip/releases', 'tcp_pubsub.generic': 'tcp_pubsub.generic/1.0.4@cip/releases', 'mts-sentinel': 'mts-sentinel/0.2.2@3rdparty/releases'}}}
            # We assume that packages with bricks_platform_name setting are production packages
            # Packages without bricks_platform_name setting are system packages
            bricks_platform_name = self.settings.get_safe("bricks_platform_name")

            if bricks_platform_name is not None:
                self.__cip_binary_dependencies = binary_dependencies.get(
                    str(bricks_platform_name),
                    None
                )
                if self.__cip_binary_dependencies is None:
                    raise RuntimeError(
                        '{0} is not built with bricks_platform_name {1}'.format(
                            self.name,
                            bricks_platform_name,
                        )
                    )
                self.__cip_requirements = requirements.get(
                    str(bricks_platform_name),
                    None
                )
            else:
                for platform_name in binary_dependencies.keys():
                    self.__cip_binary_dependencies = binary_dependencies[platform_name]
                    self.__cip_requirements = requirements[platform_name]


    @property
    def cip_requirements(self):
        """return Bricks requirements"""

        self.__bricks_init()
        return self.__cip_requirements

    @property
    def cip_binary_dependencies(self):
        """return Bricks binary dependencies"""
        self.__bricks_init()
        return self.__cip_binary_dependencies

    @staticmethod
    def __create_h_folder_readme(folder, folder1 = None, folder2 = None):
        """
        Writes a readme file into the given folder with some
        description about the meaning of the "h" folder
        and its linking

        if you specify two folder names then some
        text will be added to the README.txt about the
        links between those.

        @param folder1 Optional folder name so that we can
                        add some more text which folder
                        links where.
        @param folder2 Optional folder name two.
        """

        f = folder.joinpath("README.txt").open("w")
        f.write("""\
The 'h' folder contains links to the more human readable
build folders done with a Windows directory junction.

We are doing this to reduce the path lengths during builds
so that we don't hit the Windows path length limit so
quickly.
This is why you will see only the 'h' folders in the build
logs.

If we come up with a better solution we might remove it
again so don't rely on it.
""")

        if folder1 and folder2:
            f.write("This is a linked folder (directory " +
                "junction) and links\n " +
                "  " +str(folder1) + " with\n  " + str(folder2) + "\n")

        f.close()


    def shortened_path_if_windows(self, input_path: pathlib.Path):
        """
        calculate location of directory junction if needed
        in Windows platforms
        """
        input_path = pathlib.Path(input_path)
        output_path = input_path

        # for windows, create hashed link folder
        if os.name == 'nt':

            # first, calculate hash code of
            # our own build folder
            input_path_hash = hashlib.md5(input_path.as_posix().encode())

            # conan workarea is parent of conan build_folder
            conan_workarea = pathlib.Path(
                self.build_folder
            ).parent.resolve()
            # take the parent of build folder
            # since all build folders are located
            # in conan workarea
            output_path = conan_workarea.joinpath(
                'h',
                input_path_hash.hexdigest()
            )
            if len(str(output_path)) > len(str(input_path)):
                output_path = input_path
        return output_path

    def __create_directory_junction(
            self,
            source_dir: pathlib.Path,
            target_dir: typing.Optional[pathlib.Path]
        ) -> pathlib.Path:
        """
        create directory junction between source_dir and
        target_dir.

        :param source_dir: path to hashed source directory
        :param target_dir: cip_build symbolic link
        """

        source_dir = pathlib.Path(source_dir)

        target_dir = pathlib.Path(target_dir)


        # create source directory if it does not
        # exists already
        if not source_dir.exists():
            source_dir.mkdir(parents=True)

        if not source_dir.exists():
            self.output.warn(
                'Cannot create directory junktion since {} does not found'.format(
                    source_dir
                )
            )

        # if source dir and target directory are same
        # or if source_dir does not exist then making
        # link makes no sense
        if source_dir != target_dir and source_dir.exists():

            # make parent folder for symlink
            if not target_dir.parent.exists():
                target_dir.parent.mkdir(mode=755, parents=True)

            # use mklink to create directory junction
            # The use of shell is needed so everything
            # works better in Python virtual environment
            # which may not have access to windows APIs
            mklink_args = [
                'cmd',
                '/C',
                'mklink',
                '/J',
                str(target_dir.name),
                str(source_dir),
            ]
            self.output.info(
                'Creating symlink source={source}, target={target}, cwd={cwd}, args={prog_args}'.format(
                    source=source_dir,
                    target=target_dir.name,
                    cwd=str(target_dir.parent),
                    prog_args=mklink_args,
                )
            )
            res = subprocess.run(
                mklink_args,
                # two linking may happen same
                # time so failure may happen
                # due to race condition
                check=False,
                # we call shell explicitly
                # with cmd
                shell=False,
                cwd=str(target_dir.parent)
            )

            # there might be race-condition
            # so code may fail if two entries
            # are calculated same time so
            # we check actual output instead
            # of command itself
            if not target_dir.exists():
                raise RuntimeError(
                    'Cannot create link {source} => {target}'.format(
                        source=str(source_dir),
                        target=str(target_dir),
                    )
                )

        return target_dir

    def __get_package_overrides(self):
        ''' Get's the CIP_PACKAGE_OVERRIDE environment
            variable and return it as a nicely split up
            dict
        '''

        # load entity data first
        entity = self.read_selected_entity_json()
        binary_overrides = entity.get('__build', {}).get('binary_overrides', {})

        # conan workarea is parent of conan build_folder
        conan_workarea = pathlib.Path(
            __file__
        ).parent.parent.resolve()

        packages = {}
        for key, path in binary_overrides.items():
            path = pathlib.Path(path)
            if not path.is_absolute():
                path = conan_workarea / path
            packages[key] = path.as_posix()

        return packages

    def requirements(self):
        ''' Add 'deploy' requirements to system

            Calls conan.requires to add 'normal' requirements.
            Considers binary_overrides.
        '''
        cip_binary_overrides = self.__get_package_overrides()

        for key, package_name in self.cip_requirements.get('deploy', {}).items():
            # add dependency to package
            self.output.info('Adding package {0}'.format(package_name))

            # binary overrides are honored here already
            if key in cip_binary_overrides:
                continue
            self.requires(package_name)

    def build_requirements(self):
        ''' Add 'deploy' requirements to system

            Calls conan.requires to add 'normal' requirements.
            Considers binary_overrides.
        '''
        cip_binary_overrides = self.__get_package_overrides()

        for key, package_name in self.cip_requirements.get('build', {}).items():
            # add dependency to package
            self.output.info('Adding package {0}'.format(package_name))

            # binary overrides are honored here already
            if key in cip_binary_overrides:
                continue
            self.build_requires(package_name)

    def __get_cmake_executable(self):
        """ Returns the path to the cmake executable.

            Looks up the path to the cmake dependency and
            returns a path of the cmake contained in that package.

            TODO: why a loop
        """
        for binary_key, dep_info in self.deps_cpp_info.dependencies:
            if binary_key == 'cmake':
                return os.path.join(
                    dep_info.rootpath,
                    "bin",
                    "cmake"
                )
        return 'cmake'

    def __add_ninja_generator_arguments(self, cmake_kwargs, cmake_args):
        ''' Return ninja CMake arguments and keywords if needed

            Adds:
            - generator Ninja to kwargs
            - CMAKE_MAKE_PROGRAM ninja binary path

            TODO: why a loop
        '''

        for binary_key, dep_info in self.deps_cpp_info.dependencies:
            if binary_key == 'ninja':
                ninja_binary = os.path.join(
                    dep_info.rootpath,
                    "ninja"
                )
                # tell explicitly to use specific generator
                # if user has defined the dependency to ninja build
                # system define
                cmake_kwargs['generator'] = 'Eclipse CDT4 - Ninja'
                # define exact path for the ninja generator
                cmake_args.extend(
                    [
                        '-DCMAKE_MAKE_PROGRAM:FILEPATH={0}'.format(
                            ninja_binary),
                    ]
                )

    def __add_make_generator_arguments(self, cmake_kwargs, cmake_args):
        ''' Return make CMake arguments and keywords if needed

            Adds:
            - CMAKE_MAKE_PROGRAM make binary path

        '''

        for binary_key, dep_info in self.deps_cpp_info.dependencies:
            if binary_key == 'make':
                make_binary = os.path.join(
                    dep_info.rootpath,
                    "make"
                )
                # define exact path for the make generator
                cmake_args.extend(
                    [
                        '-DCMAKE_MAKE_PROGRAM:FILEPATH={0}'.format(
                            make_binary),
                    ]
                )
    def __get_build_type(self):
        """ Returns the build type to be used.
            In some cases this might be None, so you'll
            have to check.

            In order to do this we need to first check if
            CMAKE_BUILD_TYPE environment variable is set
            to overwrite the build type from the platform.
        """
        # first check environment variables if build type was set.
        cmake_build_type = os.environ.get('CMAKE_BUILD_TYPE')
        if cmake_build_type:
            return cmake_build_type
        else:
            return self.settings.get_safe('build_type')

    def __add_build_type_arguments(self, cmake_args):
        """ Adds the build-type args for a cmake call.
        """

        # fix SD-46923
        # You need to set both BUILD_TYPE and CONFIGURATION_TYPES. See
        # https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#build-configurations
        cmake_build_type = self.__get_build_type()
        if cmake_build_type:
            cmake_args += [
                '-DCMAKE_BUILD_TYPE={0}'.format(
                    str(cmake_build_type)),
                '-DCMAKE_CONFIGURATION_TYPES={0}'.format(
                    str(cmake_build_type))
            ]

    def __add_additional_arguments(self, cmake_args):
        """ Adds the additional arguments from --cmake-option for a cmake call.
        """
        cmake_option = os.environ.get('CMAKE_OPTION')
        if cmake_option:
            options = [base64.b64decode(bytes(x, 'ascii')).decode('ascii')
                for x in cmake_option.split(';')]
            cmake_args += options

    def __get_ctest_executable(self):
        """ Returns the path to the ctest executable which is part of cmake. """
        for binary_key, dep_info in self.deps_cpp_info.dependencies:
            if binary_key == 'cmake':
                return os.path.join(
                    dep_info.rootpath,
                    "bin",
                    "ctest"
                )
        return 'ctest'

    def __report_multipackage_error(self, error_msg: str):
        """
        throw exception and instructions for
        user how to deal with the multipackage
        build error
        """
        entity = self.read_selected_entity_json()

        entity_bricks_version = entity.get("execution").get(
            "bricks_version"
        )
        instruction_template = "Please add package {package_name} to build with --build-config {build_config}"
        error_message_template = ". ".join(
            [
                "ERROR: {entity_name}/{version} was built Bricks version {old_version} which is not compatible with {bricks_version} in multi-package builds, which causes {error_msg}",
                instruction_template
            ]
        )
        build_config = str(
            pathlib.Path(self.source_folder).resolve().joinpath(
                'conf/build.yml'
            )
        )

        self.output.info(
            instruction_template.format(
                package_name=self.cip_package_name,
                build_config=build_config,
            )
        )

        raise RuntimeError(
            error_message_template.format(
                # FIX: SD-47243 add detailed error message
                error_msg=error_msg,
                entity_name=self.name,
                bricks_version=self.CIP_BUILD_SYSTEM_VERSION,
                old_version=entity_bricks_version,
                version=self.version,
                package_name=self.cip_package_name,
                build_config=build_config,
            )
        )

    def __check_tagged_path_converter(self, root_path):
        """
        check if we must terminate build for the missing
        tagget path converter

        RelateD: SD-46867
        """
        # fix: SD-46867 give better error message
        # to user
        converter_script = pathlib.Path(
            root_path,
            'cmake/pre/scripts/tagged_paths_json_to_cmake.py'
        )
        if not converter_script.exists():
            self.__report_multipackage_error(
                'script {}  not to be found'.format(converter_script)
            )

    def __get_bricks_core_args(self):
        """ calculate additional args needed
        for CMake build
        """
        cmake_args = []
        resolved_paths = self.__get_package_lib_paths()
        core_system_module = 'bricks_cmake_core.__system'

        # try to find the bricks_cmake_core module
        for binary_key, root_path in resolved_paths.items():
            if binary_key == core_system_module:

                # validate that old package was built
                # recently enough
                self.__check_tagged_path_converter(root_path)

                pre_project_hook = os.path.join(
                    root_path,
                    'cmake',
                    'bricks_project_include_before.cmake'
                )
                post_project_hook = os.path.join(
                    root_path,
                    'cmake',
                    'bricks_project_include.cmake'
                )
                cmake_args += [
                    '-DCMAKE_PROJECT_INCLUDE:FILEPATH={0}'.format(
                        post_project_hook
                    ),
                    '-DCMAKE_PROJECT_INCLUDE_BEFORE:FILEPATH={0}'.format(
                        pre_project_hook
                    )
                ]
                # no need to go any further
                break
        # core module was not found. Check if we should have had it or not
        if not cmake_args and self.should_use_bricks_core:
            # FIX: SD-47243
            # give better error message in case of multipackage
            # build
            self.__report_multipackage_error(
                'dependency bricks_cmake_core  not to be found'
            )

        return cmake_args


    def __cmake_args_bricks_platform_name(self):
        """ Return Bricks platform name to Cmake

            We have two:
            - BRICKS_PLATFORM_NAME (deprecated)
            - BRICKS_BUILD_PLATFORM_NAME

            TODO: parameters of this function does not align with cmake_add_build_type_arguments
        """

        # the bricks_platform_name comes from the settings, so it
        # might be empty in case of target_platform: any or a tools
        # build. See conanfile_generator.py
        build_platform_name = self.settings.get_safe("bricks_platform_name")
        if not build_platform_name:
            build_platform_name = self.build_platform

        return ['-DBRICKS_PLATFORM_NAME:STRING={}'.format(build_platform_name),
                '-DBRICKS_BUILD_PLATFORM_NAME:STRING={}'.format(build_platform_name)]


    def __get_user_credential_helper(self):
        """get credential.helper which user has defined

        Returns:
            credential.helper from git config
        """

        credentials_ok = False
        # check username which user has configured for host
        shell_args = [
            'git',
            'config',
            # please note we use user global configuration here!
            '--global',
            '--get',
            'credential.username'
        ]
        res = subprocess.run(
            shell_args,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            stdin=subprocess.PIPE,
            check=False,
            shell=False,
            universal_newlines=True
        )
        if res.returncode == 0:
            git_username = res.stdout.strip()
            self.output.info(
                'Git configuration defines username {0}'.format(
                    git_username
                )
            )

            # then check the helper program user may have defined
            shell_args = [
                'git',
                'config',
                # please note we use user global configuration here!
                '--global',
                '--get',
                'credential.helper'
            ]
            res = subprocess.run(
                shell_args,
                stdout=subprocess.PIPE,
                stderr=subprocess.PIPE,
                stdin=subprocess.PIPE,
                check=False,
                shell=False,
                universal_newlines=True
            )
            if res.returncode == 0:
                credential_helper = res.stdout.strip()
                helper_style = credential_helper.split()[0]
                if (
                    helper_style == 'store' or (
                        helper_style == 'manager' and
                        not os.environ.get('JENKINS_HOME')
                    )
                ):
                    credentials_ok = True
        return credentials_ok

    def __jenkins_credential_injection(self, repository):
        """Jenkins credential override. This is needed for review-with
        builds in Jenkins

        WARNING: This is not maintainable solution

        :param repository: repository url which is replaced in Jenkins
        :type repository:  str

        :return:    github repository which is using https with github
                    credentials in url
        """

        git_user = os.environ.get('GIT_USER')
        git_password = os.environ.get('GIT_PASSWORD')
        jenkins_url = os.environ.get('JENKINS_URL')

        prefixes = [
            'http://github.conti.de/',
            'https://github.conti.de/',
            'http://github.geo.conti.de/',
            'https://github.geo.conti.de/',
            'http://github-am.geo.conti.de/',
            'https://github-am.geo.conti.de/',
            # use https also for normal checkouts if
            # review-with is used
            'git@github.conti.de:',
            'git@github.geo.conti.de:',
            'git@github-am.geo.conti.de:',
        ]

        if git_user and git_password and jenkins_url:
            replacement = 'https://{git_user}:{git_password}@github-am.geo.conti.de/'.format(
                git_user=git_user,
                git_password=git_password,
            )
            for prefix in prefixes:
                # if repository name matches then replace
                # the repository with our replacement part
                if repository.startswith(prefix):
                    repository = replacement + repository[len(prefix):]
                    self.output.info(
                        'Replaced repository to {}'.format(
                            repository
                        )
                    )
                    break
        return repository

    def log_event(self, is_start, step_id, step_name, step_subname, message = ''):
        ''' Print out timestamp event messages for performance meassurements.

            See SVPT-19688

            :param is_start: True if start, False if end
            :param step_id: A unique number for the step (int)
                Has to be the same for start and stop events.
            :param step_name: Name for the step. (string).
                Not required for stop events.
            :param step_subname: Sub-Name for the step. Can be empty string.
                Not required for stop events.
            :param message: Freetext for the step. Empty string by default.
                Not required for stop events.

            :note: the event does not gets printed when the CONAN_RECIPE_HIDE_CUSTOMEVENTS
                environment variable is set to "1"

        '''

        hide_customevents_str = os.environ.get('CONAN_RECIPE_HIDE_CUSTOMEVENTS')
        hide_customevents = hide_customevents_str == "1"

        if not hide_customevents:
            # we use utc in case we want to print out date - timestamps is always utc anyway
            dt = datetime.now(tz=timezone.utc)
            # timestamp in ms
            timestamp = round(dt.timestamp() * 1e3)

            if message == '':
                message = self.name + '[' + self.target_platform +']'

            if is_start:
                template = 'customevent.duration.v2|{step_time}|start|{step_id}|{step_name}|{step_subname}|{message}|'
            else:
                template = 'customevent.duration.v2|{step_time}|end|{step_id}|'

            # calling a tool seems to be the only sensible way
            # to get a message to show up in the log file.
            self.run('echo "' + template.format(
                step_id=self.event_id + step_id,
                step_name=step_name,
                step_subname=step_subname,
                message=message.replace('|', ''),
                step_time=str(timestamp)
            ) + '"', False)

    def source(self):
        ''' Clone repository based on used configuration

            Later on this will be expanded with other repositories more
            than likely

            Note: the scm feature
            https://docs.conan.io/en/latest/reference/conanfile/attributes.html#scm
            is marked as experimental.

            We are doing this ourselves.
        '''
        self.log_event(True, 1, 'bricks_conanfile', 'source')

        # for tests we do not check out the source code
        if self.cip_execution_method == 'test':
            return
        # I guess we don't need this unless we are using the conan feature
        scm = {'revision': '2f52aa75dab657567cd9f84356c501a0efeca809', 'repository': 'https://github-am.geo.conti.de/ADAS/next.git', 'branch': 'main'}
        scm_repository = 'https://github-am.geo.conti.de/ADAS/next.git'
        scm_revision = '2f52aa75dab657567cd9f84356c501a0efeca809'
        source_folder = self.source_folder

        # first, init source folder
        shell_args = [
            'git',
            'init',
            source_folder,
        ]
        self.run(' '.join(shell_args))

        scm_debug_template = """
*** CHECKING OUT:
    repository: {repository}
    revision:   {revision}
"""
        self.output.info(
            scm_debug_template.format(
                repository=scm_repository,
                revision=scm_revision
            )
        )
        # make injection to Jenkins but only if needed
        scm_repository = self.__jenkins_credential_injection(
            scm_repository,
        )

        # then, fetch sources we want to use
        # the clone is another alternative but fetch is bit
        # easier with incremental build which users more likely
        # prefer later on
        with tools.chdir(source_folder):
            debugged_commands = [
                [
                    'git',
                    'config',
                    '--list'
                ],
                [
                    'git',
                    'config',
                    'remote.origin.url',
                    scm_repository,
                ],
                [
                    'git',
                    'config',
                    '--add',
                    'remote.origin.fetch',
                    '+refs/heads/*:refs/remotes/origin/*',
                ],
                [
                    'git',
                    'config',
                    'core.longpaths',
                    'true',
                    '--bool',
                ],
                [
                    'git',
                    'fetch',
                    '--verbose',
                    '--progress',
                    '--tags',
                    'origin',
                ],
                [
                    'git',
                    'fetch',
                    '--verbose',
                    '--progress',
                    'origin',
                    scm_revision,
                ],
                [
                    'git',
                    'checkout',
                    '-f',
                    'FETCH_HEAD',
                ],
                [
                    'git',
                    'submodule',
                    'update',
                    '--init',
                    '--recursive',
                ],
            ]
            old_env = os.environ.copy()
            try:
                os.environ['GIT_ASKPASS'] = ''
                os.environ['GIT_TRACE'] = 'true'
                os.environ['GIT_TERMINAL_PROMPT'] = '0'
                os.environ['GCM_INTERACTIVE'] = 'never'
                os.environ['GIT_SSH_COMMAND'] = 'ssh -o StrictHostKeyChecking=no'
                for shell_args in debugged_commands:
                    self.output.info(
                        'Running {0}'.format(' '.join(shell_args))
                    )
                    self.run(
                        ' '.join(shell_args),
                        cwd=source_folder
                    )
                self.output.info('Checkout done')
            finally:
                os.environ.clear()
                os.environ.update(old_env)
        self.log_event(False, 1, 'bricks_conanfile', 'source')

    def __get_package_lib_paths(self):
        ''' Get paths to packages for uses and build_uses.

            Returns the path from binary_overrides or conan packages
            if no override.
            This is in principle the cmake module paths.
        '''

        # get all binary paths
        all_binary_keys = self.cip_binary_dependencies.get('all', [])
        conan_binary_keys = {
            binary_key: dep_info.rootpath
            for binary_key, dep_info in self.deps_cpp_info.dependencies
        }

        cip_binary_overrides = self.__get_package_overrides()

        resolved_paths = {
            # by default use binary overrides always
            binary_key: cip_binary_overrides.get(
                binary_key,
                # if binary key is not overridden then use
                # conan provided path
                conan_binary_keys.get(binary_key)
            )
            for binary_key in all_binary_keys
        }
        return resolved_paths

    def __overrides_tagged_paths(self, resolved_tagged_paths):
        ''' Add in binary_override configuration,
            which is valid in multi-package build
            to tagged_paths dict, if missing.
            solve PMTJSD-232582
        '''

        # override for tags only
        tag_list = ['all', 'production', 'deploy', 'build']
        # if package to override is not available add it
        binary_key_add = True

        # get packages to override
        cip_binary_overrides = self.__get_package_overrides()

        # for binary_key in cip_binary_overrides:
        #     # all_binary_keys.append(binary_key)
        #     resolved_taggeed_paths[binary_key]

        # check if package was configured
        # do not add package, if configuration was done
        for tag in tag_list:
            if tag in resolved_tagged_paths:
                tagged_paths = resolved_tagged_paths[tag]
                for tagged_binary in tagged_paths:
                    binary_key = tagged_binary['key']
                    if binary_key in cip_binary_overrides:
                        binary_key_add = False

        # add package to tagged_path so it can be found
        # find_package() needs entry in bricks_tagged_paths.json
        if cip_binary_overrides and binary_key_add:
            for tag in tag_list:
                for binary_key in cip_binary_overrides:
                    override = {
                        'key': binary_key,
                        'path': cip_binary_overrides[binary_key],
                    }
                    # check tagged path for binary_key and override
                    resolved_tagged_paths[tag].append(override)

        return resolved_tagged_paths

    def __calculate_tagged_paths(self):
        """calculate label paths

        This function uses __get_package_lib_paths which
        returns flat dictionary with "binary_key" as key
        and path as value. In essense, this dictionary
        tells for example where arith_add.generic
        is located on the disk.

        This method will read the tags and sort the
        directories based on tags (like production, testing).

        output should look something like
        {
            "production": [
                {
                    //key is binary key from
                    //conan package. This is
                    //something like arith_add.generic
                    //cmake etc
                    "key": "cmake",
                    //path where package root is located
                    //The path is either location in conan
                    //cache (normal case) or location
                    //to conan_workarea for depending package
                    // (multi-package build )
                    "path": "D:/.bbs_conan/095c78/1"
                },
                {
                    // one or more entries
                }
            ],
            "system": {
                //system package locations goes here
            },
            "platform": {
                //Platform package location
            }
            //etc
        }

        Purpose is to have controlled interface between
        different Bricks packages. Since the file
        is changed if dependencies changed this information
        can also be used to force reconfigure of project
        (i.e remove CMake cache files if depending package
        locations change due to changes in build.yml or
        added --build-config parameters
        )
        """
        # get all package library paths
        resolved_paths = self.__get_package_lib_paths()
        resolved_tagged_paths = {}

        def calculate_paths(binary_keys):
            """calculate objects for single tag"""
            tagged_paths = []
            for binary_key in binary_keys:
                path = pathlib.Path(resolved_paths[binary_key])
                tagged_paths.append(
                    {
                        'key': binary_key,
                        'path': path.as_posix(),
                    }
                )
            return tagged_paths

        # itereate over all tags
        for tag, binary_keys in self.cip_binary_dependencies.items():
            # convert platforms to
            # same format as every other tag
            # (it uses string instead of array)
            if type(binary_keys) == str:
                binary_keys = [binary_keys]
            # convert binary keys to paths which have
            # both path and key. This could be done
            # significantly simpler but that would mean
            # refactoring the source code
            resolved_tagged_paths[tag] = calculate_paths(
                binary_keys
            )

        resolved_tagged_paths = self.__overrides_tagged_paths(resolved_tagged_paths)

        return resolved_tagged_paths

    def __calculate_shortened_tagged_paths(self, resolved_tagged_paths):
        """
        calculate input files which are shortened using
        directory junktions
        """
        # convert the paths to shorter ones but keep also
        # the original path to make debugging slighly easier
        shortened_paths = {}
        for tag, tag_path_items in resolved_tagged_paths.items():

            new_path_items = shortened_paths.setdefault(
                tag,
                []
            )
            for tag_path_item in tag_path_items:
                original_path = tag_path_item['path']
                shortened_path = self.shortened_path_if_windows(
                    original_path
                )
                self.__create_directory_junction(
                    original_path,
                    shortened_path,
                )
                new_path_items.append(
                    {
                        'key': tag_path_item['key'],
                        'path': shortened_path.as_posix(),
                        'original_path': original_path,
                    }
                )
        return shortened_paths

    def __cmake_args_tagged_paths(self):
        """calculate arguments for cmake
        which are required for labels
        """
        resolved_tagged_paths = self.__calculate_tagged_paths()

        # dump tagged paths to disk as JSON so it is easy
        # to read by Lego team members.
        #
        # Since other
        # CIP teams have habit of using internal files
        # of Bricks, the file name should clearly indicate
        # this file is not meant to be used by other teams
        tagged_path_json = pathlib.Path(
            self.build_folder
        ).joinpath('bricks_tagged_paths.json')

        #with label_path_json.open('w') as stream:
        tagged_json_content = json.dumps(
            resolved_tagged_paths,
            # make reading bit more human friendly
            indent=4,
            # sort keys so labels are easier to find
            sort_keys=True,
        )

        new_checksum = self.__calculate_checksum_content(
            tagged_json_content
        )
        old_checksum = self.__calculate_checksum_file(
            tagged_path_json.as_posix()
        )

        if new_checksum != old_checksum:
            # if our paths are not matching
            # then we do want to remove
            # cache to force pristine
            # find_package queries on CMake
            # side
            self.remove_cmake_cache(
                'external package paths have been updated'
            )
            # write content also to disk
            with tagged_path_json.open('w') as stream:
                stream.write(tagged_json_content)



        # if we are not on Windows then we use
        # standard tagging file...
        json_passed_to_cmake = tagged_path_json

        # ... otherwise use shortened version
        # of paths
        if os.name == 'nt':
            shortened_json = pathlib.Path(
                self.build_folder
            ).joinpath('__internal_only_short_tagged_paths.json')

            # calculate content for
            # the shortened paths
            tagged_json_content = json.dumps(
                self.__calculate_shortened_tagged_paths(
                    resolved_tagged_paths
                ),
                indent=4,
                sort_keys=True,
            )
            # calculate checksum for content
            new_checksum = self.__calculate_checksum_content(
                tagged_json_content
            )
            # calculate checksum for file
            old_checksum = self.__calculate_checksum_file(
                shortened_json.as_posix()
            )

            if new_checksum != old_checksum:
                # if our paths are not matching
                # then we do want to remove
                # cache to force pristine
                # find_package queries on CMake
                # side
                self.remove_cmake_cache(
                    'external package paths have been updated'
                )
                # write content also to disk
                with shortened_json.open('w') as stream:
                    stream.write(tagged_json_content)

            # rewrite the file which is passed to CMake
            json_passed_to_cmake = shortened_json


        # define also output file which we want to
        # use for writing the CMake contentn
        bricks_tagged_paths_file = pathlib.Path(
            self.build_folder
        ).joinpath('__internal_only_bricks_tagged_paths.cmake')

        return [
            '-DBRICKS_LABEL_DEFINITION_FILE:FILEPATH={}'.format(
                bricks_tagged_paths_file.as_posix()
            ),
            '-DBRICKS_TAGGED_PATHS_JSON:FILEPATH={}'.format(
                json_passed_to_cmake.as_posix()
            )
        ]

    def __create_initial_cache(self, initial_cache_fname):
        """create initial cache definition which is passed
        to CMake
        """

        type_mapping = {
            'boolean': 'BOOL',
        }

        value_mapping = {
            True: 'ON',
            False: 'OFF',
        }

        # create cache lines with the initial spec
        # user defines
        cache_lines = [
            'set({name} "{value}" CACHE {type} "{doc}")'.format(
                name=opt_name,
                value=value_mapping.get(
                    opt_dict.get('value', ''),
                    opt_dict.get('value', '')
                ),
                type=type_mapping.get(
                    opt_dict.get('type'),
                    'STRING'
                ),
                doc=opt_dict.get('description', 'No description defined')
            )
            for opt_name, opt_dict in sorted(self.variant_options.items())
        ]
        cmake_package_version = self.version.split('-')[0]

        # add project name and
        # version to cache too
        cache_lines += [
            '# Default project name from build.yml',
            'set(DEFAULT_PROJECT_NAME "{}" CACHE STRING "Default project name")'.format(
                self.cip_package_name
            ),
            '# Default project version from cmd-line or config',
            'set(DEFAULT_PROJECT_VERSION "{}" CACHE STRING "Default project version")'.format(
                cmake_package_version
            ),
            '# Build type used',
            '# set(BUILD_TYPE "{}" CACHE STRING "Cmake build type")'.format(
                self.__get_build_type()
            ),
        ]

        cache_content = """
# initial cache values defined. Please do not edit manually
{lines}
""".format(lines='\n'.join(cache_lines))

        old_checksum = self.__calculate_checksum_file(
            initial_cache_fname
        )
        new_checksum = self.__calculate_checksum_content(cache_content)

        checksum_match = (old_checksum == new_checksum)
        if not checksum_match:
            with open(initial_cache_fname, 'w') as output_stream:
                output_stream.write(cache_content)
        return checksum_match

    def __cmake_args_initial_cache(self, initial_cache_fname):
        """calculate CMake arguments
        for initial cache
        """
        if not self.__create_initial_cache(initial_cache_fname):
            self.remove_cmake_cache(
                'options are updated'
            )

        cmake_args = [
            '-C',
            str(initial_cache_fname)
        ]
        return cmake_args

    def __cmake_args_platform_toolchain(self):
        """get full path used for platform file"""

        cmake_args = []
        fullpath_toolchain_file = None

        # if toolchain file is absolute then we do
        # not have to do any tweaks for relative paths
        if os.path.isabs(self.cmake_toolchain_file):
            fullpath_toolchain_file = self.cmake_toolchain_file
        else:
            tagged_paths = self.__calculate_tagged_paths()
            # find platform file
            # The paths which refer to platform
            # have tag "platform" and thus
            # we can use tagged paths to get the
            # information
            package_paths = tagged_paths.get(
                'platform',
                []
            )
            for key_path_dict in package_paths:
                fullpath_toolchain_file = os.path.join(
                    key_path_dict['path'],
                    '__system',
                    'cip_build_system_platforms',
                    'toolchains',
                    self.cmake_toolchain_file
                )

        if fullpath_toolchain_file:
            cmake_args += [
                '--toolchain', fullpath_toolchain_file
            ]
        return cmake_args

    def __get_module_search_paths(self, package_path_mapping):
        """get external package search path used by build"""

        build_binary_keys = self.cip_binary_dependencies.get(
            'build', []
        )

        deploy_binary_keys = self.cip_binary_dependencies.get(
            'deploy', []
        )
        native_normal_paths = [
            package_path_mapping[binary_key]
            for binary_key in build_binary_keys + deploy_binary_keys
        ]
        # convert paths to CMake format
        normal_paths = [
            path.replace(os.path.sep, '/')
            for path in native_normal_paths
        ]
        return normal_paths

    def __calculate_checksum_file(self, cmake_fname):
        '''get checksum for modified source'''

        digest = ''
        if os.path.exists(cmake_fname):
            with open(cmake_fname, 'r') as input_stream:
                digest = self.__calculate_checksum_content(
                    input_stream.read()
                )
        return digest

    def __calculate_checksum_content(self, content):
        '''calculate checksum for content'''
        checksum = hashlib.sha256()
        checksum.update(content.encode())
        return checksum.hexdigest()

    def deploy(self):
        '''copy all package files to directory
        if DEPLOY_CONAN_PACKAGE environment variable is set
        to positive number
        '''
        self.copy("*")  # copy from current package
        self.copy_deps("*")  # copy from dependencies

    def build(self):
        """execute test or build package"""

        sys.tracebacklimit = 0

        build_identifier = self.name
        name_prefix = self.settings.get_safe("bricks_platform_name")
        if name_prefix:
            build_identifier += '/' + name_prefix

        # inform about starting the build for certain variant
        self.output.info(
            'Executing {method} {id}'.format(
                method=self.cip_execution_method,
                id=build_identifier
            )
        )

        if self.cip_execution_method == 'build':
            return self.__execution_method_build()

        if self.cip_execution_method == 'test':
            return self.__execution_method_test()

        raise RuntimeError(
            "Invalid execution method {0}".format(
                self.cip_execution_method
            )
        )

    @property
    def report_directory(self):
        """ Reports should end up in the package folder """
        return os.path.join(
            self.package_folder,
            'reports'
        )

    def __execution_method_test(self):
        """ Method for executing test cases """

        self.log_event(True, 2, 'bricks_conanfile', 'test')
        # no build also means no test. SVPT-17876
        if not self.should_build:
            return

        package_path_mapping = self.__get_package_lib_paths()

        # FIXME: this should be changed by LEGO team in SVPT-41609
        tagged_paths = self.__calculate_tagged_paths()
        # find platform file
        # The paths which refer to platform
        # have tag "platform" and thus
        # we can use tagged paths to get the
        # information
        ctc_paths_dict = tagged_paths.get(
            'ctc',
            []
        )
        ctc_entity_paths = [entry.get("path", '') for entry in ctc_paths_dict]

        # get test locations
        testing_binary_keys = self.cip_binary_dependencies.get(
            'testing',
            []
        )

        # test scripts
        test_scripts = [
            os.path.join(
                package_path_mapping[binary_key],
                'tests',
                'ctest_main.cmake'
            )
            for binary_key in testing_binary_keys
        ]

        output_log_fname = os.path.join(
            self.build_folder,
            'test_execution.log'
        )
        # get search path used with build
        cmake_search_paths = ';'.join(
            self.__get_module_search_paths(
                package_path_mapping
            )
        )

        # if report directory exists then remove it
        # to keep reporting clean and shiny
        if os.path.exists(self.report_directory):
            self.output.info(
                'Removing old report directory {0}'.format(
                    self.report_directory
                )
            )
            shutil.rmtree(self.report_directory)

        for test_script in test_scripts:
            args = [
                self.__get_ctest_executable(),
                '-j8',
                '--verbose',
                '--output-on-failure',
                '-DCMAKE_PREFIX_PATH="{0}"'.format(cmake_search_paths),
                '-DCMAKE_MODULE_PATH="{0}"'.format(cmake_search_paths),
                # location to write reports
                '-DREPORT_DIRECTORY:FILEPATH="{0}"'.format(
                    self.report_directory.replace(os.path.sep, '/')
                ),

            ] + self.__cmake_args_bricks_python() + [
                '-S', test_script,
                '--output-log',
                output_log_fname,
            ]

            old_env = os.environ.copy()
            try:
                # ctc posttest.cmake run needs following extra variables, so these are provided
                # here as environment variables as giving them in ctest -D doesn't help,
                # as they are not visible in pre- or post-test.cmake calls
                os.environ['CTC_ENTITY_PATHS'] = "{0}".format(";".join(ctc_entity_paths))
                os.environ['PROJECT_ROOT'] = "{0}".format(self.source_folder)
                if not os.environ.get('__BRICKS_PYTHON_EXECUTABLE'):
                    os.environ['__BRICKS_PYTHON_EXECUTABLE'] = "{0}".format(sys.executable)
                self.run(' '.join(args))
            finally:
                os.environ.clear()
                os.environ.update(old_env)
        self.log_event(False, 2, 'bricks_conanfile', 'test')

    @property
    def cmake_build_folder(self):
        """ Return path to the (shortened) build folder
        """
        # by default use normal linked build folder
        # (except on Windows)
        return self.shortened_path_if_windows(
            self.cmake_build_folder_link
        )

    @property
    def cmake_build_folder_link(self):
        """ CMake link which is used for the build folder
        """

        return pathlib.Path(self.build_folder).resolve().joinpath('cip_build')

    @property
    def cmake_cache_files(self):
        """ Configure is time-consuming
        process so we want to do it
        only once. For this purpose
        we want to write "OK" after
        configure is done and remove
        the checkpoint file when needed

        TODO: Why does this return a list
        """
        return [
            self.cmake_build_folder.joinpath('CMakeCache.txt').as_posix(),
        ]

    def remove_cmake_cache(self, reason):
        """remove cmake cache
        which is needed sometimes
        to force rebuilding
        """
        # remove cache file so
        # find_package works properly
        for cmake_cache in self.cmake_cache_files:
            if os.path.exists(cmake_cache):
                os.remove(cmake_cache)
            # remove also CMakeFiles from root
            # to make sure that even most silliest
            # generators trigger the rebuild
            shutil.rmtree(
                str(self.cmake_build_folder.joinpath('CMakeFiles')),
                # ignore errors, since the cmake files
                # might not even be there to begin with,
                # and that's all right
                ignore_errors=True
            )
        # issue warning for user to so
        # he/she/they can see why cache was cleaned
        self.output.warn(
            'Removed cache since {}'.format(reason)
        )

    def __cmake_args_bricks_python(self):
        """add python interpreter to
        list of arguments
        """
        return [
            # we define also our own python version
            # which is not necessarily same as the version
            # user wants to use
            '-D__BRICKS_PYTHON_EXECUTABLE:FILEPATH={}'.format(
                sys.executable,
            ),
        ]

    def __cmake_args_with_launcher(self, cmake_args):
        """
        check if cmake_launcher.py exists
        in the build folder. If launcher exist
        then add call to launcher; otherwise
        return original arguments
        :param cmake_args:  original cmake arguments
        :returns:           cmake arguments which can should be used with
                            or without launcher
        """
        # define location of cmake_launcher script
        # which is used for instrumentation
        launcher_script = self.cmake_build_folder.joinpath(
            'cmake_launcher.py'
        ).as_posix()
        if os.path.exists(launcher_script):
            cmake_launcher_args = [
                sys.executable,
                '-u',
                launcher_script,
            ]
            return cmake_launcher_args + cmake_args
        return cmake_args

    @staticmethod
    def __get_number_of_jobs():
        """
        get number of parallel jobs
        which Bricks should use
        """
        # define default number of CPU cores
        # to use in build
        default_number_of_jobs = str(os.cpu_count())

        # get environment variable for the
        # number of jobs
        number_of_jobs = os.environ.get(
            'CMAKE_BUILD_PARALLEL_LEVEL'
        )
        if not number_of_jobs:
            number_of_jobs = default_number_of_jobs
        return number_of_jobs

    def __execution_method_build(self):
        '''build project (configure is called before)'''

        # first get all paths which are then
        # used to play around
        # essentially this means we take paths provided by conan
        # and binary overrides and plug them in to system

        initial_cache_fname = os.path.join(
            self.build_folder,
            'initial_cache.cmake'
        )

        # get system package directories (which are plugged to
        # system)

        graph_dir = os.path.join(
            self.build_folder,
            'graphviz',
            self.cip_package_name + '.dot'
        )

        # keyword arguments for
        # setting things like generator
        cmake_kwargs = {}

        # arguments which are passed
        # for cmake configure
        cmake_args = [
            '--log-level=VERBOSE',
            '--log-context',
            '-DCIP_BUILD_SYSTEM_VERSION={0}'.format(
                self.CIP_BUILD_SYSTEM_VERSION
            ),
            '-DBRICKS_VARIANT={0}'.format(
                self.variant_to_build
            ),
            '-DCMAKE_LIBRARY_PATH_TERMINATOR=""',
        ]
        cmake_args += self.__cmake_args_bricks_python()
        # get arguments for initial cache
        cmake_args += self.__cmake_args_initial_cache(
            initial_cache_fname
        )
        # get arguments for cmake core-binding
        # so decorators and pre/post-project routines
        # are called as they should
        cmake_args += self.__get_bricks_core_args()

        cmake_args += self.__cmake_args_bricks_platform_name()

        # calculate path locations and insert them
        # to file which is then used by Bricks
        cmake_args += self.__cmake_args_tagged_paths()
        # add custom platform file
        cmake_args += self.__cmake_args_platform_toolchain()
        # add ninja generator integration if needed
        self.__add_ninja_generator_arguments(cmake_kwargs, cmake_args)
        self.__add_build_type_arguments(cmake_args)
        self.__add_additional_arguments(cmake_args)

        # define CMake object for
        # system itself
        cmake = CMake(
            self,
            cmake_program=self.__get_cmake_executable(),
            **cmake_kwargs,
        )
        cmake.verbose = False
        is_makefile = cmake.generator and 'Makefiles' in cmake.generator
        if is_makefile:
            # add make generator integration
            self.__add_make_generator_arguments(cmake_kwargs, cmake_args)
        # if we have all files
        # available then we do
        # not need to call configure
        # explicitly
        must_configure = False
        for cache_file in self.cmake_cache_files:
            if not os.path.exists(cache_file):
                must_configure = True
        # cmake option --fresh should
        # run the configure again
        if '--fresh' in cmake_args:
            must_configure = True
        self.output.warn(
            'SHOULD_CONFIGURE = {0}, MUST_CONFIGURE = {1}, SHOULD_BUILD = {2}'
            .format(
                    self.should_configure,
                    must_configure,
                    self.should_build
            )
        )

        if not self.cmake_build_folder.exists():
            self.cmake_build_folder.mkdir(parents=True)

        # if configuration has not been changed then
        # we do not have to reconfigure the project
        if self.should_configure:

            self.log_event(True, 3, 'bricks_conanfile', 'configure')
            if must_configure:

                # if we are using Windows then
                # make a directory junction
                # to shorten paths
                if os.name == 'nt':
                    # make a directory link for
                    # easier debugging
                    self.__create_directory_junction(
                        source_dir=self.cmake_build_folder,
                        target_dir=self.cmake_build_folder_link,
                    )

                    # customers get confused by the 'h' folder
                    # so give some README.txt to explain it's usage
                    # this is for the file in the "h" folder
                    #
                    # this is directly in the build folder
                    # since it's linked you will see the file twice
                    self.__create_h_folder_readme(
                        self.cmake_build_folder,
                        self.cmake_build_folder,
                        self.cmake_build_folder_link,
                    )

                # activate cmake-file-api
                cmake_file_api_file_name = self.cmake_build_folder.joinpath(
                    ".cmake/api/v1/query/codemodel-v2")
                if not cmake_file_api_file_name.parent.exists():
                    cmake_file_api_file_name.parent.mkdir(parents=True)

                with cmake_file_api_file_name.open("w"):
                    # empty file so nothing to do
                    pass

                # skip configure if not needed
                # this saves quite big amount of time
                # for verify builds
                try:
                    cmake.configure(
                        args=cmake_args,
                        source_folder=self.source_folder,
                        build_folder=self.cmake_build_folder.as_posix(),
                    )
                except:
                    # zap cache if configuration
                    # is not succesfull
                    self.remove_cmake_cache(
                        'configure failed'
                    )
                    raise
            self.log_event(False, 3, 'bricks_conanfile', 'configure')

        if self.should_build:

            self.log_event(True, 4, 'bricks_conanfile', 'build')
            is_visual_studio = cmake.generator and 'Visual Studio' in cmake.generator

            # basic arguments to run CMake
            cmake_common_build_args = [
                self.__get_cmake_executable(),
                # define build folder we want to use
                '--build', self.cmake_build_folder.as_posix(),
                # define how many cores we want to use
                # for build. Used term jobs instead of
                # parallel to match better with Ninja
                # and make tools
                '-j', self.__get_number_of_jobs(),
            ]
            # Fix bug SVPT-15805
            # if build type is defined (like most of the time it is)
            # pass also build config option to CMake
            cmake_build_type = self.__get_build_type()
            if cmake_build_type:
                cmake_common_build_args += [
                    '--config',
                    str(cmake_build_type),
                ]

            # generator specific parts
            generator_specific_args = []

            # If we build for Visual Studio targets apply workaround from
            # SD-12441.
            if is_visual_studio:
                generator_specific_args = ['--', '/nodeReuse:false']

            # Pass the --output-sync option for make generator
            # Ensures that the complete output from each recipe is printed in one uninterrupted sequence
            # fix for PMTJSD-215676
            if is_makefile:
                generator_specific_args = ['--', '--output-sync']
            
            for group in self.cip_groups_to_build:
                # take build arguments, target specific arguments
                # and generator arguments and join them together
                cmake_build_args = cmake_common_build_args[:]

                # fix SD-36379: group might also be empty
                # so handle that case as well
                if group:
                    cmake_build_args += ['--target', group]
                cmake_build_args += generator_specific_args
                # decorate with launcher if needed
                cmake_build_args = self.__cmake_args_with_launcher(
                    cmake_build_args)
                self.run(
                    " ".join(cmake_build_args)
                )
            self.log_event(False, 4, 'bricks_conanfile', 'build')

    def read_selected_entity_json(self):
        try:
            entity_file_path = os.path.join(
                os.path.dirname(__file__),
                "selected_entity.json"
            )
            with open(entity_file_path, "r") as stream:
                entity = json.load(stream)
        except FileNotFoundError:
            entity_file_path = os.path.join(
                self.build_folder,
                "selected_entity.json"
            )
            with open(entity_file_path, "r") as stream:
                entity = json.load(stream)

        return entity

    def __generate_fingerprint(self):
        """
        This private method generates a fingerprint.json file for inclusion into
        the resulting package of a build.

        This file is not actually a fingerprint but instead a collection of meta
        information about the package which is used e.g. by reporting.

        The format of the fingerprint json consists of...
        ... when executed in the CI environment(Jenkins):

            - the Jenkins URL(jenkins_url)
            - the git branch name(branch_name)
            - the git tag name, or null if no tag is present(tag_name)
            - the Jenkins job name(job_name)
            - the Jenkins build tag(build_tag)
            - the Jenkins build number(build_number)
            - the git hash(git_hash)
            - the bricks build system project name as defined in the build.yml
            - the bricks version used to build(bricks_version)
            - the version of the project, if the build was a release build else
            null(version)
            - the package string, an identifier containing the project name,
            the variant and the platform(package_string)
            - a timestamp in the iso format and in UTC time
            (iso_timestamp_utc)
            - the variant built(variant)
            - the platform built(platform)
            - a list of fingerprints of the dependencies(dependencies) which
            might span up a tree of fingerprints of the dependencies

        ... when executed locally:

            - version is null
            - all items mentioning Jnkins above are missing
            - special attribute "local_build" is added with the content of
            "<user>@<hostname>" if this information is available at this point.

        : returns: path to file for installation through conan
        """
        # generate own fingerprint information (environment, etc)
        # package-root/.bricks/fingerprint.json
        fingerprint = dict()

        # we have different information for local builds vs Jenkins builds
        local_build = False
        if (
            os.environ.get("JENKINS_URL", None) is None or
            os.environ.get("JOB_NAME", None) is None or
            os.environ.get("BUILD_TAG", None) is None or
            os.environ.get("BUILD_NUMBER", None) is None
        ):
            local_build = True

        if not local_build:
            environment_var_list = [
                "JENKINS_URL",
                "BRANCH_NAME",
                "TAG_NAME",
                "JOB_NAME",
                "BUILD_TAG",
                "BUILD_NUMBER"
            ]
            for var in environment_var_list:
                fingerprint.update({
                    var.lower(): os.environ.get(var, None)
                })
        else:
            fingerprint.update({
                "local_build": "@".join([
                    os.environ.get(
                        "USERNAME",  # Windows
                        os.environ.get(
                            "USER",  # Linux
                            "unknown"  # cannot determine
                        )
                    ),
                    os.environ.get(
                        "COMPUTERNAME",  # Windows
                        os.environ.get(
                            "HOSTNAME",  # Linux
                            "unknown"  # cannot determine
                        )
                    )
                ])
            })

        # open the selected_entity.json file.
        # it can be at two places:
        # - build folder for Bricks builds
        # - next to conanfile.py for conan rebuilds
        # that file contains a lot of infos that we like
        # for the fingerprint.json
        entity = self.read_selected_entity_json()

        # the git hash
        fingerprint.update({
            "git_hash": entity.get("scm").get("revision")
        })
        # project name and version
        fingerprint.update({
            "project_name": entity.get(
                "package").get(
                    "name")
        })
        fingerprint.update({
            "version": entity.get(
                "release"
            ).get("version")
            if (
                entity.get("release").get("upload") and
                not entity.get("release").get("prerelease")
            ) else None
        })
        # bricks version used
        fingerprint.update({
            "bricks_version": entity.get("execution").get(
                "bricks_version")
        })
        fingerprint.update({
            "package_string": entity.get(
                "__build").get(
                    "directories").get(
                        "package")
        })
        fingerprint.update({
            "iso_timestamp_utc": datetime.utcnow().isoformat()
        })
        fingerprint.update({
            "variant": entity.get("variant").get("name"),
            "platform": entity.get("build_platform").get("name")
        })
        # dependencies
        dependencies = list()
        fingerprint.update({
            "dependencies": dependencies
        })
        # collect all dependency packages' fingerprints and append them
        paths = self.__get_package_lib_paths()
        for key, dep_path in paths.items():
            fp_file = os.path.join(
                dep_path, ".bricks", "fingerprint.json"
            )
            if os.path.exists(fp_file):
                with open(fp_file, "r") as stream:
                    dep_data = json.load(stream)
                dependencies.append(dep_data)
        # write the fingerprint to disk
        fp_path = os.path.join(
            self.package_folder, ".bricks"
        )
        if not os.path.exists(fp_path):
            os.makedirs(fp_path)
        with open(os.path.join(
            self.package_folder, ".bricks", "fingerprint.json"
        ), "w") as stream:
            json.dump(fingerprint, stream)
        # return installation path for __make_package_install
        return os.path.join(
            self.package_folder, ".bricks", "fingerprint.json"
        )

    def __make_package_install(self, used_package_dir):
        """ Call cmake to install into the package folder. """

        used_package_dir = pathlib.Path(used_package_dir)

        # if we are running build with operating system which
        # has file length limit in ad 2022 then we will
        # shorten the path which is used for installaton
        if os.name == 'nt':
            # calculate shortened path
            shortened_package_dir = self.shortened_path_if_windows(used_package_dir)
            # create directory junktion but keep the
            # package dir intact and use directory
            # junktion for installation

            self.__create_directory_junction(
                used_package_dir,
                shortened_package_dir
            )
            # use the shortened installation path
            used_package_dir = shortened_package_dir

        collected_manifest_files = []

        # new installation mode has
        # dedicated parameters for installatin
        # prefix etc.
        # Please see
        # https://cmake.org/cmake/help/latest/manual/cmake.1.html#install-a-project

        common_install_args = [
            self.__get_cmake_executable(),
            # where is the cmake_install.cmake located?
            '--install', self.cmake_build_folder.as_posix(),
            # where do we want to plug this data?
            '--prefix', used_package_dir.as_posix(),
        ]
        # we have to make check for build_type
        # since there are few cases where settings
        # does not have build_type setting
        # (examples of such cases are tools and
        # system packages)
        cmake_build_type = self.__get_build_type()
        if cmake_build_type:
            common_install_args += [
                '--config',
                str(cmake_build_type),
            ]

        # if install groups are defined then use them
        for group in self.cip_groups_to_install:
            self.output.info(
                'Build system is installing group {0}'.format(
                    group
                )
            )
            args = common_install_args[:]

            # fix SD-36379: group might also be empty
            # so handle that case as well
            if group:
                args += ['--component', group ]
            self.run(' '.join(args))

    def package(self):
        '''we install files using the install functionality of CMake
        so this is not needed
        '''

        self.log_event(True, 5, 'bricks_conanfile', 'package')

        if self.should_install and self.cip_execution_method == 'build':
            self.__make_package_install(
                self.package_folder
            )

        # write fingerprint to self.package_folder + '.bricks/fingerprint.json'
        # fingerprint will be registered in 'files_to_compress' later
        self.__generate_fingerprint()

        zip_fname = self.compressed_package_file
        if zip_fname:

            # remove old archive
            if os.path.exists(zip_fname):
                os.remove(zip_fname)

            # collect root level items which we want to compress
            with tools.chdir(self.package_folder):
                files_to_compress = sorted(
                    [
                        item
                        for item in os.listdir(os.getcwd())
                    ]
                )

                relative_archive = os.path.relpath(
                    zip_fname,
                    self.package_folder
                )

                relative_archive = relative_archive.replace(
                    os.path.sep,
                    '/'
                )

                # use cmake to define the the archive
                cmake_archive_args = [
                    self.__get_cmake_executable(),
                    '-E',
                    'tar',
                    'cvf',
                    relative_archive,
                    '--format=zip',
                ] + files_to_compress

                res = subprocess.run(
                    cmake_archive_args,
                    check=True,
                    stdout=None,
                    stderr=subprocess.STDOUT,
                    universal_newlines=True
                )
            with tools.chdir(self.build_folder):

                # use cmake to define the the archive
                cmake_archive_args = [
                    self.__get_cmake_executable(),
                    '-E',
                    'tar',
                    '-tvf',
                    zip_fname,
                ]
                self.run(' '.join(cmake_archive_args))
        self.log_event(False, 5, 'bricks_conanfile', 'package')

    def package_id(self):
        '''calculate proper hash (or identifier) for the package

        This method is used to determinate the binary interface
        compability with the packages
        '''
        if self.variant_target_any_platform:
            # mark that we are truly caring only about
            # headers and nothing else
            self.info.header_only()
        elif self.cip_is_tool:
            # mark that we want to include build settings
            # for the tools
            self.info.include_build_settings()
        else:
            # for normal builds we are only interested about target
            # types so one can cross-compile with windows and linux
            # and mix the results (sure, not adviced but possible)
            self.info.discard_build_settings()

    def package_info(self):
        '''tell what is going on with this package'''
        pass

#######################################################################
#
# This is generated script. Please not not edit!
#
# This CMake script is called during installation of
# Script will parse exported projects and handle implicit dependencies
# between groups and packages. Vital for the proper package oriented
# structuring
#
# Triggered by D:/.bbs_conan/1e3a16/1/cmake/post/handle_implicit_dependencies.cmake
#
#######################################################################

set(CMAKE_MESSAGE_CONTEXT_SHOW TRUE)
list(APPEND CMAKE_MESSAGE_CONTEXT install_script)

# internal checks for script sanity. These files
# are removed so they must not be carelessly defined
string(TIMESTAMP start_time "%s")

if ("xD:/Next-python_API/next/conan_workarea/h/1c8ed01dedea1a0fcedaba3a730f4481/.bricks/d/intermediate.jsonx" STREQUAL "xx")
    message(FATAL_ERROR "Internal error INTERMEDIATE_JSON failing")
endif()

if ("xD:/Next-python_API/next/conan_workarea/h/1c8ed01dedea1a0fcedaba3a730f4481/.bricks/d/aggregated.jsonx" STREQUAL "xx")
    message(FATAL_ERROR "Internal error AGGREGATED_JSON failing")
endif()

if ("xD:/Next-python_API/next/conan_workarea/h/1c8ed01dedea1a0fcedaba3a730f4481/.bricks/d/code_install.cmakex" STREQUAL "xx")
    message(FATAL_ERROR "Internal error GENERATED_INSTALL_SCRIPT failing")
endif()

#########################################################
#
# CREATE INTERMEDIATE.JSON
#
# Configure package_importer project which calls
# find_package for both used and create packages. Project
# uses mocked version of target-related functions. The
# find_package includes created config files which
# in turn exported-targets.cmake. The exported-targets.cmake
# is then calling the mocked functions which update the
# intermediate.json to every call
#
#########################################################

# FIX: add also parameter for generator toolset
# SD-48666
set(__generator_toolset_argument)
if (NOT ("xx" STREQUAL "xx"))
    set(__generator_toolset_argument "-T" "")
endif()

message(STATUS "CMAKE_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "CMAKE_CURRENT_LIST_DIR=${CMAKE_CURRENT_LIST_DIR}")
message(STATUS "CMAKE_FIND_PACKAGE_PREFER_CONFIG=")

# create project which imports all dependencing
# packages
execute_process(
    COMMAND
        # some the platforms are not
        # playing nicely so take out the
        # hammer and set compiler with environment
        # variables
        # Related: SD-42636
        ${CMAKE_COMMAND}
        -E env
            "AS="
            "CC=c:/LegacyApp/Microsoft Visual Studio/VC/Tools/MSVC/14.16.27023/bin/Hostx86/x64/cl.exe"
            "CXX=c:/LegacyApp/Microsoft Visual Studio/VC/Tools/MSVC/14.16.27023/bin/Hostx86/x64/cl.exe"

        ${CMAKE_COMMAND}
        "-DCMAKE_BUILD_TYPE=$<CONFIG>"
	# if there are problems with find specific packages, please activate
	# the debug-find-pkg with the list of the relevant packages,
	# the sample is for ecal and protobuf
	# sample: "--debug-find-pkg=protobuf,Protobuf,ecal,eCAL"
        # use languages which are enabled in top-level project
        # also in the dependency calculation project
        "-D__BRICKS_ENABLED_LANGUAGES=C;CXX;RC"
        # define Bricks version since some custom_commands from
        # customer rely on Bricks version to exist
        "-DCIP_BUILD_SYSTEM_VERSION:STRING=4.25.0"
        "-DPARENT_BINARY_DIR:PATH=D:/Next-python_API/next/conan_workarea/h/1c8ed01dedea1a0fcedaba3a730f4481"
        "-DPARENT_CMAKE_PREFIX_PATH:PATH=D:/.bbs_conan/b9d1f2/1;D:/.bbs_conan/ef297c/1;D:/.bbs_conan/a2cb08/1;D:/.bbs_conan/1e35b5/1;D:/.bbs_conan/861632/1;D:/.bbs_conan/8f563d/1;D:/.bbs_conan/575ef6/1;D:/.bbs_conan/f99f7b/1;D:/.bbs_conan/ed40e4/1;D:/.bbs_conan/c926df/1;D:/.bbs_conan/05150c/1;D:/.bbs_conan/0d3178/1;D:/.bbs_conan/d8cf06/1;D:/.bbs_conan/821b1a/1;D:/.bbs_conan/906693/1;D:/.bbs_conan/bd8589/1;D:/.bbs_conan/d57d72/1;D:/.bbs_conan/f65a98/1;D:/.bbs_conan/962cbc/1"
        "-DPARENT_CMAKE_MODULE_PATH:PATH=D:/.bbs_conan/b9d1f2/1;D:/.bbs_conan/ef297c/1;D:/.bbs_conan/a2cb08/1;D:/.bbs_conan/1e35b5/1;D:/.bbs_conan/861632/1;D:/.bbs_conan/8f563d/1;D:/.bbs_conan/575ef6/1;D:/.bbs_conan/f99f7b/1;D:/.bbs_conan/ed40e4/1;D:/.bbs_conan/c926df/1;D:/.bbs_conan/05150c/1;D:/.bbs_conan/0d3178/1;D:/.bbs_conan/d8cf06/1;D:/.bbs_conan/821b1a/1;D:/.bbs_conan/906693/1;D:/.bbs_conan/bd8589/1;D:/.bbs_conan/d57d72/1;D:/.bbs_conan/f65a98/1;D:/.bbs_conan/962cbc/1"
	# If enabled, this ensures, that cmake will first search for <Package>-config.cmake
	# and afterwards for Find<Packe> modules.
	# This ensures, that the package configs provided in artefactory will be seen
	# before a generic Find<Package> from cmake.
	# This is essential only for common open source packages, like protobuf.
	"-DCMAKE_FIND_PACKAGE_PREFER_CONFIG="
        "-DPARENT_INSTALL_PREFIX:PATH=${CMAKE_INSTALL_PREFIX}"
        "-DINTERMEDIATE_OUTPUT_FILE:FILEPATH=D:/Next-python_API/next/conan_workarea/h/1c8ed01dedea1a0fcedaba3a730f4481/.bricks/d/intermediate.json"
        "-DCMAKE_MAKE_PROGRAM:FILEPATH=c:/LegacyApp/Microsoft Visual Studio/MSBuild/15.0/Bin/MSBuild.exe"
        # tell packages which were found in parent project
        # explicitly
        "-DBRICKS_PACKAGES_FOUND=Protobuf;boost_headers;boost_date_time;boost_atomic;boost_filesystem;boost_container;boost_json;boost_program_options;boost_system;boost_chrono;boost_regex;Threads;boost_thread;boost_log;Boost;CMakeFunctions;eCAL;Flatbuffers;jsoncpp;mtsc;mtscpp;mts_opc;mts_sdc;mts_swc;mts_asap2;mts_fibex;mts_arxml;mts_ecu;mts_can;mts_dbc;mts_eth;mts_flexray;mts_gps;mts_nmea;mts_ref_cam;mts_rt_range;mts_rrec;mts_rec;mts_zrec;mts_mdf;mts_sdl_parser;mts_xtypes_parser;mts_csv_exporter;mts_exporter;mts_ecu_data_matcher;mts_ecu_formatter;mts_ecu_sd_formatter;mts_ecu_sd_provider;mts_swc_parser;mts_sdc_parser;mts_can_arxml_parser;mts_can_data_matcher;mts_can_fibex_parser;mts_can_formatter;mts_dbc_parser;mts_ethernet_arxml_parser;mts_ethernet_data_matcher;mts_ethernet_fibex_parser;mts_ethernet_formatter;mts_flexray_formatter;mts_flexray_fibex_parser;mts_flexray_data_matcher;mts_gps_formatter;mts_gps_sdl_parser;mts_gps_data_matcher;mts_ref_cam_formatter;mts_ref_cam_parser;mts_ref_cam_data_matcher;mts_rt_range_probe;mts_rt_range_data_matcher;mts_rt_range_formatter;mts_rt_range_parser;mts_velodyne_probe;mts_signal_storage;mts_udex_extractor;mts_xtypes_data_matcher;mts_rrec_reader;mts_rec_exporter;mts_rec_reader;mts_zrec_reader;mts_pcap_reader;mts_ffmpeg;mts_mfc5xx_processor;mts_svc_processor;mts_amba_processor;mts_core_lib;mts_sentinel;Python;Git;spdlog;VFR_Utilities;OpenCV"
        # tell to use same system as parent project
        "-DCMAKE_SYSTEM_NAME=Windows"

        # add compiler exclusion/inclusion flags
        # definitions for language C
        "-DCMAKE_C_COMPILER=c:/LegacyApp/Microsoft Visual Studio/VC/Tools/MSVC/14.16.27023/bin/Hostx86/x64/cl.exe"
        "-DCMAKE_C_COMPILER_WORKS:BOOL=TRUE"
        # definitions for language CXX
        "-DCMAKE_CXX_COMPILER=c:/LegacyApp/Microsoft Visual Studio/VC/Tools/MSVC/14.16.27023/bin/Hostx86/x64/cl.exe"
        "-DCMAKE_CXX_COMPILER_WORKS:BOOL=TRUE"
        # definitions for language RC
        "-DCMAKE_RC_COMPILER=C:/Program Files (x86)/Windows Kits/10/bin/10.0.17763.0/x64/rc.exe"
        "-DCMAKE_RC_COMPILER_WORKS:BOOL=TRUE"
        

        # tell to use our toolchain from
        # main project also in dependency
        # calculation project
        "-DCMAKE_TOOLCHAIN_FILE:FILEPATH=D:/.bbs_conan/8dd44f/1/__system/cip_build_system_platforms/toolchains/conan/conan_toolchain.cmake"
        -G "Visual Studio 15 2017 Win64"
        ${__generator_toolset_argument}
        -B "D:/Next-python_API/next/conan_workarea/h/1c8ed01dedea1a0fcedaba3a730f4481/.bricks/d/dbuild"
        -S "D:/.bbs_conan/1e3a16/1/cmake/post/implicit_dependencies/package_importer"
    COMMAND_ECHO STDOUT
    RESULT_VARIABLE res_code
)
if (NOT (res_code EQUAL 0))
    message(WARNING "next: Cannot create D:/Next-python_API/next/conan_workarea/h/1c8ed01dedea1a0fcedaba3a730f4481/.bricks/d/intermediate.json, potentially you should consider to set the variable CMAKE_FIND_PACKAGE_PREFER_CONFIG in your CMakeLists.txt to prefer packages from artefactory before any default installed packages. Alternative you can define own pre_import.cmake which is included before package is imported (for details please see user documentation).")
endif()

#########################################################
#
# CREATE AGGREGATED.JSON
#
# Call aggregate.py with intermediate.json to create
# aggregated.json which is easier for code generator
#
#########################################################

# calculate aggregated.json by
# combining the individual property
# settings to one, more coherent
# data
execute_process(
    COMMAND
        C:/Users/uig44320/cip_config_dir/bricks/4.25.0/windows/Scripts/python.exe
        -u
        "D:/.bbs_conan/1e3a16/1/cmake/post/implicit_dependencies/aggregate.py"
        "--intermediate-json=D:/Next-python_API/next/conan_workarea/h/1c8ed01dedea1a0fcedaba3a730f4481/.bricks/d/intermediate.json"
        "--aggregated-json=D:/Next-python_API/next/conan_workarea/h/1c8ed01dedea1a0fcedaba3a730f4481/.bricks/d/aggregated.json"
    COMMAND_ECHO STDOUT
    RESULT_VARIABLE res_code
)
if (NOT (res_code EQUAL 0))
    message(FATAL_ERROR "next: Cannot create D:/Next-python_API/next/conan_workarea/h/1c8ed01dedea1a0fcedaba3a730f4481/.bricks/d/aggregated.json")
endif()

#########################################################
#
# GENERATE CMAKE CODE FOR TRANSITIVE DEPENDENCIES
#
# Call aggregate.py with intermediate.json to create
# aggregated.json which is easier for code generator
#
#########################################################

# use aggregated.json to calculate implicit
# dependencies between packages
execute_process(
    COMMAND
        C:/Users/uig44320/cip_config_dir/bricks/4.25.0/windows/Scripts/python.exe
        -u
        "D:/.bbs_conan/1e3a16/1/cmake/post/implicit_dependencies/generate.py"
        "--aggregated-json=D:/Next-python_API/next/conan_workarea/h/1c8ed01dedea1a0fcedaba3a730f4481/.bricks/d/aggregated.json"
        "--workdir=D:/Next-python_API/next/conan_workarea/h/1c8ed01dedea1a0fcedaba3a730f4481/.bricks/d/workdir"
        "--install-script=D:/Next-python_API/next/conan_workarea/h/1c8ed01dedea1a0fcedaba3a730f4481/.bricks/d/code_install.cmake"
    COMMAND_ECHO STDOUT
    RESULT_VARIABLE res_code
)
if (NOT (res_code EQUAL 0))
    message(FATAL_ERROR "next: Cannot create D:/Next-python_API/next/conan_workarea/h/1c8ed01dedea1a0fcedaba3a730f4481/.bricks/d/code_install.cmake")
endif()

string(TIMESTAMP end_time "%s")
math(EXPR elapsed "${end_time}-${start_time}")
message(STATUS "Calculation took ${elapsed}")

#########################################################
#
# INSTALL GENERATED FILES
#
# Code generation also updates list of files which
# should be installed for transient dependencies. It is
# enough to simply include the file and install commands
# inside are triggered
#
#########################################################
include("D:/Next-python_API/next/conan_workarea/h/1c8ed01dedea1a0fcedaba3a730f4481/.bricks/d/code_install.cmake")

# install also generated JSON
file(
    INSTALL "D:/Next-python_API/next/conan_workarea/h/1c8ed01dedea1a0fcedaba3a730f4481/.bricks/d/aggregated.json"
    # we are inside installation script so we do
    # have to define the install prefix
    # normally (in the CMakeLists.txt) one should not
    # use absolute paths
    DESTINATION ${CMAKE_INSTALL_PREFIX}/.bricks
)

list(POP_BACK CMAKE_MESSAGE_CONTEXT)

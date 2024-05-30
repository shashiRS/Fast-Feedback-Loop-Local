cmake_minimum_required(VERSION 3.21)
# 3.21 is required for TARGET_RUNTIME_DLLS
# see https://cmake.org/cmake/help/latest/manual/cmake-generator-expressions.7.html#genex:TARGET_RUNTIME_DLLS

########################### set OS platform architecture
if (CMAKE_HOST_WIN32)
  set(ARCH win64)
endif ()

# remove protobuf
#if (UNIX AND "$ENV{CI}" STREQUAL "true")
#  execute_process(COMMAND cat /etc/debian_version
#    OUTPUT_VARIABLE deb_version
#    OUTPUT_STRIP_TRAILING_WHITESPACE
#  )
#  if ("${deb_version}" STREQUAL "buster/sid")
#    # Ubuntu 18
#    execute_process(COMMAND apt remove -y libprotoc-dev protobuf-c-compiler protobuf-compiler libprotoc10 libprotobuf-lite10 libprotobuf10 libprotobuf-dev
#      COMMAND_ERROR_IS_FATAL ANY
#    )
#  elseif ("${deb_version}" STREQUAL "bullseye/sid")
#    # Ubuntu 20
#    execute_process(COMMAND apt remove -y libprotoc-dev protobuf-c-compiler protobuf-compiler libprotoc17 libprotobuf-lite17 libprotobuf17 libprotobuf-dev
#      COMMAND_ERROR_IS_FATAL ANY
#    )
#  else ()
#    message(FATAL_ERROR "unsupported OS '${deb_version}'")
#  endif ()
#  unset(deb_version)
#endif ()

########################### Default to C++17
if (NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED True)
endif ()

### Set variable names
include(src/cmake/cmake_variables.cmake)
### Function to build the executables
include(src/cmake/build_function_for_executables.cmake)
### Function to build the library
include(src/cmake/build_function_for_library.cmake)

########################### Setup project
if (DEFINED MANUAL_PROJECT_VERSION)
  project(next VERSION ${MANUAL_PROJECT_VERSION})
else ()
  project(next)
endif ()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

########################### Compiler flags
option(DELIVERY_RELEASE "Defines if the build should be delivered to customers" FALSE)

add_compile_options(
  "$<$<CXX_COMPILER_ID:GNU>:$<BUILD_INTERFACE:-Wall;-Wextra;-Wconversion;-Wsign-conversion;-Werror>>"
  "$<$<CXX_COMPILER_ID:MSVC>:$<BUILD_INTERFACE:-DNOMINMAX;/EHs;/W4;/WX;/Zi;/Zc:__cplusplus>>"
)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")
# if the partly optimized debug version shall be built, some flags must be adjusted
if(OPTIMIZEDDEBUG)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # remove the RTC compile option, it is incompatible to optimization
    string(REGEX REPLACE "[-|/]RTC(su|[1su])" "" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    # remove the flag of not optimizing, replace it with O2 (maximum optimization, favor speed)
    string(REGEX REPLACE "[-|/]Od" "/O2" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
    # enable inlining any suitable function
    string(REGEX REPLACE "[-|/]Ob0" "/Ob2" CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG}")
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # full optimization on Linux
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O3")
  endif()
endif()

if (DELIVERY_RELEASE)
  add_compile_definitions(DELIVERY_RELEASE)
endif()

########################### Linker flags
# has to be done within a CMake project
if (MSVC)
  add_link_options("/DEBUG:FULL")
  list(REMOVE_ITEM LINK_OPTIONS "/DEBUG")
endif ()

########################### Conan Include
if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  conan_basic_setup()
  set(LOCAL_BUILD_CONAN true)
else ()
  message("Running CMake without conanbuildinfo.cmake.")
endif ()

########################### find thirdparty libraries
if (NOT BUILDDOCUMENTATION)
  if (MSVC)
    # CMake >= 3.15 will erroneously define PROTOBUF_USE_DLLS otherwise
    set(Protobuf_USE_STATIC_LIBS TRUE)
  endif ()
  # find Protobuf before eCAL is searched
  find_package(Protobuf REQUIRED)

  set(Boost_NO_SYSTEM_PATHS ON)
  set(Boost_USE_MULTITHREADED ON)
  set(Boost_USE_STATIC_LIBS ON)
  set(Boost_USE_STATIC_RUNTIME OFF)
  set(BOOST_ALL_DYN_LINK OFF)
  # date_time is required by filesystem
  # json is required by UDex
  find_package(Boost REQUIRED COMPONENTS date_time filesystem json program_options system log)
  find_package(eCAL REQUIRED)
  find_package(Flatbuffers 2.0.6 REQUIRED)
  find_package(jsoncpp REQUIRED)
  find_package(mts_core_lib CONFIG REQUIRED COMPONENTS libraries modules)
  find_package(mts_sentinel CONFIG REQUIRED COMPONENTS client sentinel)
  #  find_package(Tracy REQUIRED)

  # Python is required for the check of the include paths
  find_package(Python COMPONENTS Interpreter REQUIRED)
endif ()

########################### set the RPATH
# Setting where to look for .so files on Linux (Windows is ignoring this settings).
# By default only LD_LIBRARY_PATH env variable is considered. We are adding here to also look
# into the directory of the executable, named by $ORIGIN.
# This we do via setting RPATH, which will also overwrite the setting of the children of the dependencies.
# By default RUNPATH ("new dtag") would be set, which requires on children also have RUNPATH set.
# For the build targets we do not set it, but as soon as the targets are installed.
if (UNIX)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--disable-new-dtags")
endif ()
set(CMAKE_SKIP_BUILD_RPATH TRUE)
if (NOT BUILD_TESTING)
  # when not building the tests, set the RPATH to $ORIGIN
  set(CMAKE_INSTALL_RPATH "\$ORIGIN;\$ORIGIN/../lib")
endif ()

########################### helper macros for checking the include directories
macro(getAllTargets directory target_list)
  get_property(sub_directories DIRECTORY ${directory} PROPERTY SUBDIRECTORIES)
  foreach (sub_dir ${sub_directories})
    getAllTargets(${sub_dir} ${target_list})
  endforeach ()
  get_property(current_targets DIRECTORY ${directory} PROPERTY BUILDSYSTEM_TARGETS)
  list(APPEND ${target_list} ${current_targets})
endmacro()

macro(checkIncludeDirs target_list)
  foreach (tgt ${target_list})
    get_target_property(type ${tgt} TYPE)
    if (${type} STREQUAL "UTILITY")
      # skip targets which are in fact the components (e.g. NEXT_COMPONENT)
      continue()
    endif ()
    get_target_property(include_dirs ${tgt} INCLUDE_DIRECTORIES)
    if ("${include_dirs}" STREQUAL "include_dirs-NOTFOUND")
      # skip if no include dirs found
      continue()
    endif ()
    get_target_property(tgt_source_dir ${tgt} SOURCE_DIR)
    string(REPLACE "${BASE_SRC_DIR}/" "" swc ${tgt_source_dir})
    string(FIND ${swc} "/" swc_slash_pos)
    if (${swc_slash_pos} GREATER 0)
      string(SUBSTRING ${swc} 0 ${swc_slash_pos} swc)
    endif ()
    get_target_property(tgt_binary_dir ${tgt} BINARY_DIR)
    foreach (include_dir ${include_dirs})
      string(FIND ${include_dir} ${tgt_binary_dir} binary_dir_pos)
      if (NOT ${binary_dir_pos} EQUAL -1)
        # allow the binary dir (conan_workarea/some/path...)
        continue()
      endif ()
      string(FIND ${include_dir} "$<INSTALL_INTERFACE" include_dir_pos)
      if (NOT ${include_dir_pos} EQUAL -1)
        # allow install interfaces
        continue()
      endif ()
      if (${LOCAL_BUILD_CONAN})
        string(FIND ${include_dir} "$<CONFIG" include_dir_pos)
        if (NOT ${include_dir_pos} EQUAL -1)
          # allow install interfaces
          continue()
        endif ()
      endif ()
      string(REPLACE "$<BUILD_INTERFACE:" "" inc ${include_dir})
      string(REGEX REPLACE ">$" "" inc ${inc})
      string(FIND ${inc} "${BASE_SRC_DIR}/${swc}" swc_path_pos)
      if (NOT ${swc_path_pos} EQUAL -1)
        # allow include paths within the swc
        continue()
      endif ()
      string(FIND ${inc} "D:/.bbs_conan" bbs_dir_pos)
      if (${bbs_dir_pos} EQUAL 0)
        # allow the bricks package folder
        continue()
      endif ()
      string(FIND ${inc} "cip_config_dir" config_dir_pos)
      if (NOT ${config_dir_pos} EQUAL -1)
        # allow using the cip_config_dir (cache directory of the bricks packages)
        continue()
      endif ()
      message(FATAL_ERROR "Target ${tgt} is using include dir ${inc}.")
    endforeach ()
  endforeach ()
endmacro()

########################### Add source directory and check the include paths
if (NOT BUILDDOCUMENTATION)
  add_subdirectory(src)
  add_subdirectory(examples)

  find_package(Python REQUIRED)
  getAllTargets(src local_targets)
  checkIncludeDirs(${local_targets})
  execute_process(COMMAND ${Python_EXECUTABLE} scripts/include_check.py src
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMAND_ERROR_IS_FATAL ANY
  )
endif ()

########################### Add test directories
if ((NOT ${CMAKE_CROSSCOMPILING}) AND BUILD_TESTING)
  include(CTest)

  # use the build directories of the other libraries within RPATH, so we do not have to copy everything around
  set(CMAKE_SKIP_BUILD_RPATH FALSE)

  add_subdirectory(tests)
endif ()

########################### Do the documentation build
if (BUILDDOCUMENTATION)
  include(src/cmake/documentation_code.cmake)
endif (BUILDDOCUMENTATION)

find_package(mts_core_lib CONFIG REQUIRED COMPONENTS libraries modules)
find_package(mts_sentinel CONFIG REQUIRED COMPONENTS client)

# TODO split this file up into post imports per component

include(${CMAKE_CURRENT_LIST_DIR}/build_flatbuffers.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/install_helper.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/link_helper.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/pluginhelper.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/testhelper.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/version_helper.cmake)

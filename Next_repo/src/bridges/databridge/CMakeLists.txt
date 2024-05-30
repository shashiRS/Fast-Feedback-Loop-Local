set(CMAKE_INSTALL_RPATH_BACKUP ${CMAKE_INSTALL_RPATH})
set(CMAKE_INSTALL_RPATH "$\{ORIGIN\};$\{ORIGIN\}/../lib")

#*********************************************************************************************************************
##Begin DataBridge Library
#*********************************************************************************************************************

set(DATABRIDGE_LIB databridge_lib)

set(SRC_FILES_DATABRIDGE_LIB
  DataManager/data_manager_impl.cpp
  DataManager/DataBroadcaster/data_broadcaster_impl.cpp
  DataClassManager/data_class_manager.cpp
  DataClassManager/data_request_server.cpp
  DataClassManager/event_handler_helper.cpp
  DataClassManager/json_to_plugin_config.cpp
  DataClassManager/next_ui_config_request_handler.cpp
  DataClassManager/next_ui_update_request_handler.cpp
  DataClassManager/next_ui_backend_request_handler.cpp
  PluginManager/OutputSchema/output_schema_checker.cpp
  PluginManager/OutputSchema/version.cpp
  PluginManager/plugin_manager.cpp
  PluginManager/plugin_config_handler.cpp
  PluginManager/process_manager.cpp
  DataManager/OutputManager/flatbuffers_to_json.cpp
  DataManager/OutputManager/plugin_description_to_json.cpp
  DataManager/OutputManager/simulation_environment_cache.cpp
  DataManager/OutputManager/plugin_data_class_parser.cpp
)

set(PUBLIC_INCLUDE_DIRS_DATABRIDGE_LIB
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/DataManager/DataBroadcaster>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/DataManager>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PluginManager>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/PluginManager/InputPortController>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/DataClassManager>
  $<INSTALL_INTERFACE:include>
)

set(PRIVATE_INCLUDE_DIRS_DATABRIDGE_LIB ${PLUGINS_INTERFACE_DIR})

set(PUBLIC_LINK_LIBS_DATABRIDGE_LIB
  next_udex::next_udex
  mtscpp::mtscpp # Only the Magno SDK as link dependency
  next_sdk::next_sdk
  next_sdk::next_appsupport
  next_control::next_control
  next_plugin
)

set(PRIVATE_LINK_LIBS_DATABRIDGE_LIB
  web_server
  eCAL::core
  Boost::filesystem
  Boost::date_time # required by filesystem
  flatbuffers::flatbuffers
  jsoncpp_static
  next_flatbuffers_3d_view
  next_flatbuffers_core
)

next_create_library(
  NAME ${DATABRIDGE_LIB}
  TYPE STATIC
  SRC_FILES ${SRC_FILES_DATABRIDGE_LIB}
  FOLDER_VIEW Bridges/Databridge
  PUBLIC_LINK_LIBS ${PUBLIC_LINK_LIBS_DATABRIDGE_LIB}
  PRIVATE_LINK_LIBS ${PRIVATE_LINK_LIBS_DATABRIDGE_LIB}
  PUBLIC_INCLUDE_DIRS ${PUBLIC_INCLUDE_DIRS_DATABRIDGE_LIB}
  PRIVATE_INCLUDE_DIRS ${PRIVATE_INCLUDE_DIRS_DATABRIDGE_LIB}
)

add_dependencies(
  ${DATABRIDGE_LIB}
  GENERATE_next_flatbuffers_3d_view
  GENERATE_next_flatbuffers_core
)

if(UNIX)
  target_link_libraries(${DATABRIDGE_LIB}
    PRIVATE
      dl
  )
endif()

list(APPEND LIB_LIST "eCAL::core" "eCAL::core_pb" "next_sdk::next_sdk" "next_sdk::next_appsupport")

install_extra_libs_linux(
  LIB_LIST ${LIB_LIST}
  EXTRA_LIB_LIST mts_core_lib::modules
  COMPONENT BRIDGES_COMPONENT
)

#//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
##End DataBridge Library
#//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#*********************************************************************************************************************
##Begin main
#*********************************************************************************************************************
set(TARGET_NAME next_databridge)

set(SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

set(RC_FILE ${CMAKE_CURRENT_SOURCE_DIR}/version_info/version_info.rc)
set(VERSION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/version_info/version.h.in)

set(PRIVATE_LINK_LIBS
  databridge_lib
  next_sdk::next_appsupport
)

set(PRIVATE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/interface)

next_create_executable(
  NAME ${TARGET_NAME}
  SRC_FILES ${SRC_FILES}
  USE_VERSION_INFO TRUE
  RC_FILE ${RC_FILE}
  VERSION_FILE_PATH ${VERSION_FILE}
  FOLDER_VIEW Bridges/Databridge
  PRIVATE_LINK_LIBS ${PRIVATE_LINK_LIBS}
  PRIVATE_INCLUDE_DIRS ${PRIVATE_INCLUDE_DIRS}
)

set_target_properties(${TARGET_NAME}
  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${DATABRIDGE_OUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${DATABRIDGE_OUT_DIR}"
    VS_DEBUGGER_WORKING_DIRECTORY "${DATABRIDGE_OUT_DIR}"
)

install_runtime(
  TARGET_NAME ${TARGET_NAME}
  COMPONENT BRIDGES_COMPONENT
)

install_extra_libs_windows(
  TARGET_NAME ${TARGET_NAME}
  LIB_LIST mts_core_lib::modules
  COMPONENT BRIDGES_COMPONENT
)

if(UNIX)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath=../lib")

  # Newer versions of the linker in some platforms, emit DT_RUNPATH where they used to emit DT_RPATH
  # To replicate the old behaviour on versions of the linker that emit RUNPATH instead, pass the --disable-new-dtags flag to the linker
  target_link_libraries(${TARGET_NAME} PRIVATE "-Wl,--disable-new-dtags")
endif()


add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ECAL_INI_FILE} $<TARGET_FILE_DIR:${TARGET_NAME}>
)

#install_lib(libname Tracy::TracyClient targetname ${TARGET_NAME})

#//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
##End main
#//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_RPATH_BACKUP})

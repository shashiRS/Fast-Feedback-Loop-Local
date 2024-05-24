
add_subdirectory(thirdparty)

#*********************************************************************************************************************
##Begin flatbuffers
#*********************************************************************************************************************
set(FLATBUFFERS_FLATC_FLAGS "--scoped-enums" "--gen-object-api")

flatbuffers_generate_headers(
  TARGET next_flatbuffers_core
  SCHEMAS
    ${BRIDGESDK_SCHEMA_DIR}/core/signal_update.fbs
    ${BRIDGESDK_SCHEMA_DIR}/core/signal.fbs
    ${BRIDGESDK_SCHEMA_DIR}/core/types.fbs
    ${BRIDGESDK_SCHEMA_DIR}/core/meta_info.fbs
  FLAGS ${FLATBUFFERS_FLATC_FLAGS}
  BINARY_SCHEMAS_DIR ${DATABRIDGE_SCHEMAS_OUT_DIR}/core
  INCLUDE_PREFIX next/bridgesdk/schema/core
)

flatbuffers_generate_headers(
  TARGET next_flatbuffers_3d_view
  SCHEMAS
  ${BRIDGESDK_SCHEMA_DIR}/core/meta_info.fbs
  ${BRIDGESDK_SCHEMA_DIR}/3d_view/egovehicle.fbs
  ${BRIDGESDK_SCHEMA_DIR}/3d_view/freegeometry.fbs
  ${BRIDGESDK_SCHEMA_DIR}/3d_view/groundline.fbs
  ${BRIDGESDK_SCHEMA_DIR}/3d_view/pointcloud.fbs
  ${BRIDGESDK_SCHEMA_DIR}/3d_view/primitive.fbs
  ${BRIDGESDK_SCHEMA_DIR}/3d_view/trafficparticipant.fbs
  ${BRIDGESDK_SCHEMA_DIR}/3d_view/view_update.fbs
  FLAGS ${FLATBUFFERS_FLATC_FLAGS}
  BINARY_SCHEMAS_DIR ${DATABRIDGE_SCHEMAS_OUT_DIR}/3d_view
  INCLUDE_PREFIX next/bridgesdk/schema/3d_view
)

flatbuffers_generate_headers(
  TARGET next_flatbuffers_view_update
  SCHEMAS
  ${BRIDGESDK_SCHEMA_DIR}/3d_view/view_update.fbs
  FLAGS ${FLATBUFFERS_FLATC_FLAGS}
  BINARY_SCHEMAS_DIR ${CMAKE_CURRENT_BINARY_DIR}
  INCLUDE_PREFIX next/bridgesdk/schema/3d_view
)

flatbuffers_generate_headers(
  TARGET next_flatbuffers_profiler
  SCHEMAS
  ${BRIDGESDK_SCHEMA_DIR}/profiler/basic_info.fbs
  ${BRIDGESDK_SCHEMA_DIR}/profiler/cpu_profiler.fbs
  ${BRIDGESDK_SCHEMA_DIR}/profiler/ecal_monitor.fbs
  ${BRIDGESDK_SCHEMA_DIR}/profiler/network_graph.fbs
  FLAGS ${FLATBUFFERS_FLATC_FLAGS}
  BINARY_SCHEMAS_DIR ${DATABRIDGE_SCHEMAS_OUT_DIR}/profiler
  INCLUDE_PREFIX next/bridgesdk/schema/profiler
)

# Note: following target is so far not used, so the files are not generated and not installed
flatbuffers_generate_headers(
  TARGET next_flatbuffers
  SCHEMAS
  ${BRIDGESDK_SCHEMA_DIR}/fov.fbs
  ${BRIDGESDK_SCHEMA_DIR}/path.fbs
  ${BRIDGESDK_SCHEMA_DIR}/roadsign.fbs
  ${BRIDGESDK_SCHEMA_DIR}/tunnel.fbs
  FLAGS ${FLATBUFFERS_FLATC_FLAGS}
  BINARY_SCHEMAS_DIR ${DATABRIDGE_SCHEMAS_OUT_DIR}
  INCLUDE_PREFIX next/bridgesdk/schema/
)

set(INSTALL_DIRS
  ${CMAKE_CURRENT_BINARY_DIR}/next_flatbuffers_3d_view/
  ${CMAKE_CURRENT_BINARY_DIR}/next_flatbuffers_profiler/
  ${CMAKE_CURRENT_BINARY_DIR}/next_flatbuffers_core/
# ${CMAKE_CURRENT_BINARY_DIR}/next_flatbuffers/
)

install_runtime(
  COMPONENT BRIDGES_COMPONENT
  DIRECTORY ${INSTALL_DIRS}
)

install_runtime(
  COMPONENT BRIDGES_COMPONENT
  DIRECTORY ${DATABRIDGE_SCHEMAS_OUT_DIR}
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/next/bridgesdk/
)

#*********************************************************************************************************************
##End flatbuffers
#*********************************************************************************************************************

#*********************************************************************************************************************
##Begin Plugin
#*********************************************************************************************************************
set(TARGET_NAME next_plugin)

set(SRC_FILES
  interface/next/bridgesdk/datatypes.h
  interface/next/bridgesdk/plugin_config.h
  interface/next/bridgesdk/plugin.h
  Plugin/plugin.cpp
  Plugin/plugin_private.h
  Plugin/plugin_private.cpp
  interface/next/bridgesdk/plugin_addons/plugin_datapublisher.hpp
  Plugin/plugin_datapublisher.cpp
  Plugin/plugin_datapublisher_impl.hpp
  Plugin/plugin_datapublisher_impl.cpp
  interface/next/bridgesdk/plugin_addons/plugin_datasubscriber.hpp
  Plugin/plugin_datasubscriber.cpp
  Plugin/plugin_datasubscriber_impl.hpp
  Plugin/plugin_datasubscriber_impl.cpp
  interface/next/bridgesdk/plugin_addons/plugin_3D_view.hpp
  Plugin/plugin_3D_view.cpp
  Plugin/plugin_3D_view_impl.hpp
  Plugin/plugin_3D_view_impl.cpp
  interface/next/bridgesdk/plugin_addons/plugin_signal_explorer.hpp
  Plugin/plugin_signal_explorer.cpp
  Plugin/plugin_signal_explorer_impl.hpp
  Plugin/plugin_signal_explorer_impl.cpp
  interface/next/bridgesdk/plugin_addons/plugin_signal_extractor.hpp
  Plugin/plugin_signal_extractor.cpp
  Plugin/plugin_signal_extractor_impl.hpp
  Plugin/plugin_signal_extractor_impl.cpp
  Plugin/plugin_regex_searcher.cpp
  interface/next/bridgesdk/plugin_addons/plugin_regex_searcher.hpp
)

set(PUBLIC_LINK_LIBS
  mtscpp::mtscpp # Only the Magno SDK as link dependency
  next_sdk::next_sdk
  next_udex::next_udex
)
set(PRIVATE_LINK_LIBS
  base64
  Boost::filesystem
  Boost::regex
  jsoncpp_static
)

set(PUBLIC_INCLUDE_DIRS
  $<BUILD_INTERFACE:${BRIDGESDK_INTERFACE_DIR}>
  $<INSTALL_INTERFACE:include>
)
set(PRIVATE_INCLUDE_DIRS
  Plugin
  ${DATABRIDGE_SRC_DIR}/interface
  ${DATABRIDGE_SRC_DIR}/PluginManager
  ${PLUGINS_INTERFACE_DIR}
)

next_create_library(NAME create_bfbs_header
  TYPE STATIC
  SRC_FILES dummy.cpp
  PRIVATE_LINK_LIBS next_flatbuffers_view_update
)
add_custom_command(TARGET create_bfbs_header POST_BUILD
  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/write_bfbs_to_header.cmake
)

next_create_library(
  NAME ${TARGET_NAME}
  TYPE SHARED
  SRC_FILES ${SRC_FILES}
  USE_VERSION_INFO TRUE
  FOLDER_VIEW Bridges/BridgeSDK
  PUBLIC_LINK_LIBS ${PUBLIC_LINK_LIBS}
  PRIVATE_LINK_LIBS ${PRIVATE_LINK_LIBS}
  PUBLIC_INCLUDE_DIRS ${PUBLIC_INCLUDE_DIRS}
  PRIVATE_INCLUDE_DIRS ${PRIVATE_INCLUDE_DIRS}
)

add_dependencies(next_plugin
  next_flatbuffers_3d_view
  next_flatbuffers_profiler
  next_flatbuffers
  next_flatbuffers_view_update
  create_bfbs_header
)

add_library(next_bridges::next_plugin ALIAS ${TARGET_NAME})

include(GNUInstallDirs)

install_runtime(
  TARGET_NAME ${TARGET_NAME}
  COMPONENT BRIDGES_COMPONENT
  DIRECTORY ${BRIDGESDK_INTERFACE_DIR}/
)

#//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
##End Plugin
#//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#*********************************************************************************************************************
##Begin Webserver
#*********************************************************************************************************************
set(WEB_SERVER web_server)

set(SRC_FILES_WEB
  ${BRIDGESDK_INTERFACE_DIR}/next/bridgesdk/web_server.hpp
  WebServer/tcp_session.cpp
  WebServer/web_server_impl.cpp
  WebServer/include/tcp_session.hpp
  WebServer/include/web_server_impl.hpp
)

set(PUBLIC_INCLUDE_DIRS_WEB
  $<BUILD_INTERFACE:${BRIDGESDK_INTERFACE_DIR}>
  $<INSTALL_INTERFACE:include>
)
set(PRIVATE_INCLUDE_DIRS_WEB
  WebServer/include
  interface
)

set(PRIVATE_LINK_LIBS_WEB
  next_sdk::next_sdk
  Boost::json
)

set(TARGET_COMPILE_PRIVATE_OPT_WEB
  # silence some C++17 deprecation warnings, produced by boost asio (e.g. std::allocator<void>)
  # https://github.com/chriskohlhoff/asio/issues/290#issuecomment-377727614
  _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING
  _SILENCE_CXX17_ADAPTOR_TYPEDEFS_DEPRECATION_WARNING
)

next_create_library(
  NAME ${WEB_SERVER}
  TYPE STATIC
  SRC_FILES ${SRC_FILES_WEB}
  FOLDER_VIEW Bridges/BridgeSDK
  TARGET_COMPILE_PRIVATE_DEF ${TARGET_COMPILE_PRIVATE_OPT_WEB}
  PRIVATE_LINK_LIBS ${PRIVATE_LINK_LIBS_WEB}
  PUBLIC_INCLUDE_DIRS ${PUBLIC_INCLUDE_DIRS_WEB}
  PRIVATE_INCLUDE_DIRS ${PRIVATE_INCLUDE_DIRS_WEB}
)

if(UNIX)
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)
  target_link_libraries(${WEB_SERVER} PRIVATE Threads::Threads)
endif(UNIX)

#//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
##End Webserver
#//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

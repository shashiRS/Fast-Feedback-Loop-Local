set(TARGET_NAME next_udex)

set(SRC_FILES
  src/explorer/signal_explorer.cpp
  src/explorer/signal_explorer_impl.cpp

  src/extractor/data_extractor.cpp
  src/extractor/data_extractor_impl.cpp
  src/extractor/struct_extractor_array_factory.cpp
  src/extractor/struct_extractor_basic.cpp
  src/extractor/package_tree_extractor.cpp
  src/extractor/sync_algo.cpp
  src/extractor/sync_algo_loader.cpp
  src/extractor/udex_queue.cpp
  src/extractor/gia/gia_dll_handler.cpp

  src/publisher/data_publisher.cpp
  src/publisher/data_publisher_impl.cpp

  src/signal_storage/core_lib_wrap_data_converter.cpp
  src/signal_storage/core_lib_wrap_data_descriptor_parser.cpp
  src/signal_storage/signal_description_handler.cpp
  src/signal_storage/signal_tree_module.cpp
  src/signal_storage/signal_watcher.cpp
  src/signal_storage/udex_dynamic_publisher.cpp
  src/signal_storage/udex_dynamic_subscriber.cpp

  src/subscriber/data_subscriber.cpp
  src/subscriber/data_subscriber_impl.cpp

  src/udex_manager.cpp
)

set(RC_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/version_info/version_info.rc)
set(VERSION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/src/version_info/version.h.in)

set(PUBLIC_LINK_LIBS
  mtscpp::mtscpp # Magno SDK as link dependency
  mts_ecu::mts_ecu # Magno ECU Toolbox, CMake interface library
  mts_can::mts_can # Magno CAN Toolbox, CMake interface library
  mts_eth::mts_eth # Magno Ethernet Toolbox, CMake interface library
  mts_flexray::mts_flexray # Magno FlexRay Toolbox, CMake interface library
  mts_rt_range::mts_rt_range # Magno Reference Sensor Toolbox, CMake interface library
  mts_gps::mts_gps # Magno Reference Sensor Toolbox, CMake interface library
  mts_ref_cam::mts_ref_cam # Magno Reference Sensor Toolbox, CMake interface library
  next_sdk::next_sdk
)

set(PRIVATE_LINK_LIBS
  next_sdk::next_appsupport
  Boost::json
  Boost::system
  Boost::filesystem
  Boost::date_time # required by filesystem
  eCAL::core
  jsoncpp_static
)

set(PUBLIC_INCLUDE_DIRS
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/interface>
  $<INSTALL_INTERFACE:include>
)

next_create_library(
  NAME ${TARGET_NAME}
  TYPE SHARED
  SRC_FILES ${SRC_FILES}
  USE_VERSION_INFO TRUE
  RC_FILE ${RC_FILE}
  VERSION_FILE_PATH ${VERSION_FILE}
  FOLDER_VIEW UDex
  TARGET_COMPILE_PRIVATE_DEF UDEX_LIBRARY_BUILD
  PUBLIC_LINK_LIBS ${PUBLIC_LINK_LIBS}
  PRIVATE_LINK_LIBS ${PRIVATE_LINK_LIBS}
  PUBLIC_INCLUDE_DIRS ${PUBLIC_INCLUDE_DIRS}
)

set_target_properties(${TARGET_NAME}
  PROPERTIES
  C_VISIBILITY_PRESET hidden
  CXX_VISIBILITY_PRESET hidden
)

#target_compile_definitions(${TARGET_NAME}
#  PRIVATE
#    TRACY_ENABLE
#)

add_library(next_udex::next_udex ALIAS ${TARGET_NAME})

########################### Install
include(GNUInstallDirs)

install_runtime(
  TARGET_NAME ${TARGET_NAME}
  COMPONENT UDEX_COMPONENT
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/interface/
)

set(TARGET_NAME next)

set(SRC_FILES
  next_impl.hpp
  sync_manager/sync_manager.hpp
  sync_manager/syncref_handler.hpp
  sync_manager/sync_value_handler.hpp
  sync_manager/sync_config_reader.hpp
  ${NEXT_INTERFACE_DIR}/next/data_types.hpp
  ${NEXT_INTERFACE_DIR}/next/next.hpp
  ${NEXT_INTERFACE_DIR}/next/next_callbacks.hpp
  ${NEXT_INTERFACE_DIR}/next/next_config.h
  next.cpp
  next_callbacks.cpp
  next_impl.cpp
  next_inport.cpp
  next_outport.cpp
  sync_manager/sync_manager.cpp
  sync_manager/syncref_handler.cpp
  sync_manager/sync_value_handler.cpp
)

set(PUBLIC_LINK_LIBS
  next_control::next_control
  next_sdk::next_appsupport
  next_sdk::next_sdk
)
set(PRIVATE_LINK_LIBS
  next_udex::next_udex
  jsoncpp_static
  Boost::filesystem
)

set(PUBLIC_INCLUDE_DIRS
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${NEXT_INTERFACE_DIR}>
)
set(PRIVATE_INCLUDE_DIRS
  $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

next_create_library(
  NAME ${TARGET_NAME}
  TYPE SHARED
  SRC_FILES ${SRC_FILES}
  USE_VERSION_INFO TRUE
  FOLDER_VIEW Next
  PUBLIC_LINK_LIBS ${PUBLIC_LINK_LIBS}
  PRIVATE_LINK_LIBS ${PRIVATE_LINK_LIBS}
  TARGET_COMPILE_PUBLIC_OPT $<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus>
  PUBLIC_INCLUDE_DIRS ${PUBLIC_INCLUDE_DIRS}
  PRIVATE_INCLUDE_DIRS ${PRIVATE_INCLUDE_DIRS}
)

configure_file(config.h.in include/config.h)

########################### Install
include(GNUInstallDirs)

install_runtime(
  TARGET_NAME ${TARGET_NAME}
  COMPONENT NEXT_COMPONENT
  DIRECTORY ${NEXT_INTERFACE_DIR}/
)

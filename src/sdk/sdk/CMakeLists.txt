set(TARGET_NAME next_sdk)

set(SRC_FILES
  src/ecal_singleton.cpp
  src/event_base/base_event_binary_impl.cpp
  src/event_base/event_client.cpp
  src/event_base/event_client.h
  src/event_base/event_manager.cpp
  src/event_base/event_manager.h
  src/event_base/event_publisher.cpp
  src/event_base/event_publisher.h
  src/event_base/event_server.cpp
  src/event_base/event_server.h
  src/event_base/event_subscriber.cpp
  src/event_base/event_subscriber.h
  src/events/cycle_event.cpp
  src/events/cycle_information_event.cpp
  src/events/lifecycle_state.cpp
  src/events/lifecycle_trigger.cpp
  src/events/log_event.cpp
  src/events/request_config_from_client.cpp
  src/events/request_config_from_server.cpp
  src/events/request_config_value_from_server.cpp
  src/events/send_config_to_client.cpp
  src/events/send_config_tags_to_server.cpp
  src/events/send_config_to_server.cpp
  src/events/send_config_value_to_client.cpp
  src/events/status_code_event.cpp
  src/events/session_event.cpp
  src/events/update_topic_cache.cpp
  src/events/version_info.cpp
  src/lifecycle_interface.cpp
  src/logger.cpp
  src/log_router.hpp
  src/log_router.cpp
  src/profiler.cpp
  src/shutdown_handler.cpp
  src/simple_init_chain.cpp
  src/user_hash/user_hash.cpp
  src/version_manager.cpp

  ${SDK_INTERFACE_DIR}/next/sdk/sdk.hpp
  ${SDK_INTERFACE_DIR}/next/sdk/sdk_config.hpp
  ${SDK_INTERFACE_DIR}/next/sdk/sdk_macros.h
  ${SDK_INTERFACE_DIR}/next/sdk/ecal/ecal_singleton.hpp
  ${SDK_INTERFACE_DIR}/next/sdk/event_base/base_event.h
  ${SDK_INTERFACE_DIR}/next/sdk/event_base/base_event_binary.h
  ${SDK_INTERFACE_DIR}/next/sdk/event_base/base_event_binary_blocking.h
  ${SDK_INTERFACE_DIR}/next/sdk/event_base/base_event_binary_impl.h
  ${SDK_INTERFACE_DIR}/next/sdk/event_base/event_types.h
  ${SDK_INTERFACE_DIR}/next/sdk/events/cycle_event.hpp
  ${SDK_INTERFACE_DIR}/next/sdk/events/cycle_information_event.hpp
  ${SDK_INTERFACE_DIR}/next/sdk/events/lifecycle_state.h
  ${SDK_INTERFACE_DIR}/next/sdk/events/lifecycle_trigger.h
  ${SDK_INTERFACE_DIR}/next/sdk/events/log_event.hpp
  ${SDK_INTERFACE_DIR}/next/sdk/events/request_config_from_client.h
  ${SDK_INTERFACE_DIR}/next/sdk/events/request_config_from_server.h
  ${SDK_INTERFACE_DIR}/next/sdk/events/request_config_value_from_server.h
  ${SDK_INTERFACE_DIR}/next/sdk/events/send_config_to_client.h
  ${SDK_INTERFACE_DIR}/next/sdk/events/send_config_tags_to_server.h
  ${SDK_INTERFACE_DIR}/next/sdk/events/send_config_to_server.h
  ${SDK_INTERFACE_DIR}/next/sdk/events/send_config_value_to_client.h
  ${SDK_INTERFACE_DIR}/next/sdk//events/status_code_event.hpp
  ${SDK_INTERFACE_DIR}/next/sdk/events/session_event.hpp
  ${SDK_INTERFACE_DIR}/next/sdk/events/update_topic_cache.h
  ${SDK_INTERFACE_DIR}/next/sdk/events/version_info.h
  ${SDK_INTERFACE_DIR}/next/sdk/init_chain/simple_init_chain.hpp
  ${SDK_INTERFACE_DIR}/next/sdk/lifecycle/lifecycle_interface.hpp
  ${SDK_INTERFACE_DIR}/next/sdk/logger/logger.hpp
  ${SDK_INTERFACE_DIR}/next/sdk/logger/logger_data_types.hpp
  ${SDK_INTERFACE_DIR}/next/sdk/profiler/profiler.hpp
  ${SDK_INTERFACE_DIR}/next/sdk/shutdown_handler/shutdown_handler.hpp
  ${SDK_INTERFACE_DIR}/next/sdk/string_helper/strings.hpp
  ${SDK_INTERFACE_DIR}/next/sdk/types/package_data_types.hpp
  ${SDK_INTERFACE_DIR}/next/sdk/user_hash/user_hash.hpp
  ${SDK_INTERFACE_DIR}/next/sdk/version_manager/version_manager.hpp
)

set(PUBLIC_LINK_LIBS
  eCAL::core
  eCAL::core_c # for components written in C
  mts_sentinel::client
)
set(PRIVATE_LINK_LIBS
  Boost::filesystem
  Boost::log
  jsoncpp_static
)


set(PUBLIC_INCLUDE_DIRS
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${SDK_INTERFACE_DIR}>
)

next_create_library(
  NAME ${TARGET_NAME}
  TYPE SHARED
  SRC_FILES ${SRC_FILES}
  USE_VERSION_INFO TRUE
  FOLDER_VIEW SDK
  TARGET_COMPILE_PUBLIC_DEF SPDLOG_COMPILED_LIB
  TARGET_COMPILE_PUBLIC_OPT $<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus>
  PUBLIC_LINK_LIBS ${PUBLIC_LINK_LIBS}
  PRIVATE_LINK_LIBS ${PRIVATE_LINK_LIBS}
  PUBLIC_INCLUDE_DIRS ${PUBLIC_INCLUDE_DIRS}
)

#target_compile_definitions(next_sdk
#  PRIVATE
#    TRACY_ENABLE
#)

########################### SPDLog and TracyClient
find_package(spdlog 1.6.0 REQUIRED)

list(APPEND libs_to_check "spdlog::spdlog")

# Tracy requires specific links:
list(APPEND additional_libraries "$<LINK_ONLY:Threads::Threads>")
if(NOT MSVC)
  list(APPEND additional_libraries "$<LINK_ONLY:dl>")
endif()

foreach(cur_lib ${libs_to_check})
  message("======= checking lib ${cur_lib}")
  get_target_property(tmp_include_dir ${cur_lib} INTERFACE_INCLUDE_DIRECTORIES)
  message("include dir: ${tmp_include_dir}")
  get_target_property(tmp_link_libraries ${cur_lib} INTERFACE_LINK_LIBRARIES)
  message("link libs: ${tmp_link_libraries}")
  get_target_property(tmp_imported_configurations ${cur_lib} IMPORTED_CONFIGURATIONS)
  foreach(imported_config ${tmp_imported_configurations})
    get_target_property(tmp_location_tmp ${cur_lib} IMPORTED_LOCATION_${imported_config})
    message("location: ${tmp_location_tmp}")
    get_filename_component(tmp_location_dir_tmp ${tmp_location_tmp} DIRECTORY)
    message("location dir: ${tmp_location_dir_tmp}")
    list(APPEND imported_locations ${tmp_location_dir_tmp})
    set(tmp_location_dir_tmp)
    set(tmp_location_tmp)
  endforeach()
  set(tmp_imported_configurations)
  list(APPEND additional_include_dirs ${tmp_include_dir})
  list(REMOVE_DUPLICATES additional_include_dirs)
  message("additional inc dirs: ${additional_include_dirs}")
  set(tmp_include_dir)
  list(APPEND additional_link_dirs ${imported_locations})
  list(REMOVE_DUPLICATES additional_link_dirs)
  message("additional link dirs: ${additional_link_dirs}")
  set(imported_location)
  list(APPEND additional_libraries ${tmp_link_libraries})
  list(REMOVE_DUPLICATES additional_libraries)
  message("additional libs: ${additional_libraries}")
  set(tmp_link_libraries)
  message("======= end checking lib ${cur_lib}")
endforeach()


target_include_directories(${TARGET_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${additional_include_dirs}>
)
target_link_directories(${TARGET_NAME}
  PUBLIC
    $<BUILD_INTERFACE:${additional_link_dirs}>
  INTERFACE
    $<INSTALL_INTERFACE:lib>
)
target_link_libraries(${TARGET_NAME}
  PUBLIC
    spdlog::spdlog
    #Tracy::TracyClient
    ${additional_libraries}
)

########################### Install
include(GNUInstallDirs)

add_library(next_sdk::next_sdk ALIAS ${TARGET_NAME})

install_runtime(
  TARGET_NAME ${TARGET_NAME}
  COMPONENT SDK_COMPONENT
  DIRECTORY ${SDK_INTERFACE_DIR}/
)

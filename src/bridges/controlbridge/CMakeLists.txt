set(CMAKE_INSTALL_RPATH_BACKUP ${CMAKE_INSTALL_RPATH})
set(CMAKE_INSTALL_RPATH "$\{ORIGIN\};$\{ORIGIN\}/../lib")

#*********************************************************************************************************************
##Begin ControlBridge Library
#*********************************************************************************************************************

set(CONTROLBRIDGE_LIB controlbridge_lib)

set(EXTRA_SRC_FILES
  CommandHandler/PlayerCommandHandler/close_handler.cpp
  CommandHandler/PlayerCommandHandler/close_handler.h
  CommandHandler/PlayerCommandHandler/fastforward_handler.cpp
  CommandHandler/PlayerCommandHandler/fastforward_handler.h
  CommandHandler/PlayerCommandHandler/jump_handler.cpp
  CommandHandler/PlayerCommandHandler/jump_handler.h
  CommandHandler/PlayerCommandHandler/jump_to_end_handler.cpp
  CommandHandler/PlayerCommandHandler/jump_to_end_handler.h
  CommandHandler/PlayerCommandHandler/open_binary_handler.cpp
  CommandHandler/PlayerCommandHandler/open_binary_handler.h
  CommandHandler/PlayerCommandHandler/open_file_handler.cpp
  CommandHandler/PlayerCommandHandler/open_file_handler.h
  CommandHandler/PlayerCommandHandler/open_project_directory_handler.cpp
  CommandHandler/PlayerCommandHandler/open_project_directory_handler.h
  CommandHandler/PlayerCommandHandler/pause_handler.cpp
  CommandHandler/PlayerCommandHandler/pause_handler.h
  CommandHandler/PlayerCommandHandler/play_handler.cpp
  CommandHandler/PlayerCommandHandler/play_handler.h
  CommandHandler/PlayerCommandHandler/rewind_handler.cpp
  CommandHandler/PlayerCommandHandler/rewind_handler.h
  CommandHandler/PlayerCommandHandler/step_handler.cpp
  CommandHandler/PlayerCommandHandler/step_handler.h
  CommandHandler/PlayerCommandHandler/update_sil_factor_handler.cpp
  CommandHandler/PlayerCommandHandler/update_sil_factor_handler.h
  ConfigManager/config_manager.h
  ConfigManager/config_manager.cpp
  log_distributor/log_distributor.hpp
  status_code_manager/status_code_manager.hpp
  status_code_manager/status_code_manager.cpp
  status_queue_manager/condition.hpp
  status_queue_manager/condition.cpp
  status_queue_manager/condition_handler.hpp
  status_queue_manager/condition_handler.cpp
  status_queue_manager/status_queue_manager.hpp
  status_queue_manager/status_queue_manager.cpp
  TelemetryHandler/telemetry_handler.h
  TelemetryHandler/telemetry_handler.cpp
)

set(SRC_FILES_CONTROLBRIDGE_LIB
  CommandHandler/player_control_manager.cpp
  CommandHandler/gui_command_parser.cpp
  EventHandler/player_event_distributor.cpp
  EventHandler/version_info_event_distributor.cpp
  LifeCycleOrchestrator/lifecycle_orchestrator.cpp
  log_distributor/log_distributor.cpp
  http_post/http_post.cpp
  http_post/websocket_client_async.cpp
)
list(APPEND SRC_FILES_CONTROLBRIDGE_LIB ${EXTRA_SRC_FILES})

set(PUBLIC_INCLUDE_DIRS_CONTROLBRIDGE_LIB
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/CommandHandler>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/LifeCycleOrchestrator>
  $<BUILD_INTERFACE:${CONTROLBRIDGE_INTERFACE_DIR}>
  $<INSTALL_INTERFACE:include>
)

set(PUBLIC_LINK_LIBS_CONTROLBRIDGE_LIB
  next_control::next_control
  next_sdk::next_sdk
  web_server
  next_sdk::next_appsupport
  jsoncpp_static
)

set(PRIVATE_LINK_LIBS_CONTROLBRIDGE_LIB
  eCAL::core
  Boost::filesystem
  Boost::date_time # filesystem requires date_time
  Boost::system
)

set(TARGET_COMPILE_PRIVATE_OPT_CONTROLBRIDGE_LIB
  # silence some C++17 deprecation warnings, produced by boost asio (e.g. std::allocator<void>)
  # https://github.com/chriskohlhoff/asio/issues/290#issuecomment-377727614
  _SILENCE_CXX17_ALLOCATOR_VOID_DEPRECATION_WARNING
  _SILENCE_CXX17_ADAPTOR_TYPEDEFS_DEPRECATION_WARNING
)

next_create_library(
  NAME ${CONTROLBRIDGE_LIB}
  TYPE STATIC
  SRC_FILES ${SRC_FILES_CONTROLBRIDGE_LIB}
  FOLDER_VIEW Bridges/Controlbridge
  TARGET_COMPILE_PRIVATE_DEF ${TARGET_COMPILE_PRIVATE_OPT_CONTROLBRIDGE_LIB}
  PUBLIC_LINK_LIBS ${PUBLIC_LINK_LIBS_CONTROLBRIDGE_LIB}
  PRIVATE_LINK_LIBS ${PRIVATE_LINK_LIBS_CONTROLBRIDGE_LIB}
  PUBLIC_INCLUDE_DIRS ${PUBLIC_INCLUDE_DIRS_CONTROLBRIDGE_LIB}
)

if (UNIX)
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)
  target_link_libraries(${CONTROLBRIDGE_LIB} PRIVATE Threads::Threads)
endif(UNIX)

#//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
##End ControlBridge Library
#//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
set(TARGET_NAME next_controlbridge)

set(SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
list(APPEND SRC_FILES ${EXTRA_SRC_FILES})

set(RC_FILE ${CMAKE_CURRENT_SOURCE_DIR}/version_info/version_info.rc)
set(VERSION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/version_info/version.h.in)

set(PRIVATE_LINK_LIBS
  controlbridge_lib
  Boost::boost
  next_sdk::next_appsupport
)

next_create_executable(
  NAME ${TARGET_NAME}
  SRC_FILES ${SRC_FILES}
  USE_VERSION_INFO TRUE
  RC_FILE ${RC_FILE}
  VERSION_FILE_PATH ${VERSION_FILE}
  FOLDER_VIEW Bridges/Controlbridge
  PRIVATE_LINK_LIBS ${PRIVATE_LINK_LIBS}
)

install_runtime(
  TARGET_NAME ${TARGET_NAME}
  COMPONENT BRIDGES_COMPONENT
)

if(MSVC)
  set_target_properties(${TARGET_NAME} PROPERTIES VS_DEBUGGER_COMMAND_ARGUMENTS "-i ${CMAKE_CURRENT_SOURCE_DIR}/conf/${TARGET_NAME}.ini")

  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${ECAL_INI_FILE} $<TARGET_FILE_DIR:${TARGET_NAME}>/..
  )
endif(MSVC)

if(UNIX)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath=../lib")

  # Newer versions of the linker in some platforms, emit DT_RUNPATH where they used to emit DT_RPATH
  # To replicate the old behaviour on versions of the linker that emit RUNPATH instead, pass the --disable-new-dtags flag to the linker
  target_link_libraries(${TARGET_NAME} PRIVATE "-Wl,--disable-new-dtags")

  add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${ECAL_INI_FILE} $<TARGET_FILE_DIR:${TARGET_NAME}>
  )
endif(UNIX)

#install_lib(libname Tracy::TracyClient targetname ${TARGET_NAME})

#//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
##End main
#//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_RPATH_BACKUP})

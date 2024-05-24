set(TEST_PACKAGE plugin_manager_test)

set(TEST_RESOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/ap_pdo_tool.sdl
)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS 
    output_schema_checker_test.cpp
    plugin_config_handler_test.cpp
    plugin_manager_test.cpp
    process_manager_test.cpp 
    ${PLUGIN_MANAGER_SRC_DIR}/plugin_manager.cpp
    ${PLUGIN_MANAGER_SRC_DIR}/plugin_config_handler.cpp
    ${PLUGIN_MANAGER_SRC_DIR}/process_manager.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unittest_mock_plugin_manager.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unittest_plugin_manager_helper.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MockPlugins/plugin_sdl_mock.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MockPlugins/plugin_concurrency_mock.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/MockPlugins/plugin_errorHandling_mock.hpp
  RESOURCES ${TEST_RESOURCES}
  PRIVATEINCLUDEDIRS
    ${PLUGIN_MANAGER_SRC_DIR}/OutputSchema
    MockPlugins
    ${UNIT_TEST_COMMON_DIR}
    ${PLUGINS_INTERFACE_DIR}
  PRIVATELINKLIBS
    next_udex::next_udex
    next_control::next_control
    databridge_lib
    Boost::date_time
    Boost::filesystem
    jsoncpp_static
    next_sdk::next_sdk
  USESDB
  INSTALLTARGETS
    mts_sentinel::client
    mts_core_lib::modules
)

if(MSVC)
  add_custom_command(TARGET ${TEST_PACKAGE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${TEST_PACKAGE}> $<TARGET_FILE_DIR:${TEST_PACKAGE}>
    COMMAND_EXPAND_LISTS
  )
endif()

add_custom_command(TARGET ${TEST_PACKAGE} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${BRIDGESDK_INTERFACE_DIR}/next/bridgesdk/schema/3d_view/groundline.fbs" $<TARGET_FILE_DIR:${TEST_PACKAGE}>
  COMMAND ${CMAKE_COMMAND} -E copy_if_different "${BRIDGESDK_INTERFACE_DIR}/next/bridgesdk/schema/3d_view/trafficparticipant.fbs" $<TARGET_FILE_DIR:${TEST_PACKAGE}>
)

install(
  FILES
    ${BRIDGESDK_INTERFACE_DIR}/next/bridgesdk/schema/3d_view/groundline.fbs
    ${BRIDGESDK_INTERFACE_DIR}/next/bridgesdk/schema/3d_view/trafficparticipant.fbs
  COMPONENT __CIP_TEST_COMMON
  DESTINATION tests/cases/${TEST_PACKAGE} # is there a variable for this?
)

add_subdirectory(MockPlugins)

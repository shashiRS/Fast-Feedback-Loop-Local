
set(TEST_PACKAGE simple_ifc)

set(TEST_SRC
  simple_ifc.cpp
  ${NEXT_SRC_DIR}/next.cpp
  ${NEXT_SRC_DIR}/next_callbacks.cpp
  ${NEXT_SRC_DIR}/next_impl.cpp
  ${NEXT_SRC_DIR}/next_inport.cpp
  ${NEXT_SRC_DIR}/next_outport.cpp
  ${NEXT_SRC_DIR}/sync_manager/sync_manager.cpp
  ${NEXT_SRC_DIR}/sync_manager/syncref_handler.cpp
)

set(TEST_RESOURCES
  ${CMAKE_CURRENT_LIST_DIR}/test_signal.sdl
  ${CMAKE_CURRENT_LIST_DIR}/test_signal2.sdl
  ${CMAKE_CURRENT_LIST_DIR}/test_signal3.sdl
  ${CMAKE_CURRENT_LIST_DIR}/node_conf.json
)

configure_file(${NEXT_SRC_DIR}/config.h.in include/config.h)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS ${TEST_SRC}
  RESOURCES ${TEST_RESOURCES}
  PRIVATEINCLUDEDIRS
    ${NEXT_INTERFACE_DIR}
    ${NEXT_SRC_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/include
  PRIVATELINKLIBS
    Boost::filesystem
    next_control::next_control
    next_sdk::next_sdk
    next_sdk::next_appsupport
    next_udex::next_udex
    jsoncpp_static
  INSTALLTARGETS
    mts_core_lib::modules
    mts_sentinel::sentinel
    mts_sentinel::msdia
  USE_CUSTOM_ECAL_DOMAIN
)

target_compile_definitions(${TEST_PACKAGE} PUBLIC NEXT_LIBRARY_STATIC)

if(MSVC)
    add_custom_command(TARGET ${TEST_PACKAGE} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:${TEST_PACKAGE}>
        $<TARGET_FILE_DIR:${TEST_PACKAGE}>
        COMMAND_EXPAND_LISTS
    )

    install(
        FILES
        $<TARGET_RUNTIME_DLLS:${TEST_PACKAGE}>
        COMPONENT __CIP_TEST_COMMON
        DESTINATION tests/cases/${TEST_PACKAGE}
    )
endif()

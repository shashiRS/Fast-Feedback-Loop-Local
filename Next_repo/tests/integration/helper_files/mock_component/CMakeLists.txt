set(TEST_PACKAGE dummy_component)

add_executable(${TEST_PACKAGE} mock_component.cpp)

target_include_directories(${TEST_PACKAGE}
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/interface>
    $<INSTALL_INTERFACE:interface>
  #PRIVATE
    #${PLUGIN_UTILS_DIR}
    #${BRIDGE_SDK_FLATBUFFERS_SCHEMA_DIR}
)

target_link_libraries(${TEST_PACKAGE}
  PRIVATE
    next_udex::next_udex
    next_sdk::next_appsupport
    next_sdk::next_sdk
)

set_target_properties(
  ${TEST_PACKAGE}
  PROPERTIES
  # for any reason it is not possible to use a Visual Studio variable here (e.g. "$(TargetDir)").
  VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${TEST_PACKAGE}>
)

set_target_properties(${TEST_PACKAGE} PROPERTIES FOLDER "tests/integration")

# add_custom_command(TARGET ${TEST_PACKAGE} POST_BUILD
# 	COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${TEST_PACKAGE}> $<TARGET_FILE_DIR:${TEST_PACKAGE}>
# 	COMMAND_EXPAND_LISTS
# )

# copying the files into the package folder
if(MSVC)
  get_target_property(libs mts_core_lib::modules INTERFACE_LINK_LIBRARIES)
  foreach(lib ${libs})
    get_target_property(target_type ${lib} TYPE)
    if(target_type STREQUAL "SHARED_LIBRARY")
      # copy into the build folder (e.g. for debugging)
      add_custom_command(TARGET ${TEST_PACKAGE} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${lib}> $<TARGET_FILE_DIR:${TEST_PACKAGE}>
      )
      # installing into package folder
      install(IMPORTED_RUNTIME_ARTIFACTS ${lib} COMPONENT __CIP_TEST_COMMON DESTINATION tests/cases/${TEST_PACKAGE})
    endif()
  endforeach()
  
  install(
      FILES $<TARGET_RUNTIME_DLLS:${TEST_PACKAGE}>
      COMPONENT __CIP_TEST_COMMON
      DESTINATION tests/cases/${TEST_PACKAGE}
  )
  
  add_custom_command(TARGET ${TEST_PACKAGE} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${TEST_PACKAGE}> $<TARGET_FILE_DIR:${TEST_PACKAGE}>
    COMMAND_EXPAND_LISTS
  )
  install_mts_sentinel(
    COMPONENT __CIP_TEST_COMMON
    PATH $<TARGET_FILE_DIR:${TEST_PACKAGE}>
  )
endif(MSVC)

install_mts_sentinel(
  COMPONENT __CIP_TEST_COMMON
  PATH tests/cases/${TEST_PACKAGE}
)

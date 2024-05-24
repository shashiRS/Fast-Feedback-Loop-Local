set(TEST_PACKAGE system_PerformanceViewer_test)
find_package(eCAL REQUIRED)

if(MSVC)
  get_target_property(libraries mts_core_lib::modules INTERFACE_LINK_LIBRARIES)
  foreach(library ${libraries})
    get_target_property(target_type ${library} TYPE)
    if(target_type STREQUAL "SHARED_LIBRARY")
      install(
        FILES $<TARGET_FILE:${library}>
        COMPONENT __CIP_TEST_COMMON
        DESTINATION tests/cases/${TEST_PACKAGE}
      )
      # copy files into build folder for testing with IDE
      install(
        FILES $<TARGET_FILE:${library}>
        COMPONENT __CIP_TEST_COMMON
        DESTINATION $<TARGET_FILE_DIR:${TEST_PACKAGE}>
      )
    endif()
  endforeach()
endif(MSVC)

next_create_test(
    NAME ${TEST_PACKAGE}
    SRCS
      performance_viewer_test.cpp
      cpu_profiler_test.cpp
      ${PLUGINS_SRC_DIR}/core/system_PerformanceViewer/performance_viewer.cpp
      ${PLUGINS_SRC_DIR}/core/system_PerformanceViewer/next_cpu_profiler.cpp
    PRIVATEINCLUDEDIRS
      ${PLUGINS_SRC_DIR}/core/system_PerformanceViewer
      ${BRIDGESDK_INTERFACE_DIR}
      ${PLUGINS_INTERFACE_DIR}        
      ${PERFORMANCEVIEWER_INTERFACE_DIR}
    PRIVATELINKLIBS
      eCAL::core
      eCAL::pb         
      flatbuffers::flatbuffers
      jsoncpp_static
      next_plugin
      next_flatbuffers_profiler
    USESDB
)

add_dependencies(${TEST_PACKAGE}
  GENERATE_next_flatbuffers_profiler
)

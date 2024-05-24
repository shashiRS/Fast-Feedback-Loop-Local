set(Plugin_NAME system_PerformanceViewer)

next_create_plugin(
  NAME
    ${Plugin_NAME}
  SRCS
    performance_viewer.h
    performance_viewer.cpp
    next_cpu_profiler.cpp
  PRIVATEINCLUDEDIRS
    ${PERFORMANCEVIEWER_INTERFACE_DIR}
    ${PLUGINS_INTERFACE_DIR}
    ${BRIDGESDK_INTERFACE_DIR}
  PRIVATELINKLIBS
    eCAL::core
    eCAL::pb
    flatbuffers::flatbuffers
    jsoncpp_static
    next_plugin
    next_flatbuffers_profiler
  RESOURCES
    ${BRIDGESDK_SCHEMA_DIR}/profiler/basic_info.fbs
    ${BRIDGESDK_SCHEMA_DIR}/profiler/cpu_profiler.fbs
  TYPE
    core
  INSTALL_TARGET
  BRIDGES_COMPONENT
)

add_dependencies(${Plugin_NAME} GENERATE_next_flatbuffers_profiler)

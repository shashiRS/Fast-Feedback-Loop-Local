set(Plugin_NAME system_ECalMonitor)

next_create_plugin(
  NAME
    ${Plugin_NAME}
  SRCS
    ecal_monitor.cpp
  PRIVATEINCLUDEDIRS
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
    ${BRIDGESDK_SCHEMA_DIR}/profiler/ecal_monitor.fbs
    ${BRIDGESDK_SCHEMA_DIR}/profiler/basic_info.fbs
    ${BRIDGESDK_SCHEMA_DIR}/profiler/network_graph.fbs
  TYPE
    core
  INSTALL_TARGET
  BRIDGES_COMPONENT
)

add_dependencies(${Plugin_NAME} GENERATE_next_flatbuffers_profiler)

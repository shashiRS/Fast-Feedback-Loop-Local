set(Plugin_NAME system_RecExporter)

next_create_plugin(
  NAME
    ${Plugin_NAME}
  SRCS
    rec_exporter.h
    rec_exporter.cpp
  PRIVATEINCLUDEDIRS
    ${PLUGINS_INTERFACE_DIR}
  PRIVATELINKLIBS
    Boost::filesystem
    jsoncpp_static
    next_plugin
    next_sdk::next_appsupport
    mts_swc::mts_swc # Magno SWC Toolbox
    mts_ecu::mts_ecu # Magno ECU Toolbox, CMake interface library
  RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/rec_exporter.fbs
    UNICODE
  TYPE
    core
  INSTALL_TARGET
  BRIDGES_COMPONENT
)

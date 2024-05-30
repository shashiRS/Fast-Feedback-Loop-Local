set(Plugin_NAME system_SignalList)

next_create_plugin(
  NAME
    ${Plugin_NAME}
  SRCS
    signal_list.h
    signal_list.cpp
  PRIVATEINCLUDEDIRS
    ${PLUGINS_INTERFACE_DIR}
  PRIVATELINKLIBS
    next_plugin
    jsoncpp_static
  RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/signalList.fbs
  TYPE
    core
  INSTALL_TARGET
  BRIDGES_COMPONENT
)

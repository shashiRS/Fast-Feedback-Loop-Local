set(Plugin_NAME system_ReferenceCamera)

next_create_plugin(
  NAME
    ${Plugin_NAME}
  SRCS
    reference_camera.h
    reference_camera.cpp
  PRIVATEINCLUDEDIRS
    ${PLUGINS_INTERFACE_DIR}
  PRIVATELINKLIBS
    next_plugin
    jsoncpp_static
    base64
  RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/reference_camera.fbs
  TYPE
    core
  INSTALL_TARGET
  BRIDGES_COMPONENT
)

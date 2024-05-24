set(Plugin_NAME system_RawFrameView)
set(OpenCV_STATIC TRUE)
find_package(OpenCV REQUIRED core imgproc imgcodecs)

next_create_plugin(
  NAME
    ${Plugin_NAME}
  SRCS
    raw_frame_view.h
    raw_frame_view.cpp
  PRIVATEINCLUDEDIRS
    ${PLUGINS_INTERFACE_DIR}
  PRIVATELINKLIBS
    next_plugin
    base64
    jsoncpp_static
    ${OpenCV_LIBS}
  RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/description.json
    ${CMAKE_CURRENT_SOURCE_DIR}/raw_frame_view.fbs
    ${CMAKE_CURRENT_SOURCE_DIR}/raw_frame_plugin.ini
  TYPE
    core
  INSTALL_TARGET
  BRIDGES_COMPONENT
)

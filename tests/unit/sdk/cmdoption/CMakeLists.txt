configure_file(${APPSUPPORT_SRC_DIR}/config.h.in ${CMAKE_CURRENT_BINARY_DIR}/include/config.h)

next_create_test(
  NAME cmdoptions_test
  SRCS cmdoptions_test.cpp
  PRIVATELINKLIBS 
    next_appsupport
  PRIVATEINCLUDEDIRS
    ${CMAKE_CURRENT_BINARY_DIR}/include
  RESOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/cfg.json
)

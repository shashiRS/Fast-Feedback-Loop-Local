next_create_test(
  NAME config_change_hook_handler_test
  SRCS 
    config_change_hook_handler_test.cpp
    ${APPSUPPORT_SRC_DIR}/src/config/config_change_hook_handler.cpp
  PRIVATELINKLIBS
    next_appsupport
    next_sdk
  PRIVATEINCLUDEDIRS
    ${APPSUPPORT_SRC_DIR}/src/config
)

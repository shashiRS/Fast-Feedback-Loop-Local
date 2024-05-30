next_create_test(
    NAME plugins_utils_test
    SRCS plugins_utils_test.cpp
    PRIVATELINKLIBS
      next_plugin
    PRIVATEINCLUDEDIRS
      ${BRIDGESDK_INTERFACE_DIR}
      ${PLUGINS_INTERFACE_DIR}      
)

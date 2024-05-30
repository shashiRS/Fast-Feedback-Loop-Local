next_create_test(
  NAME plugin_helper_data_extractor_test
  SRCS
    plugin_helper_data_extractor_test.cpp
  PRIVATEINCLUDEDIRS
    ${BRIDGESDK_INTERFACE_DIR}
  PRIVATELINKLIBS
    next_udex::next_udex
  BIGOBJECT
  INSTALLTARGETS
    mts_sentinel::client
)

next_create_test(
  NAME app_lifecycle_test
  SRCS app_lifecycle_test.cpp
  PRIVATEINCLUDEDIRS ${SDK_SRC_DIR}/src/event_base
  PRIVATELINKLIBS 
    next_appsupport
  INSTALLTARGETS
    mts_sentinel::sentinel
    mts_sentinel::msdia
  USE_CUSTOM_ECAL_DOMAIN
)

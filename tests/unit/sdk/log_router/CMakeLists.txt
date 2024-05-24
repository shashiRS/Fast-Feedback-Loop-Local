next_create_test(
  NAME log_router
  SRCS
    log_router_test.cpp
    ${SDK_SRC_DIR}/src/log_router.cpp
  PRIVATELINKLIBS
    next_sdk
  USE_CUSTOM_ECAL_DOMAIN
)

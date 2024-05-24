next_create_test(
  NAME ethernet_package_test
  SRCS
  ethernet_package_test.cpp
  PRIVATEINCLUDEDIRS
  ${UDEX_SRC_DIR}
  ${PLAYER_SRC_DIR}
  PRIVATELINKLIBS
  next_udex
  INSTALLTARGETS
  mts_core_lib::modules
  mts_sentinel::client
  USE_CUSTOM_ECAL_DOMAIN
)

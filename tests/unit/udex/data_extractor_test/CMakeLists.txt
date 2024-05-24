next_create_test(
  NAME data_extractor_test
  SRCS data_extractor_test.cpp
  RESOURCES
    ${CMAKE_CURRENT_LIST_DIR}/ARS5xx.sdl
    ${CMAKE_CURRENT_LIST_DIR}/ARS5xx_RTMLF.sdl
    ${CMAKE_CURRENT_LIST_DIR}/FBX4_35up_KW25_13_V5_ETHERNET1_V2_lokal_withoutETS.xml
    ${CMAKE_CURRENT_LIST_DIR}/arrays.sdl
    ${CMAKE_CURRENT_LIST_DIR}/Vehicle_CAN2_Public.dbc
    ${CMAKE_CURRENT_LIST_DIR}/Fibex_Q3_2013.xml
    ${CMAKE_CURRENT_LIST_DIR}/ARS5xx_big_endian.sdl
  PRIVATEINCLUDEDIRS
    ${UDEX_SRC_DIR}/external_api/
  ${PLAYER_SRC_DIR}
  PRIVATELINKLIBS
    next_udex
    Boost::boost
  INSTALLTARGETS
    mts_core_lib::modules
    mts_sentinel::client
  USE_CUSTOM_ECAL_DOMAIN
)

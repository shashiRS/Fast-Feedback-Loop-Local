next_create_test(
  NAME register_data_description_test
  SRCS register_data_description_test.cpp
  RESOURCES
    ${CMAKE_CURRENT_LIST_DIR}/test1.sdl
    ${CMAKE_CURRENT_LIST_DIR}/test2.sdl
    ${CMAKE_CURRENT_LIST_DIR}/test3.cdl
    ${CMAKE_CURRENT_LIST_DIR}/test4.dbc
    ${CMAKE_CURRENT_LIST_DIR}/test5.xml
  RESOURCES ${ECAL_INI_FILE}
  PRIVATEINCLUDEDIRS ${UDEX_SRC_DIR}
  PRIVATELINKLIBS
    next_udex
  INSTALLTARGETS
    mts_core_lib::modules
    mts_sentinel::client
  USESDB
)

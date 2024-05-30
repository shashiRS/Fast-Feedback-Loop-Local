next_create_test(
  NAME udex_queue_test
  SRCS 
    udex_queue_test.cpp
  ${UDEX_SRC_DIR}/src/extractor/udex_queue.cpp
    ${UDEX_SRC_DIR}/src/signal_storage/core_lib_wrap_data_converter.cpp
  PRIVATEINCLUDEDIRS
  ${UDEX_SRC_DIR}/src/extractor
  PRIVATELINKLIBS
    next_udex
  INSTALLTARGETS
    mts_core_lib::modules
  USESDB
)

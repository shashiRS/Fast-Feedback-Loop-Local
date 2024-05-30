find_package(gia REQUIRED)

if (MSVC)
  set(gia_binary ${gia_PACKAGE_PATH}/gia_sim/gia_adapter.dll)
else()
  set(gia_binary ${gia_PACKAGE_PATH}/gia_sim/libgia_adapter.so)
endif()

next_create_test(
  NAME gia_test
  SRCS 
    gia_test.cpp
  ${UDEX_SRC_DIR}/src/extractor/gia/gia_dll_handler.cpp
  PRIVATEINCLUDEDIRS
  ${UDEX_SRC_DIR}/src/extractor/gia
  RESOURCES
    ${gia_binary}
  PRIVATELINKLIBS
    next_udex
    Boost::filesystem
  INSTALLTARGETS
    mts_core_lib::modules
    mts_sentinel::client
  USE_CUSTOM_ECAL_DOMAIN
)

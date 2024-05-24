include(${BASE_SRC_DIR}/cmake/version_helper.cmake)

next_create_test(
  NAME testapp
  SRCS
    testapp.cpp
    ${SRC_FILES}
  RESOURCES ${ECAL_INI_FILE}
  PRIVATELINKLIBS
    next_sdk
  PRIVATEINCLUDEDIRS 
    ${PROJECT_BINARY_DIR}
  INSTALLTARGETS
    mts_sentinel::client
)
install_mts_sentinel(
  COMPONENT __CIP_TEST_COMMON
  PATH tests/cases/testapp
)

create_version_info(
  RC_FILE testapp.rc
  VERSION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
  TARGET_NAME testapp
)

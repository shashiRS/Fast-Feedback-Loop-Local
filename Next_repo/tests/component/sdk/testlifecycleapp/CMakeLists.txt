include(${BASE_SRC_DIR}/cmake/version_helper.cmake)

next_create_test(
  NAME testlifecycleapp
  SRCS
  testlifecycleapp.cpp
    ${SRC_FILES}
  RESOURCES ${ECAL_INI_FILE}
  PRIVATELINKLIBS
    next_sdk
    next_appsupport
  PRIVATEINCLUDEDIRS ${PROJECT_BINARY_DIR}
)

create_version_info(
  RC_FILE testapp.rc
  VERSION_FILE ${CMAKE_CURRENT_SOURCE_DIR}/version.h.in
  TARGET_NAME testlifecycleapp
)

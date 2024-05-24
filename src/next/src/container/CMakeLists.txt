set(TARGET_NAME next_container)

set(SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/container_main.cpp)

set(PRIVATE_LINK_LIBS
  next_sdk::next_appsupport
  next_sdk::next_sdk
  Boost::system
  Boost::filesystem
  Boost::date_time # required by filesystem
)

next_create_executable(
  NAME ${TARGET_NAME}
  SRC_FILES ${SRC_FILES}
  USE_VERSION_INFO TRUE
  FOLDER_VIEW Next
  PRIVATE_LINK_LIBS ${PRIVATE_LINK_LIBS}
)

configure_file(config.h.in include/config.h)

########################### Install
include(GNUInstallDirs)

install_runtime(
  TARGET_NAME ${TARGET_NAME}
  COMPONENT NEXT_COMPONENT
)

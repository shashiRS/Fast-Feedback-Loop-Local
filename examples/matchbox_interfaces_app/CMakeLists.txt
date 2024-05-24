include(${CMAKE_SOURCE_DIR}/src/cmake/link_helper.cmake)
include(${CMAKE_SOURCE_DIR}/src/cmake/install_helper.cmake)

set(TARGET_NAME matchbox_example_app)

add_executable(${TARGET_NAME}
  main.cpp
)

link_against_next(TARGET ${TARGET_NAME} FULL)

install(
  TARGETS ${TARGET_NAME}
  COMPONENT NEXT_COMPONENT
  DESTINATION examples/${TARGET_NAME}
)

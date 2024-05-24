include(${CMAKE_SOURCE_DIR}/src/cmake/link_helper.cmake)
include(${CMAKE_SOURCE_DIR}/src/cmake/install_helper.cmake)

set(TARGET_NAME next_example_node)

add_executable(${TARGET_NAME}
  main.cpp
)

link_against_next(TARGET ${TARGET_NAME} FULL)

install(
  TARGETS ${TARGET_NAME}
  COMPONENT NEXT_COMPONENT
  DESTINATION examples/${TARGET_NAME}
)

install(
  FILES ${CMAKE_CURRENT_SOURCE_DIR}/test_signal.sdl
  COMPONENT NEXT_COMPONENT
  DESTINATION examples/${TARGET_NAME}
)

# this does not work from inside the next repo itself, only for external use for now
#next_install(
#  COMPONENT ${TARGET_NAME}
#  BINDIR ${TARGET_NAME}
#)

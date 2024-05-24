set(TEST_PACKAGE control_dummy)

set(TEST_SRC
  control_dummy.cpp
)

#set(TEST_RESOURCES
#  ${CMAKE_CURRENT_LIST_DIR}/file.ext
#)

next_create_test(
  NAME ${TEST_PACKAGE}
  SRCS ${TEST_SRC}
)

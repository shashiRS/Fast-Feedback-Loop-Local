next_create_test(
    NAME DataContainer
    SRCS 
      threadsafe_list_tests.cpp
      threadsafe_queue_tests.cpp 
      threadsafe_set_tests.cpp
    PRIVATEINCLUDEDIRS
      ${BRIDGESDK_INTERFACE_DIR}
)

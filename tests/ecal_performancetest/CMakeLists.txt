set(TARGET_NAME ecal_performancetest)

add_executable(${TARGET_NAME} 
    performancetest.cpp
    common.hpp
    publisher.hpp
    sleep.hpp
    statswriter.hpp
)

target_compile_definitions(${TARGET_NAME}
    PRIVATE
        BOOST_DATE_TIME_NO_LIB
        BOOST_FILESYSTEM_NO_LIB
)

target_link_libraries(${TARGET_NAME} 
    PRIVATE
        eCAL::core
        next_sdk::next_sdk
        Boost::boost
        $<$<CXX_COMPILER_ID:GNU>:rt>
)

if(UNIX)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath=../lib")

  # Newer versions of the linker in some platforms, emit DT_RUNPATH where they used to emit DT_RPATH
  # To replicate the old behaviour on versions of the linker that emit RUNPATH instead, pass the --disable-new-dtags flag to the linker
  target_link_libraries(${TARGET_NAME} PRIVATE "-Wl,--disable-new-dtags")
endif()

########################### Install
include(GNUInstallDirs)
install(
  TARGETS ${TARGET_NAME}
  COMPONENT TARGET_COMPONENT
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
if(CMAKE_HOST_WIN32)
    install(
        DIRECTORY ${eCAL_DIR}/../../../bin
        COMPONENT TARGET_COMPONENT
        DESTINATION .
    )
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:${TARGET_NAME}> $<TARGET_FILE_DIR:${TARGET_NAME}>
        COMMAND_EXPAND_LISTS
    )
else()
endif()

############################ helper for creating test applications
# you have to give at least the name and some sources
# example: next_create_test(NAME myTest SRCS my_test.cpp)
# optionally you can give:
#  PUBLICINCLUDEDIRS      - public include dirs
#  PRIVATEINCLUDEDIRS     - private include dirs
#  PUBLICLINKLIBS         - linking publicly against given libs
#  PRIVATELINKLIBS        - linking privatly against given libs
#  INSTALLTARGETS         - targets which have to be installed (not used for linking)
#  RESOURCES              - resource files
#  PROTOFILES             - Protobuf files
#  PROTOHDRS              - Protobuf target dir
#  USESDB                 - give this option when the mts DB is used
#                           then the test "locks" the DB,
#                           no other test, which is using the DB, will be executed at the same time
#                           (normally bricks executes the tests in parallel, by this we can serialize them when needed)
#  BIGOBJECT              - set the compile options for enabling big object files (Windows only)
#  USE_CUSTOM_ECAL_DOMAIN - give this option when you want to run the test on a distinct eCal Domain
#                           it will prevent issues with the same test running multiple times in parallel, mixing eCal messages
# the function will crate a target named with your given name (in the example "myTest"),
# use this target name to set other things when needed
function(next_create_test)
    set(oneValueArgs NAME TIMEOUT)
    set(multiValueArgs SRCS PUBLICINCLUDEDIRS PRIVATEINCLUDEDIRS PUBLICLINKLIBS PRIVATELINKLIBS INSTALLTARGETS RESOURCES PROTOFILES PROTOHDRS)
    set(options USESDB BIGOBJECT USE_CUSTOM_ECAL_DOMAIN)
    cmake_parse_arguments(TST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT DEFINED TST_NAME)
        message(FATAL_ERROR "there is no name given for the test")
    endif()
    if(NOT DEFINED TST_SRCS)
        message(FATAL_ERROR "no source files given for test ${TST_NAME}")
    endif()
    message("adding test executable ${TST_NAME}")

    add_executable(${TST_NAME}
        ${TST_SRCS}
    )

    target_compile_definitions(${TST_NAME}
        PUBLIC
            BUILD_TESTING
    )

    target_link_libraries(${TST_NAME}
        PUBLIC
            ${TST_PUBLICLINKLIBS}
        PRIVATE
            ${TST_PRIVATELINKLIBS}
    )

    target_include_directories(${TST_NAME}
        PUBLIC
            ${TST_PUBLICINCLUDEDIRS}
        PRIVATE
            ${TST_PRIVATEINCLUDEDIRS}
    )

    if(TST_BIGOBJECT)
        if(MSVC)
            target_compile_options(${TST_NAME} PUBLIC "/bigobj")
        endif()
    endif()

    if (TARGET __CIP_TEST_GENERIC)
        find_package(cip_test_support REQUIRED)
        target_link_libraries(
            ${TST_NAME}
            PUBLIC
                conti::cip_test_support
        )
    endif()

    if (TEST_ENABLED_LOCALLY)
        # If tracing macros are needed
        # then cip_test_support
        find_package(cip_test_support REQUIRED)
        target_link_libraries(
            ${TST_NAME}
            # Continental tracing macros
            # are used so we link to conti::cip_test_support
            PUBLIC
                conti::cip_test_support
        )
    endif()

    add_test(${TST_NAME}
        ${TST_NAME}
    )

    set_tests_properties(
        ${TST_NAME}
        PROPERTIES
            CIP_TEST_TYPE GENERIC
    )

    set_target_properties(
        ${TST_NAME}
        PROPERTIES
            # for any reason it is not possible to use a Visual Studio variable here (e.g. "$(TargetDir)").
            VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:${TST_NAME}>
    )

    if(TST_USESDB)
        set_tests_properties(
            ${TST_NAME}
            PROPERTIES
                RESOURCE_LOCK MTSDBFILE
        )
    endif()

    if(MSVC)
        # if additional libraries are used, copy their DLL files
        # into the build folder (e.g. for debugging)
        add_custom_command(TARGET ${TST_NAME} POST_BUILD
            COMMAND if \"$<TARGET_RUNTIME_DLLS:${TST_NAME}>\" neq \"\" ${CMAKE_COMMAND} -E copy_if_different $<TARGET_RUNTIME_DLLS:${TST_NAME}> $<TARGET_FILE_DIR:${TST_NAME}>
            VERBATIM
            COMMAND_EXPAND_LISTS
        )
        # copying the files into the package folder
        install(
            FILES $<TARGET_RUNTIME_DLLS:${TST_NAME}>
            COMPONENT __CIP_TEST_COMMON
            DESTINATION tests/cases/${TST_NAME} # is there a variable for this?
        )

        string(FIND ${CMAKE_CURRENT_LIST_DIR} "tests/unit/" dir_str_start)
        if(${dir_str_start} EQUAL -1)
            string(FIND ${CMAKE_CURRENT_LIST_DIR} "tests/component/" dir_str_start)
            if(${dir_str_start} EQUAL -1)
                message(FATAL_ERROR "Unable to determine path of the test, is it a unit or component test (in folder 'tests/unit/<yourTest>' or 'tests/component/<yourTest>')?")
            endif()
        endif()
        string(SUBSTRING ${CMAKE_CURRENT_LIST_DIR} ${dir_str_start} -1 vs_path)
        set_target_properties(${TST_NAME} PROPERTIES FOLDER ${vs_path})
    else()
        set_tests_properties(${TST_NAME} PROPERTIES
            ENVIRONMENT "LD_LIBRARY_PATH=../lib"
        )
    endif()

    foreach(tgt ${TST_INSTALLTARGETS})
        if (NOT TARGET ${tgt})
             message ("Target : ${tgt} does not exist; ignored")
             continue()
        endif()
        get_target_property(target_type ${tgt} TYPE)
        if(target_type STREQUAL "EXECUTABLE")
            # copy into the build folder (e.g. for debugging)
            add_custom_command(TARGET ${TST_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${tgt}> $<TARGET_FILE_DIR:${TST_NAME}>
            )
            # installing into package folder
            install(IMPORTED_RUNTIME_ARTIFACTS ${tgt} COMPONENT __CIP_TEST_COMMON DESTINATION tests/cases/${TST_NAME})
        elseif(target_type STREQUAL "SHARED_LIBRARY")
            if(MSVC)
                add_custom_command(TARGET ${TST_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${tgt}> $<TARGET_FILE_DIR:${TST_NAME}>
                )
                install(IMPORTED_RUNTIME_ARTIFACTS ${tgt} COMPONENT __CIP_TEST_COMMON DESTINATION tests/cases/${TST_NAME})
            else()
                install(IMPORTED_RUNTIME_ARTIFACTS ${tgt} COMPONENT __CIP_TEST_COMMON DESTINATION tests/cases/lib)
            endif()
        else()
            get_target_property(libs ${tgt} INTERFACE_LINK_LIBRARIES)
            foreach(lib ${libs})
                get_target_property(target_type ${lib} TYPE)
                if(target_type STREQUAL "SHARED_LIBRARY")
                    if(MSVC)
                        # copy into the build folder (e.g. for debugging)
                        add_custom_command(TARGET ${TST_NAME} POST_BUILD
                            COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${lib}> $<TARGET_FILE_DIR:${TST_NAME}>
                        )
                        # installing into package folder
                        install(IMPORTED_RUNTIME_ARTIFACTS ${lib} COMPONENT __CIP_TEST_COMMON DESTINATION tests/cases/${TST_NAME})
                    else()
                        # copying into build folder is not needed, this is handled by the RPATH
                        #   if it should be needed in future, try doing something like this:
                        #     https://github-am.geo.conti.de/ADAS/next_sdk/blob/v0.10.600-rc3/src/cmake/testhelper.cmake#L153
                        #   drawback on this solution: If two targets finishes at the same time, two rsync commands will try to write the same file
                        #   solution: do not copy into common lib folder
                        # installing into package folder
                        install(IMPORTED_RUNTIME_ARTIFACTS ${lib} COMPONENT __CIP_TEST_COMMON DESTINATION tests/cases/lib)
                    endif()
                endif()
            endforeach()
        endif()
    endforeach()

    if(TST_RESOURCES)
        add_custom_command(TARGET ${TST_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${TST_RESOURCES} $<TARGET_FILE_DIR:${TST_NAME}>
        )

        install(
            FILES ${TST_RESOURCES}
            COMPONENT __CIP_TEST_COMMON
            DESTINATION tests/cases/${TST_NAME}
        )
    endif()

    if(TST_PROTOFILES AND TST_PROTOHDRS)
        PROTOBUF_TARGET_CPP(${TST_NAME} ${TST_PROTOHDRS} ${TST_PROTOFILES})
        target_link_libraries(${TST_NAME}
            PRIVATE
                protobuf::libprotobuf
        )

    endif()

    if(TST_TIMEOUT)
        set_tests_properties(${TST_NAME} PROPERTIES TIMEOUT ${TST_TIMEOUT})
    endif()

    if(TST_USE_CUSTOM_ECAL_DOMAIN)
        string(TIMESTAMP DISTINCT_ECAL_DOMAIN_SUFFIX "%Y%m%d%H%M%S%f")
        cmake_host_system_information(RESULT TEST_HOST_NAME QUERY HOSTNAME)
        string(CONCAT DISTINCT_ECAL_DOMAIN_SUFFIX ${DISTINCT_ECAL_DOMAIN_SUFFIX} ${TST_NAME} ${TEST_HOST_NAME})
        configure_file(${ECAL_INI_TEMPLATE} "${CMAKE_CURRENT_BINARY_DIR}/${TST_NAME}.ecal.ini")
        install(
            FILES ${CMAKE_CURRENT_BINARY_DIR}/${TST_NAME}.ecal.ini
            COMPONENT __CIP_TEST_COMMON
            DESTINATION tests/cases/${TST_NAME}
            RENAME ecal.ini
        )
        if(MSVC)
            # copy into the build folder (e.g. for debugging)
            add_custom_command(TARGET ${TST_NAME} POST_BUILD
               COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_BINARY_DIR}/${TST_NAME}.ecal.ini $<TARGET_FILE_DIR:${TST_NAME}>/ecal.ini)
        endif()
    endif()
endfunction()
###########################

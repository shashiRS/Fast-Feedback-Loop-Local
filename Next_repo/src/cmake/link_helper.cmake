# Link against Next(and udex) and generate versioning information for the target, also set the RPATH
#
# Mandatory arguments:
#  TARGET - name of the target, used for the install commands
# Options:
#  FULL - links also to next_udex
# example:
#  to link against Next, for the target "MY_TARGET":
#    link_against_next(TARGET MY_TARGET)
#  to link against Next and Udex, for the target "MY_TARGET":
#    link_against_next(TARGET MY_TARGET FULL)

function(link_against_next)
    set(options FULL DISABLE_VERSION_INFO)
    set(oneValueArgs TARGET)
    set(multiValueArgs "")
    cmake_parse_arguments(NXTINST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if(NOT DEFINED NXTINST_TARGET)
        message(FATAL_ERROR "The name of the target for which to link Next must be given.")
    endif()

    message("========= NXTINST_TARGET =========")
    message(${NXTINST_TARGET})

    #link
    if(NOT DEFINED NXTINST_FULL)
        target_link_libraries(${NXTINST_TARGET} 
        PRIVATE
            next
        )
    else()
        if (next_FOUND)
            #if this is called from outside Next repo
            target_link_libraries(${NXTINST_TARGET} 
            PRIVATE
                next::next
                next::next_udex
            )
        else()
            target_link_libraries(${NXTINST_TARGET} 
            PRIVATE
                next
                next_udex::next_udex
            )
        endif() 
    endif()
    
    #rpath - check with >ldd binary
    set_target_properties(${NXTINST_TARGET}
        PROPERTIES INSTALL_RPATH "$\{ORIGIN\};$\{ORIGIN\}/../lib"
    )

    if(NOT DEFINED NXTINST_DISABLE_VERSION_INFO)
        create_version_info(
        TARGET_NAME ${NXTINST_TARGET}
        )
    endif()
    
endfunction()

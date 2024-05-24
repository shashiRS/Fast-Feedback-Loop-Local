#######################################################################
#
# This is wrapped version of install
#
# The reason for this file is that CMake support neither function call
# by name nor wrapping same function multiple times
#
#######################################################################


#######################################################################
#
# Generated wrapper function for install which calls
# pre and post wrappers when needed
#
#######################################################################
macro(install)

    # ========== DO WRAPPING =========
    if (
        (BRICKS_DISABLE_FUNCTION_DECORATING)
        OR
        (CIP_DISABLE_FUNCTION_DECORATING)
    )
        # if decorating is disabled we simply
        # call original function
        _install(${ARGV})
    else()
        # call prehooks, original function
        # and post-hooks using ARGV
        #
        # Please note that prehooks can
        # inject arguments to call by setting
        # the BRICKS_INJECTED_ARGV variable
        # which contains list of arguments instead
        # of ARGV

        # unset parameter injection to avoid
        # sneaky bugs or accidentally setting
        # the parameter override
        if (DEFINED BRICKS_INJECTED_ARGV)
            unset(BRICKS_INJECTED_ARGV)
        endif()

        # decorated version of my_function
        # ** CALL PREHOOKS ***
       if (DEFINED BRICKS_INJECTED_ARGV)
           __bricks_install_automatically_inject_export_name(${BRICKS_INJECTED_ARGV})
       else()
           __bricks_install_automatically_inject_export_name(${ARGV})
       endif()
        # ** PREHOOKS DONE ***

        # if some pre-handled has injected
        # new arguments explicitly with BRICKS_INJECTED_ARGV
        # then they should be passed to to
        if (DEFINED BRICKS_INJECTED_ARGV)
            _install(${BRICKS_INJECTED_ARGV})
        else()
            # call original function with original
            # values (_ at front means original function)
            _install(${ARGV})
        endif()

        # ** CALL POSTHOOKS ***
       if (BRICKS_INJECTED_ARGV)
           __bricks_install_add_dependencies_with_components(${BRICKS_INJECTED_ARGV})
       else()
           __bricks_install_add_dependencies_with_components(${ARGV})
       endif()
       if (BRICKS_INJECTED_ARGV)
           __bricks_install_handle_export_names_with_components(${BRICKS_INJECTED_ARGV})
       else()
           __bricks_install_handle_export_names_with_components(${ARGV})
       endif()
        # ** POSTHOOKS DONE ***

        # unset bricks handling so
        # we are not modifying the original functions
        # too much
        if (DEFINED BRICKS_INJECTED_ARGV)
            unset(BRICKS_INJECTED_ARGV)
        endif()

    endif()

endmacro()
# end of decorate enabled function

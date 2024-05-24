/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     get_def_std.hpp
 *  @brief    Get the by default used C++ standard of this repository.
 */

#include "def_cxx_std.hpp"

#ifdef static_assert
#error "static assert is defined in C++17, should not be available"
#endif

long getDefStd() { return __cplusplus; }

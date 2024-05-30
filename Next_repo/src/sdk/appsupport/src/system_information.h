/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     system_information.h
 * @brief    Collection of information about the setup that next is running on
 *
 *
 */

#ifndef SYSTEM_INFORMATION_H_
#define SYSTEM_INFORMATION_H_
#include <string>

namespace next {
namespace appsupport {
namespace system {

// Set Platform
#ifdef __linux__
constexpr const static std::string platform = "Linux";
#elif _WIN32
constexpr const static std::string platform = "Windows";
}
}
}

#endif // !SYSTEM_INFORMATION_H_

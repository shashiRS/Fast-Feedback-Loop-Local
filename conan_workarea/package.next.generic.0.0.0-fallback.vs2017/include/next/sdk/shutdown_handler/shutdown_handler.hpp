/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     shutdown_handler.hpp
 * @brief    handling shutdown requests
 */

#ifndef NEXTSDK_SHUTDOWN_HANDLER_H_
#define NEXTSDK_SHUTDOWN_HANDLER_H_

#include <functional>

#include "../sdk_config.hpp"

namespace next {
namespace sdk {
namespace shutdown_handler {

typedef std::function<void(void)> shutdown_callback;

//! whether a shutdown was requested
/**
 * @return returns true if a shutdown was requested (e.g. CTRL+C was pressed), otherwise false
 */
bool DECLSPEC_nextsdk shutdown_requested();

//! Registering the shutdown handler
/**
 * This function must be called once within the main application.
 * Afterwards via shutdown_requested() can be checked whether a shutdown was requested.
 *
 * On Windows this function will handle CTRL+C in the console application. Also signals of system shutdown, user
 * logoff and application close events are handled.
 * On Linux SIGINT (e.g. CTRL+C) and SIGHUB (console is closed) are handled.
 *
 * After a shutdown request was received, the main application has two seconds to close everything.
 * If several events are received in a row (e.g. the user presses CTRL+C several times) the first event will cause
 * that the callback is called (if registered) and the internal variable will be set to true, shutdown_requested()
 * will return true afterwards. Within the next three seconds the events are ignored. Getting after three seconds
 * again such an event the application will be closed by calling exit(EXIT_FAILURE).
 */
void DECLSPEC_nextsdk register_shutdown_handler();

//! Register the shutdown handler and a callback function which is called in case of shutdown request.
/**
 * See register_shutdown_handler(). The given callback will be called in case of shutdown request.
 * Give nullptr to unregister a previously registered callback.
 */
void DECLSPEC_nextsdk register_shutdown_handler(shutdown_callback f);

} // namespace shutdown_handler
} // namespace sdk
} // namespace next

#endif // NEXTSDK_SHUTDOWN_HANDLER_H_

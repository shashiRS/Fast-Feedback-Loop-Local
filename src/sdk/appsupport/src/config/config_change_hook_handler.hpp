/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     config_change_hook_handler.hpp
 * @brief    class for adding function hooks if config value(s) changed
 *
 *
 */

#ifndef CONFIG_CHANGE_HOOK_HANDLER_H_
#define CONFIG_CHANGE_HOOK_HANDLER_H_

#include <functional>
#include <string>
#include <unordered_map>

#include <next/appsupport/config/config_change_hook_collection.hpp>
#include <next/appsupport/config/config_types.hpp>

namespace next {
namespace appsupport {

/*!
 * @brief  class to hold change hooks which can be called in the the config system
 */

class ConfigChangeHookHandler {
public:
  ConfigChangeHookHandler();
  virtual ~ConfigChangeHookHandler();

public: // public interface
  //! adds the callback function which would be invoked upon each write operation in the config tree
  /*!
   * @param hook callable object which would be called
   */

  size_t addChangeHook(configtypes::change_hook hook);

  //! removes the callback function which would be invoked upon each write operation in the config tree
  /*!
   * @param hook callable object which would be removed
   */
  void removeChangeHook(size_t id);

  //! calls callback functions which were added via addChangeHook
  /*!
   * @param key parameter which is handed over as input for the function calls
   *
   * @return  boolean  true: change hooks could be called.
   *                   false: calling of change hooks failed (Hint: doesn't consider return value of the change hooks)
   */
  bool callChangeHooks(std::string key);

  //! returns if calling of the callback functions is active
  /*!
   * @return  boolean  true: change hooks are called
   *                   false: calling of change hooks deactivated
   */
  bool getChangeHooksActivationStatus();

  //! set if calling of the callback functions is active
  /*!
   * @param change_hook_activation_status  true: change hooks are called
   *                                       false: change hooks are deactivated
   */
  void setChangeHooksActivationStatus(bool change_hook_activation_status);

private:
  uint64_t getCurrentNanoSeconds();

  bool change_hooks_activated_ = true;
  std::unordered_map<size_t, configtypes::change_hook> hash_map_for_hooks_;
};

} // namespace appsupport
} // namespace next

#endif // CONFIG_CHANGE_HOOK_HANDLER_H_

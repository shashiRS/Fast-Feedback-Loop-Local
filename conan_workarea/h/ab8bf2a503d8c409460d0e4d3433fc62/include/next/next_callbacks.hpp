/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     next_callbacks.hpp
 * @brief    Provide all overwritable functions for the Next Component Interface
 *
 */

#ifndef NEXT_CALLBACKS_H_
#define NEXT_CALLBACKS_H_

#include <string>

#include "next_config.h"

namespace next {

class DECLSPEC_next NextCallbacks {
public:
  NextCallbacks() = delete;
  NextCallbacks(const std::string &component_name);
  virtual ~NextCallbacks();

protected:
  /*!
   * @brief Hook that gets called whenever the given config key is changed
   *
   * @param changed_config_key key in config that will be checked for changed value
   * @return true
   * @return false
   */
  virtual bool onConfigChanged(std::string changed_config_key);
};

} // namespace next

#endif

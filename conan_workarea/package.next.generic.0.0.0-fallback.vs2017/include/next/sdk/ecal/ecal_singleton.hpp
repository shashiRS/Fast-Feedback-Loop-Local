/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     ecal_singleton.hpp
 * @brief    eCAL singleton, used for initalization and finalizing eCAL
 */

#ifndef NEXTSDK_ECAL_SINGLETON_H_
#define NEXTSDK_ECAL_SINGLETON_H_

#include "../init_chain/simple_init_chain.hpp"
#include "../sdk_config.hpp"

namespace next {
namespace sdk {

class DECLSPEC_nextsdk EcalSingleton {
public:
  static bool do_init(const InitChain::ConfigMap &configMap, int level = 0);
  static void do_reset(const InitChain::ConfigMap &configMap = InitChain::ConfigMap(), int level = 0);

  static EcalSingleton &get_instance();

  EcalSingleton(const EcalSingleton &) = delete;
  void operator=(const EcalSingleton &) = delete;

  /**
   * @brief get the config key for the instance name for the init chain
   */
  static const char *getInstancenameCfgKey();

  /**
   * @brief register the singleton for initialization
   */
  static void register_to_init_chain();

protected:
  EcalSingleton();
  virtual ~EcalSingleton();

  //! check whether the internal object is created
  static bool init_done();

  //! will set the internal pointer to nullptr, the caller is responsible to clean up (delete the object)
  //! mainly for unit testing (deleting the singleton between two tests, but within the same application)
  static void clear();

private:
  static bool init_done_;

  static EcalSingleton *self_;
};

} // namespace sdk
} // namespace next

#endif // NEXTSDK_ECAL_SINGLETON_H_

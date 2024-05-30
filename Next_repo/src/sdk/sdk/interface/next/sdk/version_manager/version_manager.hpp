/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     version_manager.hpp
 * @brief    declaration of version manager function
 */

#ifndef NEXTSDK_VERSION_MANAGER_H_
#define NEXTSDK_VERSION_MANAGER_H_

#include "../init_chain/simple_init_chain.hpp"
#include "../sdk_config.hpp"

namespace next {
namespace sdk {
namespace version_manager {

enum class ComponentName : std::int8_t {
  NEXT_CONTROL_BRIDGE,
  NEXT_DATA_BRIDGE,
  NEXT_PLAYER,
  NEXT_UDEX,
  NEXT_CONTROL,
  NEXT_SDK,
  NEXT
};

/**
 * @brief return the name of the component Player
 *
 */
inline std::string getPlayerName() { return "next_player"; }

/**
 * @brief return the name of the component Databridge
 *
 */
inline std::string getDatabridgeName() { return "next_databridge"; }

/**
 * @brief return the name of the component Controlbridge
 *
 */
inline std::string getControlbridgeName() { return "next_controlbridge"; }

/**
 * @brief initialize the version_manager
 *
 */
bool DECLSPEC_nextsdk init(const InitChain::ConfigMap &configMap, int = 0);

/**
 * @brief close the version_manager
 *
 */
void DECLSPEC_nextsdk close(const InitChain::ConfigMap & = InitChain::ConfigMap(), int = 0);

/**
 * @brief get the config key for the name of the node providing it's version
 *
 * As values [const] char* and [const] std::string are allowed. Otherwise the init function will fail.
 */
const char DECLSPEC_nextsdk *getVersionManagerComponentNameCfgKey();

/**
 * @brief get the config key for the documentation link of the node
 *
 * As values [const] char* and [const] std::string are allowed. Otherwise the init function will fail.
 */
const char DECLSPEC_nextsdk *getVersionManagerDocumentationCfgKey();

/**
 * @brief get the config key for the version of the node
 *
 * As values [const] char* and [const] std::string are allowed. Otherwise the init function will fail.
 */
const char DECLSPEC_nextsdk *getVersionManagerVersionStringCfgKey();

/**
 * @brief register the version_manager for initialization chain
 */
void DECLSPEC_nextsdk register_to_init_chain();

/**
 * @brief generates VersionInfo string from major and minor versions
 */
std::string DECLSPEC_nextsdk MakeVersionString(const wchar_t *);

/**
 * @brief generates documentation link from predefined inputs as well as also from version details
 */
std::string DECLSPEC_nextsdk CreateDocumentationLink(ComponentName component_name);
} // namespace version_manager
} // namespace sdk
} // namespace next

#endif // NEXTSDK_VERSION_MANAGER_H_

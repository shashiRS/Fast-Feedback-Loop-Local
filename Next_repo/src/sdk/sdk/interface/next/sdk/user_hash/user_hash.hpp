/* ========================== NEXT Project ==================================
 * Copyright(C) A.D.C.Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     user_hash.hpp
 * @brief    class for generating an hash based on the user name and the mac address
 *
 **/
#ifndef NEXT_USER_HASH_HPP
#define NEXT_USER_HASH_HPP

#include "../sdk_config.hpp"

#include <string.h>

namespace next {
namespace sdk {

/**
 * UserHash class
 * Used for creating a unique hash based on the user name and the mac address
 */
class DECLSPEC_nextsdk UserHash {
public:
  /**
   * @brief create unique hash based on the user name and the mac address
   *
   * @param hash created hash based on the user name and the mac address. If the return value is false an hash value 0
   * is returned.
   * @return states if the hash creation was successfull.
   */
  static bool CreateHash(size_t &hash);

protected:
  static bool GetLoginName(std::string &user_name);
  static bool GetMacAdress(std::string &mac_address);
  static void GetReadableMacAddress(const unsigned char *mac_adress, unsigned int length_mac_adress,
                                    std::string &mac_address_readable);

  // mac address types are sorted based on their assumed stable existence
  //(e.g. bluetooth is an ethernet adapter but it might get easily deactivated by an user)
  // NetworkAdapterType with a lower index are considered more stable
  enum class NetworkAdapterType {
    ETHERNET_CARD, /**< Adapter type MIB_IF_TYPE_ETHERNET and containing the word "Ethernet" in the description and not
                      the word "Virtual Adapter". */
    WIFI_CARD, /**< Adapter type IF_TYPE_IEEE80211 and containing the word "Wi-Fi" in the description and not the word
                  "Virtual Adapter". */
    ETHERNET_OTHER, /**< Any other adapter of type MIB_IF_TYPE_ETHERNET which is not added to the category
                       ETHERNET_CARD. */
    WIFI_OTHER,     /**< Any other adapter of type IF_TYPE_IEEE80211 which is not added to the category WIFI_CARD. */
    OTHER           /**< Any other adapter which isn't added to the previous catergories. */
  };
};
} // namespace sdk
} // namespace next
#endif // NEXT_USER_HASH_HPP

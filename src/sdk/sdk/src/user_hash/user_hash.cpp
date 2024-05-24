/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     user_hash.cpp
 * @brief    class for generating an hash based on the user name and the mac address
 *
 **/

#ifdef __linux__
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <pwd.h>
#include <sys/ioctl.h>
#include <unistd.h>
#elif _WIN32
#include <Windows.h>
#include <stdio.h>
// don't change include order here
#include <Iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#endif

#include <map>
#include <string>

#include "next/sdk/user_hash/user_hash.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "next/sdk/types/package_hash.hpp"

namespace next {
namespace sdk {

constexpr uint16_t kUserNameMaxCharacter = 256;
constexpr uint16_t kMaxNumberNics = 32;

bool UserHash::CreateHash(size_t &hash) {
  hash = 0;

  std::string user_name;
  if (!GetLoginName(user_name)) {
    return false;
  }

  std::string mac_address;
  if (!GetMacAdress(mac_address)) {
    return false;
  }
  std::string hash_string = user_name + mac_address;

  for (char const &c : hash_string) {
    types::PackageHash::create_hash(hash, (uint64_t)c);
  }
  return true;
}

#ifdef __linux__
bool UserHash::GetLoginName(std::string &user_name) {
  user_name = "dummyUser";
  uid_t uid = geteuid();
  struct passwd *pw = getpwuid(uid);
  if (pw) {
    user_name = pw->pw_name;
  } else {
    return false;
  }
  return true;
}
#elif _WIN32
bool UserHash::GetLoginName(std::string &user_name) {
  user_name = "dummyUser";
  char user_name_char[kUserNameMaxCharacter + 1];
  DWORD username_len = kUserNameMaxCharacter + 1;
  if (!GetUserName(user_name_char, &username_len)) {
    return false;
  }
  user_name = user_name_char;
  return true;
}
#endif

void UserHash::GetReadableMacAddress(const unsigned char *mac_adress, unsigned int length_mac_adress,
                                     std::string &mac_address_readable) {
  mac_address_readable = "";
  for (unsigned int i = 0; i < length_mac_adress; ++i) {
    if (i > 0) {
      mac_address_readable += "-";
    }
    constexpr uint16_t size_array = 3; // add + 1 to the size for null terminator \0 at the end of the string
    std::unique_ptr<char[]> mac_addr(new char[size_array]);
    snprintf(mac_addr.get(), size_array, "%02X", mac_adress[i]);
    mac_address_readable += mac_addr.get();
  }
}

#ifdef __linux__
bool UserHash::GetMacAdress(std::string &mac_address) {
  mac_address = "00-00-00-00-00-00"; // dummy mac address
  // based on: https://stackoverflow.com/questions/1779715/how-to-get-mac-address-of-your-machine-using-a-c-program
  struct ifreq ifr;
  struct ifconf ifc;
  char buf[1024];
  int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  if (sock == -1) {
    return false;
  };

  ifc.ifc_len = sizeof(buf);
  ifc.ifc_buf = buf;
  if (ioctl(sock, SIOCGIFCONF, &ifc) == -1) {
    return false;
  }

  std::map<NetworkAdapterType, std::map<std::string, std::string>> mac_adress_list;
  mac_adress_list[NetworkAdapterType::ETHERNET_CARD] = std::map<std::string, std::string>();
  mac_adress_list[NetworkAdapterType::WIFI_CARD] = std::map<std::string, std::string>();
  mac_adress_list[NetworkAdapterType::OTHER] = std::map<std::string, std::string>();

  struct ifreq *it = ifc.ifc_req;
  const struct ifreq *const end = it + (ifc.ifc_len / (int)sizeof(struct ifreq));

  for (; it != end; ++it) {
    strcpy(ifr.ifr_name, it->ifr_name);
    if (ioctl(sock, SIOCGIFFLAGS, &ifr) == 0) {
      if (!(ifr.ifr_flags & IFF_LOOPBACK)) { // don't count loopback
        if (ioctl(sock, SIOCGIFHWADDR, &ifr) == 0) {
          // check type of hardware defined in <net/if_arp.h>
          if (ifr.ifr_hwaddr.sa_family == ARPHRD_ETHER) {
            // get mac address
            std::string mac_address_readable;
            GetReadableMacAddress((unsigned char *)ifr.ifr_hwaddr.sa_data,
                                  sizeof(ifr.ifr_hwaddr.sa_data) / sizeof(ifr.ifr_hwaddr.sa_data[0]),
                                  mac_address_readable);
            mac_adress_list[NetworkAdapterType::ETHERNET_CARD].insert(
                std::pair<std::string, std::string>(mac_address_readable, ifr.ifr_name));
          } else if (ifr.ifr_hwaddr.sa_family == ARPHRD_IEEE80211) {
            // get mac address
            std::string mac_address_readable;
            GetReadableMacAddress((unsigned char *)ifr.ifr_hwaddr.sa_data,
                                  sizeof(ifr.ifr_hwaddr.sa_data) / sizeof(ifr.ifr_hwaddr.sa_data[0]),
                                  mac_address_readable);
            mac_adress_list[NetworkAdapterType::WIFI_CARD].insert(
                std::pair<std::string, std::string>(mac_address_readable, ifr.ifr_name));
          } else {
            std::string mac_address_readable;
            GetReadableMacAddress((unsigned char *)ifr.ifr_hwaddr.sa_data,
                                  sizeof(ifr.ifr_hwaddr.sa_data) / sizeof(ifr.ifr_hwaddr.sa_data[0]),
                                  mac_address_readable);
            mac_adress_list[NetworkAdapterType::OTHER].insert(
                std::pair<std::string, std::string>(mac_address_readable, ifr.ifr_name));
          }
        }
      }
    } else {
      return false;
    }
  }

  //// iterate over the adapter categories and use the first adapter which is found in a category as mac address
  for (auto [key, value] : mac_adress_list) {
    if (value.size() != 0) {
      mac_address = value.begin()->first;
      (void)key;
      return true;
    }
  }
  //// no adapters found
  return false;
}
#elif _WIN32
bool UserHash::GetMacAdress(std::string &mac_address) {
  // base on: https://www.codeguru.com/network/three-ways-to-get-your-mac-address/

  mac_address = "00-00-00-00-00-00"; // dummy mac address
  // Fetches the MAC address and prints it
  IP_ADAPTER_INFO AdapterInfo[kMaxNumberNics]; // Allocate information for NICs
  DWORD dwBufLen = sizeof(AdapterInfo);        // Save memory size of buffer

  DWORD dwStatus = GetAdaptersInfo( // Call GetAdapterInfo
      AdapterInfo,                  // [out] buffer to receive data
      &dwBufLen);                   // [in] size of receive data buffer
  // Verify return value is valid, no buffer overflow
  if (dwStatus != ERROR_SUCCESS) {
    return false;
  }

  std::map<NetworkAdapterType, std::map<std::string, std::string>> mac_adress_list;
  mac_adress_list[NetworkAdapterType::ETHERNET_CARD] = std::map<std::string, std::string>();
  mac_adress_list[NetworkAdapterType::WIFI_CARD] = std::map<std::string, std::string>();
  mac_adress_list[NetworkAdapterType::ETHERNET_OTHER] = std::map<std::string, std::string>();
  mac_adress_list[NetworkAdapterType::WIFI_OTHER] = std::map<std::string, std::string>();
  mac_adress_list[NetworkAdapterType::OTHER] = std::map<std::string, std::string>();

  PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo; // Contains pointer to current adapter info
  // Hint: Order of adapter is unspecified starting with windows 10, source:
  //       https://learn.microsoft.com/en-us/windows/win32/api/iphlpapi/nf-iphlpapi-getadaptersinfo
  do {
    // get mac address
    std::string mac_address_readable;
    GetReadableMacAddress(pAdapterInfo->Address, pAdapterInfo->AddressLength, mac_address_readable);
    // sort in adapter in adapter categories
    // Hint: Adapter with the same mac address will be inserted only once
    std::string adapter_description;
    adapter_description += pAdapterInfo->Description;
    if (pAdapterInfo->Type == MIB_IF_TYPE_ETHERNET) {
      if (adapter_description.find("Ethernet") != std::string::npos &&
          adapter_description.find("Virtual Adapter") == std::string::npos) {
        mac_adress_list[NetworkAdapterType::ETHERNET_CARD].insert(
            std::pair<std::string, std::string>(mac_address_readable, adapter_description));
      } else {
        mac_adress_list[NetworkAdapterType::ETHERNET_OTHER].insert(
            std::pair<std::string, std::string>(mac_address_readable, adapter_description));
      }
    } else if (pAdapterInfo->Type == IF_TYPE_IEEE80211) {
      if (adapter_description.find("Wi-Fi") != std::string::npos &&
          adapter_description.find("Virtual Adapter") == std::string::npos) {
        mac_adress_list[NetworkAdapterType::WIFI_CARD].insert(
            std::pair<std::string, std::string>(mac_address_readable, adapter_description));
      } else {
        mac_adress_list[NetworkAdapterType::WIFI_OTHER].insert(
            std::pair<std::string, std::string>(mac_address_readable, adapter_description));
      }
    } else {
      mac_adress_list[NetworkAdapterType::OTHER].insert(
          std::pair<std::string, std::string>(mac_address_readable, adapter_description));
    }
    pAdapterInfo = pAdapterInfo->Next; // Progress through linked list
  } while (pAdapterInfo);              // Terminate if last adapter

  // iterate over the adapter categories and use the first adapter which is found in the sorted categories as mac
  // address
  for (auto [key, value] : mac_adress_list) {
    if (value.size() != 0) {
      mac_address = value.begin()->first;
      return true;
    }
  }
  // no adapters found
  return false;
}
#endif
} // namespace sdk
} // namespace next

/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */

#ifndef NEXT_UDEX_URL_HELPER_HPP
#define NEXT_UDEX_URL_HELPER_HPP

namespace next {
namespace udex {

//! get the device name from a group url
/*!
 * @param url url of a group
 * @return return the device name
 */
inline std::string get_device_name(const std::string &url) {
  auto pos = url.find_first_of(".");
  if (pos == std::string::npos) {
    return "";
  }

  return url.substr(0, pos);
}

//! get the view name from a group url
/*!
 * @param url url of a group
 * @return return the view name
 */
inline std::string get_view_name(const std::string &url) {
  auto pos = url.find_first_of(".");
  if (pos == std::string::npos) {
    return "";
  }

  std::string partial_url = url.substr(pos + 1);

  pos = partial_url.find_last_of(".");
  if (pos == std::string::npos) {
    return "";
  }

  return partial_url.substr(0, pos);
}

//! get the group name from a group url
/*!
 * @param url url of a group
 * @return return the group name
 */
inline std::string get_group_name(const std::string &url) {
  auto pos = url.find_last_of(".");
  if (pos == std::string::npos) {
    return "";
  }

  return url.substr(pos + 1);
}

} // namespace udex
} // namespace next

#endif // NEXT_UDEX_URL_HELPER_HPP

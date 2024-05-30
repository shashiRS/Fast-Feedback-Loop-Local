/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     version.h
 * @brief    Class for handling version information.
 */
#ifndef NEXT_DATABRIDGE_VERSION_H_
#define NEXT_DATABRIDGE_VERSION_H_

#include <string>

namespace next {
namespace databridge {
namespace plugin_manager {
// Method to compare two version strings
//   v1 <  v2  -> -1
//   v1 == v2  ->  0
//   v1 >  v2  -> +1

class Version {

public:
  Version(std::string v);
  bool operator<(Version v);
  bool operator>(Version v);
  bool operator<=(Version v);
  bool operator>=(Version v);
  bool operator==(Version v);
  int version_compare(std::string v1, std::string v2);

private:
  std::string version_string;
};

} // namespace plugin_manager
} // namespace databridge
} // namespace next
#endif // NEXT_DATABRIDGE_VERSION_H_

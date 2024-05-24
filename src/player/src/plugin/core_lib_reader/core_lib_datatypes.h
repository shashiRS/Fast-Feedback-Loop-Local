/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
#ifndef NEXT_CORE_LIB_DATATYPES_H
#define NEXT_CORE_LIB_DATATYPES_H

struct CoreLibProcessor {
  std::vector<std::string> publish_urls;
  next::sdk::types::PackageFormatType package_type;
};

#endif // NEXT_CORE_LIB_DATATYPES_H

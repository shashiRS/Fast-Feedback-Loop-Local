/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */

#ifndef NEXT_UDEX_SIGNAL_EXPLORER_GLUE_H
#define NEXT_UDEX_SIGNAL_EXPLORER_GLUE_H

#include "../signal_storage/signal_watcher.h"

namespace next {
namespace udex {
namespace explorer {

class SignalExplorerImpl {
public:
  SignalExplorerImpl();
  virtual ~SignalExplorerImpl();
  std::vector<ChildInfo> GetChildByUrl(std::string parent_url);
  std::vector<std::string> GetPackageUrl(std::string signal_url);
  std::unordered_map<std::string, std::vector<std::string>> GetFullUrlTree();
  SignalDescription getSignalDescription(const std::string &url, bool &available);
  std::pair<bool, std::string> generateSdl(const std::string topic);
  std::vector<std::string> getDeviceByFormatType(const std::string &format);
  std::vector<std::string> searchSignalTree(std::string keyword);
  void EnableFastSearch();

private:
  ecal_util::SignalWatcher &signal_watcher_ = ecal_util::SignalWatcher::GetInstance();
};

} // namespace explorer
} // namespace udex
} // namespace next

#endif // NEXT_UDEX_SIGNAL_EXPLORER_GLUE_H

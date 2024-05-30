#ifndef NEXT_UDEX_GIA_DLL_HANDLER_H
#define NEXT_UDEX_GIA_DLL_HANDLER_H

#include <next/sdk/sdk_macros.h>
#include <next/sdk/logger/logger.hpp>

#include "gia_interfaces.h"
#include "memory_access.hpp"

#include "boost/dll.hpp"
#include "boost/filesystem.hpp"
#include "boost/function.hpp"

class GiaDllHandler {
public:
  GiaDllHandler(){};

  virtual ~GiaDllHandler(){};

  IAdapter *GetAdapter(const std::string &adapter_dll);

  friend class GiaDllHandlerTest;

private:
  std::string RemoveExtension(const std::string &adapter_dll, const std::string &extension);
  std::string HandleFileName(const std::string &adapter_dll);
  void SetMemoryFunctions(IAdapterAccess *adapter_access_ptr);

  boost::function<void *(int)> entry_func_boost_;

  IAdapter *adapter_ptr_ = nullptr;
};

#endif // NEXT_UDEX_GIA_DLL_HANDLER_H

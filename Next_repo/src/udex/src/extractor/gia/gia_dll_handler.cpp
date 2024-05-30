#include "gia_dll_handler.h"

CMemoryAccess CMemoryAccess::instance_;
std::map<void *, unsigned int> CMemoryAccess::pointers_;

std::string GiaDllHandler::RemoveExtension(const std::string &adapter_dll, const std::string &extension) {
  std::string base_string = adapter_dll;

  auto start_position_to_erase = base_string.find(extension);
  base_string.erase(start_position_to_erase, extension.size());

  if (extension == ".so") {
    // linux shared libraries have "lib" prefix
    base_string.erase(0, strlen(kSharedLibFilePrefix));
  }

  return base_string;
}

std::string GiaDllHandler::HandleFileName(const std::string &adapter_dll) {
  std::string file_name;
  auto extension = boost::filesystem::extension(adapter_dll);

  if (extension == "") {
    // no extension, already a base string
    file_name = adapter_dll;
  } else {
    file_name = RemoveExtension(adapter_dll, extension);
  }

  // add current platform extension
  file_name += kSharedLibFileExtension;
#if !defined(_MSC_VER)
  // for linux also add prefix to shared library
  file_name = kSharedLibFilePrefix + file_name;
#endif

  return file_name;
}

IAdapter *GiaDllHandler::GetAdapter(const std::string &adapter_dll) {
  if (adapter_dll.empty()) {
    return nullptr;
  }

  auto file_name = HandleFileName(adapter_dll);

  if (boost::filesystem::extension(file_name) == kSharedLibFileExtension) {
    try {
      boost::dll::shared_library lib(file_name);

      if (!lib.has("CreateAdapterObj")) {
        warn(__FILE__, " Adapter DLL entry function(CreateAdapterObj) not found in: {0}", file_name);
      } else {
        entry_func_boost_ = boost::dll::import_symbol<void *(int)>(boost::move(lib), "CreateAdapterObj");
        if (entry_func_boost_) {
          IAdapterAccess *adapter_access_ptr = reinterpret_cast<IAdapterAccess *>(entry_func_boost_(0));
          if (adapter_access_ptr) {
            SetMemoryFunctions(adapter_access_ptr);
            adapter_ptr_ =
                reinterpret_cast<IAdapter *>(adapter_access_ptr->GetInterface(const_cast<wchar_t *>(L"IAdapter")));
          }
        }
      }
    } catch (std::exception &e) {
      warn(__FILE__, " Adapter DLL could not be loaded: {0} -> {1}", file_name, e.what());
    }
  }
  return adapter_ptr_;
}

void GiaDllHandler::SetMemoryFunctions(IAdapterAccess *adapter_access_ptr) {
  ISetFcPointer *set_fct_pointer_ptr =
      reinterpret_cast<ISetFcPointer *>(adapter_access_ptr->GetInterface(const_cast<wchar_t *>(L"ISetFcPointer")));

  if (set_fct_pointer_ptr) {
    set_fct_pointer_ptr->SetMemFcPointer(CMemoryAccess::AllocMemory, CMemoryAccess::FreeMemory,
                                         CMemoryAccess::LockMemory);
  }
}

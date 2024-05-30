#include "udex_manager.hpp"

#include <set>

#include <boost/filesystem.hpp>
#include <boost/function.hpp>

#ifdef _WIN32
#include <Windows.h>
#include "libloaderapi.h"
#endif
#include <boost/dll/import.hpp>
#include <boost/dll/shared_library.hpp>
#include <boost/dll/shared_library_load_mode.hpp>

#include <next/sdk/sdk.hpp>
#include <next/sdk/types/package_hash.hpp>

#include <next/udex/publisher/package_processor.hpp>

namespace next {
namespace udex {

class UdexManagerImpl {
public:
  UdexManagerImpl(){};
  ~UdexManagerImpl(){};
  std::set<boost::dll::shared_library> loaded_dlls_ = {};
  std::vector<UdexManager::ProcessorInfo> processors_ = {};
  std::vector<UdexManager::ProcessorInfo> new_processors_ = {};
};

UdexManager *udex_manager = nullptr;

UdexManager &UdexManager::GetInstance() {
  if (udex_manager == nullptr) {
    udex_manager = new UdexManager();
  }
  return *udex_manager;
}

void UdexManager::DeleteInstance() {
  if (udex_manager != nullptr) {
    delete udex_manager;
  }
  udex_manager = nullptr;
}
UdexManager::~UdexManager() { Reset(); }

bool UdexManager::Reset() {
  bool success = true;
  std::lock_guard<std::mutex> lock(protect_manager_);
  for (boost::dll::shared_library dll : impl_->loaded_dlls_) {
    try {
      dll.unload();
    } catch (std::exception &e) {
      next::sdk::logger::error(__FILE__, "Failed to unload library {0} with: ", dll.location().string(), e.what());
      success = false;
    }
  }
  impl_.reset();
  impl_ = std::make_unique<UdexManagerImpl>();
  return success;
}

UdexManager::UdexManager() {
  impl_ = std::make_unique<UdexManagerImpl>();
  static next::sdk::InitChain::El init_el(
      next::sdk::InitPriority::SIGNAL_WATCHER, [](const next::sdk::InitChain::ConfigMap &, int) { return true; },
      [this](const next::sdk::InitChain::ConfigMap &, int) { DeleteInstance(); });
}

std::vector<std::shared_ptr<publisher::PackageProcessor>> UdexManager::GetProcessorByHash(const size_t hash) {
  std::lock_guard<std::mutex> lock(protect_manager_);

  std::vector<std::shared_ptr<publisher::PackageProcessor>> result;
  for (auto proc_temp : impl_->processors_) {
    if (std::find(proc_temp.package_hashes.begin(), proc_temp.package_hashes.end(), hash) !=
        proc_temp.package_hashes.end()) {
      result.push_back(proc_temp.processor_instance);
    }
  }
  return result;
}

bool UdexManager::loadProcessorDlls(const std::string &path) {
  bool success = true;
  std::lock_guard<std::mutex> lock(protect_manager_);

  debug(__FILE__, "scanning folder {}", path);
  boost::filesystem::path boostPath = boost::filesystem::system_complete(path);
  if (!boost::filesystem::exists(boostPath) || !boost::filesystem::is_directory(boostPath)) {
    return false;
  }

  for (boost::filesystem::directory_entry &entry : boost::filesystem::directory_iterator(boostPath)) {
    if (!boost::filesystem::is_regular_file(entry)) {
      continue;
    }
    if (boost::filesystem::extension(entry.path()) != kSharedLibFileExtension) {
      continue;
    }
    try {
#ifdef _WIN32
      // This is to prevent error mode dialogs from hanging the application.
      SetErrorMode(0x0001);

      // use windows lib loader flags to fill boost dllLoadMode
      // boost doesn't define them, so use Windows definitions
      // see libloaderapi.h from Windows API
      boost::dll::load_mode::type dllLoadMode =
          boost::dll::load_mode::type(LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
#else
      boost::dll::load_mode::type dllLoadMode = boost::dll::load_mode::default_mode;
#endif

      boost::dll::shared_library lib(entry.path(), dllLoadMode);

#ifdef _WIN32
      // Set Error Mode to default
      SetErrorMode(0);
#endif
      auto abs_path = entry.path().string();

      if (!lib.has(createProcessorMethodName)) {
        debug(__FILE__, " {} is not a compatible reader plugin", abs_path);
        lib.unload();
        continue;
      }

      auto check_for_path = [&abs_path](const ProcessorInfo &info) { return info.path == abs_path; };
      auto find_processor = std::find_if(impl_->processors_.begin(), impl_->processors_.end(), check_for_path);
      if (find_processor != impl_->processors_.end()) {
        debug(__FILE__, " {} is already loaded. Skipped.", abs_path);
        continue;
      }
      typedef processor_creator_t(processor_constructor_)();
      boost::function<processor_constructor_> creator =
          boost::dll::import_symbol<processor_constructor_>(lib, createProcessorMethodName);

      auto newProcessor = (next::udex::publisher::PackageProcessor *)creator();
      auto supportedFileTypesOfNewReader = newProcessor->provideRequestedPackageInfo();

      // ignore plugins with no file type support
      if (supportedFileTypesOfNewReader.size() == 0) {
        warn(__FILE__, "plugin doesn't support any file type. Plugin is ignored.");
        lib.unload();
        continue;
      }
      debug(__FILE__, "  {0} is a PackageProcessor. Loaded into UDexManager.", abs_path);
      // add the DLL to list of loaded DLLs
      // so that it doesn't get unloaded
      impl_->loaded_dlls_.insert(lib);
      // add the Reader to list of loaded readers
      AddProcessorToInfoMap(newProcessor->CreateNewInstance(), abs_path, entry.path().filename().string());
    } catch (const boost::system::system_error &e) {
      warn(__FILE__, "Could not load plugin; Reason [{0}]. Plugin is ignored.", e.what());
      success = false;
      continue;
    }
  }
  return success;
}

void UdexManager::AddProcessorToInfoMap(std::shared_ptr<next::udex::publisher::PackageProcessor> new_processor,
                                        const std::string &path, const std::string &name) {
  ProcessorInfo proc_info;
  proc_info.processor_instance = new_processor;
  proc_info.path = path;
  proc_info.name = name;
  auto meta_infos = new_processor->provideRequestedPackageInfo();
  for (auto &info : meta_infos) {
    size_t hash = next::sdk::types::PackageHash::hashMetaInfo(info);
    proc_info.infos.push_back(info);
    proc_info.package_hashes.push_back(hash);
  }
  impl_->processors_.push_back(proc_info);
  impl_->new_processors_.push_back(proc_info);
}

std::vector<std::shared_ptr<publisher::PackageProcessor>> UdexManager::GetAllProcessors() {
  std::lock_guard<std::mutex> lock(protect_manager_);
  std::vector<std::shared_ptr<publisher::PackageProcessor>> result;
  for (auto &proc_temp : impl_->processors_) {
    result.push_back(proc_temp.processor_instance);
  }
  return result;
}

std::vector<UdexManager::ProcessorInfo> UdexManager::GetNewProcessors() {
  std::lock_guard<std::mutex> lock(protect_manager_);
  std::vector<ProcessorInfo> result;
  for (auto &proc_temp : impl_->new_processors_) {
    result.push_back(proc_temp);
  }
  impl_->new_processors_.clear();
  return result;
}

} // namespace udex
} // namespace next
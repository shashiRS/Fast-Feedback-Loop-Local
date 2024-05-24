/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     argument_parser.hpp
 * @brief    parses the arguments for the container
 */

#ifndef NEXT_CONTAINER_DLL_LOADER_HPP_
#define NEXT_CONTAINER_DLL_LOADER_HPP_

#include <functional>

#include <boost/dll.hpp>
#include <boost/filesystem.hpp>

#include <next/sdk/sdk.hpp>

namespace next {
namespace container {

using MainCall = std::function<int(int, char *[])>;
using NameCall = std::function<const char *(void)>;

struct NextDll {
  std::string name;
  MainCall dll_main_func;
  NameCall dll_name_func;
  std::shared_ptr<boost::dll::shared_library> dll_instance;
};

class DllLoader {
public:
  static bool loadLibrariesInWorkingDirectory(std::vector<NextDll> &dlls) {

    const std::string kMethodNameDllMain = "nextDllMain";
    const std::string kMethodNameDllName = "nextDllName";

    boost::filesystem::path boostPath = boost::filesystem::system_complete("./");
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

        auto dll_entry =
            std::make_shared<boost::dll::shared_library>(entry.path(), boost::dll::load_mode::default_mode);

        if (!dll_entry->has(kMethodNameDllMain) || !dll_entry->has(kMethodNameDllName)) {
          dll_entry->unload();
          next::sdk::logger::debug(
              __FILE__, "DLL found in file {0} failed loading. No matching signature available {1} or {2} missing.",
              entry.path().string(), kMethodNameDllMain, kMethodNameDllName);
          continue;
        }
        NextDll dll_temp;
        dll_temp.dll_name_func = boost::dll::import_symbol<const char *(void)>(*dll_entry.get(), kMethodNameDllName);
        next::sdk::logger::debug(__FILE__, "DLL found in file {0} with module name {1}", entry.path().string(),
                                 dll_temp.dll_name_func());
        dll_temp.name = dll_temp.dll_name_func();
        dll_temp.dll_main_func = boost::dll::import_symbol<int(int, char *[])>(*dll_entry.get(), kMethodNameDllMain);
        dll_temp.dll_instance = dll_entry;
        dlls.push_back(dll_temp);
      } catch (const boost::system::system_error &error) {
        next::sdk::logger::error(__FILE__,
                                 "DLL found in file {0} failed loading. Loading of {1} or {2} failed. Error: {3}",
                                 entry.path().string(), kMethodNameDllMain, kMethodNameDllName, error.what());
        continue;
      }
    }
    return true;
  }

  static bool filterLibraries(std::vector<NextDll> &dlls, std::vector<std::string> node_names = {}) {
    if (node_names.size() == 0) {
      return true;
    }

    std::map<std::string, bool> check_hit;
    for (auto &name : node_names) {
      check_hit[name] = false;
    }
    auto dll_it = dlls.begin();
    while (dll_it != dlls.end()) {
      auto found_name = std::find(node_names.begin(), node_names.end(), dll_it->name);
      if (found_name == node_names.end()) {
        try {
          if (dll_it->dll_instance != nullptr) {
            dll_it->dll_instance->unload();
          }
        } catch (const std::exception &e) {
          next::sdk::logger::debug(__FILE__, "Unload dll {0} failed with {1}", dll_it->name, e.what());
        }
        dll_it = dlls.erase(dll_it);
      } else {
        check_hit[*found_name] = true;
        dll_it++;
      }
    }

    for (auto &hit : check_hit) {
      if (hit.second == false) {
        return false;
      }
    }
    return true;
  }
};

} // namespace container
} // namespace next

#endif // NEXT_CONTAINER_DLL_LOADER_HPP_

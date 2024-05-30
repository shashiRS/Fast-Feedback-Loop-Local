/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     reader_manager.cpp
 * @brief    Manages the concrete reader instance selection to be used by the recording file reader.
 *
 **/

#include "reader_manager.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/function.hpp>

#include <next/sdk/logger/logger.hpp>

#ifdef _WIN32
#include <Windows.h>
#include "libloaderapi.h"
#endif

// define the C constructor definition to load the reader libraries
typedef reader_plugin_create_t(reader_plugin_constructor_t)();

namespace next {
namespace player {

ReaderManager::ReaderManager() {
  // private constructor

  mts_core_lib_reader_ = new next::player::plugin::corelibreader::NextCoreLibReader();
  auto supportedFileTypesOfCoreLib = mts_core_lib_reader_->GetSupportedFileTypes();
  supported_file_types.insert(supportedFileTypesOfCoreLib.begin(), supportedFileTypesOfCoreLib.end());

  // load all libraries from reader plugin dir "plugins/reader/"
  ScanDirectoryForReaderPlugins("plugins/reader", 3);
}

ReaderManager *ReaderManager::GetInstance() {
  static ReaderManager rm;

  return &rm;
}

std::shared_ptr<next::player::plugin::ReaderInterface>
ReaderManager::GetReaderByFilePath(const std::string &file_path) {
  size_t last_of_point = file_path.find_last_of(".");
  if (std::string::npos == last_of_point) {
    // file doesn't have a file ending
    error(__FILE__,
          "Given recording filePath '" + file_path + "' doesn't have a file ending. No file reader has been loaded.");
    return nullptr;
  }
  std::string file_extension = file_path.substr(last_of_point + 1);
  boost::algorithm::to_lower(file_extension);

  return GetReaderByFileExtension(file_extension);
}

std::shared_ptr<next::player::plugin::ReaderInterface>
ReaderManager::GetReaderByFileExtension(std::string fileExtension) {
  if (mts_core_lib_reader_ != nullptr) {
    std::set supported_file_types_of_core_lib = mts_core_lib_reader_->GetSupportedFileTypes();
    if (supported_file_types_of_core_lib.find(fileExtension) != supported_file_types_of_core_lib.end()) {
      // this ReaderInterface supports the requested file type

      // return new instance of this reader as shared pointer
      return std::make_shared<next::player::plugin::corelibreader::NextCoreLibReader>();
    }
  }

  // iterate over all reader plugins to find a matching one
  for (next::player::plugin::ReaderInterface *readerPlugin : this->readersFromPluginDir) {
    std::set supported_file_types_of_plugin = readerPlugin->GetSupportedFileTypes();
    if (supported_file_types_of_plugin.find(fileExtension) != supported_file_types_of_plugin.end()) {
      // this ReaderInterface supports the requested file type

      // return new instance of the reader as shared pointer
      return readerPlugin->CreateNewInstance();
    }
  }

  // no reader interface found
  warn(__FILE__, "Recording with file extension '" + fileExtension + "' is not supported.");
  return nullptr;
}

void ReaderManager::ScanDirectoryForReaderPlugins(const std::string &path, int depth) {
  if (-1 > depth)
    return;
  debug(__FILE__, "scanning folder {}", path);
  boost::filesystem::path boostPath = boost::filesystem::system_complete(path);
  if (boost::filesystem::exists(boostPath) && boost::filesystem::is_directory(boostPath)) {
    for (boost::filesystem::directory_entry &entry : boost::filesystem::directory_iterator(boostPath)) {
      debug(__FILE__, "checking {}", entry.path().string());
      if (boost::filesystem::is_regular_file(entry)) {
        if (boost::filesystem::extension(entry.path()) == kSharedLibFileExtension) {
          debug(__FILE__, "found library file: {}", entry.path().string());
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
            if (!lib.has(createMethodNameInReaderLibraries)) {
              debug(__FILE__, " {} is not a compatible reader plugin", entry.path().string());
              lib.unload();
              continue;
            } else {
              boost::function<reader_plugin_constructor_t> creator =
                  boost::dll::import_symbol<reader_plugin_constructor_t>(lib, createMethodNameInReaderLibraries);

              auto newReaderPlugin = (next::player::plugin::ReaderInterface *)creator();
              auto supportedFileTypesOfNewReader = newReaderPlugin->GetSupportedFileTypes();

              // ignore plugins with no file type support
              if (supportedFileTypesOfNewReader.size() == 0) {
                warn(__FILE__, "plugin doesn't support any file type. Plugin is ignored.");
                lib.unload();
                continue;
              }

              // check if this reader only supports new file types
              bool pluginSupportsAlreadySupportedFileTypes = false;
              for (auto fileType : supportedFileTypesOfNewReader) {
                if (supported_file_types.find(fileType) != supported_file_types.end()) {
                  // we found file type which already is supported by another plugin
                  warn(__FILE__, "plugin supports already supported file type '{0}'. Plugin is ignored.", fileType);
                  lib.unload();
                  pluginSupportsAlreadySupportedFileTypes = true;
                  break;
                }
              }
              if (pluginSupportsAlreadySupportedFileTypes) {
                // continue with next library if available
                continue;
              }

              debug(__FILE__, "  {0} seems to be a compatible reader plugin", entry.path().string());
              auto abs_path = boost::filesystem::absolute(boostPath).string();
              debug(__FILE__, "  absolute path: {}", abs_path);

              // add the DLL to list of loaded DLLs
              // so that it doesn't get unloaded
              loaded_dlls.insert(lib);

              // add the Reader to list of loaded readers
              readersFromPluginDir.push_back(newReaderPlugin);
              supported_file_types.insert(supportedFileTypesOfNewReader.begin(), supportedFileTypesOfNewReader.end());
            }
          } catch (const boost::system::system_error &e) {
            warn(__FILE__, "Could not load plugin; Reason [{0}]. Plugin is ignored.", e.what());
            continue;
          }
        } else {
          debug(__FILE__, "  ignoring file: {}", entry.path().string());
        }
      } else if (boost::filesystem::is_directory(entry) && 0 <= depth) {
        debug(__FILE__, "  looking into directory", entry.path().string());
        ScanDirectoryForReaderPlugins(entry.path().string(), depth - 1);
      }
    }
  }
}

const std::set<std::string> ReaderManager::GetSupportedFileTypes() { return supported_file_types; }

} // namespace player
} // namespace next

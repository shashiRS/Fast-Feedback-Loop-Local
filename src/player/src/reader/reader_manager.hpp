/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     reader_manager.hpp
 * @brief    Manages the concrete reader instance selection to be used by the recording file reader.
 *
 **/

#ifndef READER_MANAGER_H_
#define READER_MANAGER_H_

#include <boost/dll.hpp>

#include <next/player/plugin/reader_interface.hpp>

#include "../plugin/core_lib_reader/core_lib_reader.h"

namespace next {
namespace player {

class ReaderManager {
public:
  /*! @brief Singleton
   *
   * @return The one and only instance of ReaderManager.
   */
  static ReaderManager *GetInstance();

  /*! @brief Selects the concrete reader instance of given file by analysing the file extension.
   *
   * @return Concrete reader instance of recording type or nullptr in case file type is not supported.
   */
  std::shared_ptr<next::player::plugin::ReaderInterface> GetReaderByFilePath(const std::string &file_path);

  /*! @brief Provides list of supported file types
   *
   * Called e.g. before file open dialog is started to set file filter.
   *
   * @return set of strings (e.g. ["rec","rrec"])
   */
  const std::set<std::string> GetSupportedFileTypes();

private:
  ReaderManager();

  /*! @brief Selects the concrete reader instance of given file extension.
   *
   * @return Concrete reader instance or nullptr in case file extension is not supported.
   */
  std::shared_ptr<next::player::plugin::ReaderInterface> GetReaderByFileExtension(std::string file_extension);

  /*! @brief Scans a directory for DLL/.so files that are reader plugins and adds them to the internal list.
   *
   * @param path directory to search for reader Plugins
   * @param depth the depth how deep the given directory has to be searched into
   */
  void ScanDirectoryForReaderPlugins(const std::string &path, int depth);

  // the one and only singleton instance
  static ReaderManager *instance_;

  // references to all existing readers
  next::player::plugin::ReaderInterface *mts_core_lib_reader_ =
      new next::player::plugin::corelibreader::NextCoreLibReader();
  std::vector<next::player::plugin::ReaderInterface *> readersFromPluginDir = {};

  // the method name in the reader libraries
  const char *createMethodNameInReaderLibraries = "create_reader_plugin";

  // list of supported file types from all readers
  std::set<std::string> supported_file_types = {};

  std::set<boost::dll::shared_library> loaded_dlls = {};
};

} // namespace player
} // namespace next

#endif // READER_MANAGER_H_

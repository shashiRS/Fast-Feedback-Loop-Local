#ifndef NEXT_PLAYER_READER_INTERFACE_H
#define NEXT_PLAYER_READER_INTERFACE_H

#include <functional>
#include <set>
#include <string>

#include <next/sdk/types/data_description_types.hpp>

namespace next {
namespace player {
namespace plugin {

struct FileStats {
  uint64_t start_timestamp;
  uint64_t end_timestamp;
  uint64_t file_size;
  bool supports_random_access;
};

using LoadingProgressCallback = std::function<void(uint64_t file_size, float progress_percent)>;

class ReaderInterface {
public:
  ReaderInterface() = default;
  virtual ~ReaderInterface() = default;

  struct CycleInformation {
    uint32_t id;
    std::string device_name;
    std::string cycle_description;
  };

  /*! @brief Interface being called to check the supported file types.
   * This is mandatory. Readers providing an empty list will not be used.
   * Only one reader per File type is allowed to prevent people implementing their own reader for an already supported
   * recording type.
   *
   * @return List of file suffixes that are supported by this particular reader. e.g. ["rec","rrec","zrec"]
   */
  virtual std::set<std::string> GetSupportedFileTypes() = 0;

  /*! @brief Interface being called for a reset. Remove all data from previously loaded recordings
   *
   * @return true if successful. False value will result in a shutdown
   */
  virtual bool Reset() = 0;

  /*! @brief Interface being called for new packages after file was read and descriptions were being passed
   *
   * function gets called repetitive during the runtime. Most useful is a single package return.
   *
   * @return provide latest packages from read
   */
  virtual std::vector<std::unique_ptr<next::sdk::types::IPackage>> GetPackages() = 0;

  /*! @brief Interface being called whenever a new file is requested.
   *
   * Implement your file opening and preparation here. Afterwards
   * * getCurrentDataDescriptions
   * * getFileStats
   * are getting called.
   * Using file_paths with multiple files is reserved for NEXT internal use. External file readers
   * will be called with only one file.
   *
   * @param[in] file_paths requested file paths
   * @param[in] loading_callback callback to provide loading status (progress of current file opening)
   *
   * @return true if success
   */
  virtual bool OpenFile(const std::vector<std::string> &file_paths, const LoadingProgressCallback loading_callback) = 0;

  /*! @brief Interface function being called for File stats after opening a file
   *
   * @return FileStats
   */
  virtual FileStats GetFileStats() = 0;

  /*! @brief Interface being called to request current data description from the previously set files
   * Return the current DataDescription
   * Supported file formats SDL, FIBEX.
   * @return List of DataDescriptions
   */
  virtual std::vector<next::sdk::types::DataDescription> GetCurrentDataDescriptions() = 0;

  /*! @brief Interface gets called with all the resulting package names created by the DataDescriptions
   *
   * In case the reader is running in parallel to other readers (recording merge mode)
   * the package_name_list will also contain the package names of the other readers.
   *
   * @param[in] package_name_list already registered package names
   * @param[in,out] additional_binary_package_names
   * @return true if success
   */
  virtual bool SetCurrentPackageNames(const std::vector<std::string> &package_name_list,
                                      std::vector<std::string> &additional_binary_package_names) = 0;

  /*! @brief Interface being call to get the current Timestamp in the fileread
   *
   * @return current timestamp from the filereader
   */
  virtual uint64_t GetCurrentTimestamp() = 0;

  /*! @brief check if the file was read completely
   *
   * @return return true if file is at end
   */
  virtual bool CheckEndOfFile() = 0;

  /*! @brief Jump to the requested target timestamp.
   * Any reader must support jumping to a timestamp before the own start timestamp.
   * Then it must automatically jump to first position.
   * The reader must also support jumping to a timestamp after the own end timestamp.
   * Then EOF is reached but method should return true.
   *
   * @param target_timestamp
   * @return true if jump was possible, false e.g. in case of file read error etc.
   */
  virtual bool JumpToTimestamp(uint64_t target_timestamp) = 0;

  /*! @brief Creates new instance of this reader
   *
   * Called in case another instance of this reader is required (e.g. in case multiple recordings should be read in
   * parallel).
   *
   * @return pointer to new instance of the reader
   */
  virtual std::shared_ptr<ReaderInterface> CreateNewInstance() = 0;

  /*! @brief set filter expression to filter data URLs from being processed.
   *
   * This can be used to minimize the data packages to be sent.
   * With this the file reader could reduce processing time and
   * increase the overall runtime performance.
   *
   * @param filter_expression
   * @return true if successfull
   */
  virtual bool SetFilter(std::string filter_expression) = 0;

  /*! @brief describes if setting filter expressions are supported by the file reader.
   *
   * If you are unsure how filter expressions are used, just return false.
   *
   * @return true if filters are supported, false otherwise
   */
  virtual bool SupportsFilter() = 0;

  /*! @brief provides the information about the cycles present in the recording
   *
   * @return list of cycleInformations
   */
  virtual std::vector<CycleInformation> GetCycleDescription() = 0;
};

} // namespace plugin
} // namespace player
} // namespace next

// C interface to enable usage in DLL loading
// Libraries should implement "create_reader_plugin"
// which is checked to identify if the lib is a reader plugin.
// e.g. reader_plugin_create_t create_reader_plugin() { return new MyReaderPlugin(); }
typedef next::player::plugin::ReaderInterface *reader_plugin_create_t;

#endif // NEXT_PLAYER_READER_INTERFACE_H

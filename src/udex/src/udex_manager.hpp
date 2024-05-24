/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
/**
 * @file    udex_manager.h
 * @brief   Manages next_udex dependencies and dll loading
 */
#ifndef NEXT_UDEX_MANAGER_HPP
#define NEXT_UDEX_MANAGER_HPP
#include <mutex>
#include <string>

#include <next/udex/publisher/package_processor.hpp>

namespace next {
namespace udex {

class UdexManagerImpl;

class UdexManager {
public:
  struct ProcessorInfo {
    std::shared_ptr<next::udex::publisher::PackageProcessor> processor_instance;
    std::vector<size_t> package_hashes;
    std::vector<next::sdk::types::IPackage::PackageMetaInfo> infos;
    std::string path;
    std::string name;
  };

  //! Disabled SignalWatcher copy constructor
  UdexManager(UdexManager &other) = delete;
  //! Disabled copy assignment
  void operator=(const UdexManager &) = delete;
  //! default destructor
  virtual ~UdexManager();

  /*! @brief get the current instances of the UDexManager
   *
   * @return instance of the udes manager
   */
  static UdexManager &GetInstance();

  /*! @brief load all processors matching from the defined path
   *
   * @param path system path either relativ or absolute. Evaluated based on the working directory of the processj
   * @return success if no crashes
   */
  bool loadProcessorDlls(const std::string &path);

  /*! @brief Get all processors
   *
   * @return list of processors
   */
  std::vector<std::shared_ptr<publisher::PackageProcessor>> GetAllProcessors();

  /*! @brief get all Processors loaded from paths since the last call to GetNewProcessors
   * Use this to receive necessary processors which need to be registered to the publishers
   * @return list of new processors
   */
  std::vector<ProcessorInfo> GetNewProcessors();

  /*! @brief receive the currently matching processors for a respectiv Package Information
   *
   * @param hash Meta Information to filter the respective processors
   * @return list of matching processors for this Package Information
   */
  std::vector<std::shared_ptr<publisher::PackageProcessor>> GetProcessorByHash(const size_t hash);

  /*! @brief Reset the UDex Manger
   * unloads all dlls and clears all maps
   * @return
   */
  bool Reset();

private:
  //! @brief add the processor to the internal maps
  void AddProcessorToInfoMap(std::shared_ptr<next::udex::publisher::PackageProcessor> new_processor,
                             const std::string &path, const std::string &name);

  //! used to remove the current instance
  void DeleteInstance();

  //! private constructor for singleton pattern due to dll loading
  UdexManager();

  std::mutex protect_manager_;            //!< mutex for list access
  std::unique_ptr<UdexManagerImpl> impl_; //!< internal maps
};

} // namespace udex
} // namespace next
#endif // NEXT_UDEX_MANAGER_HPP

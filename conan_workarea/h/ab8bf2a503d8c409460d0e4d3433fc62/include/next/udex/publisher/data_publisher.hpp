/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
/*
 * @file     data_publisher.h
 * @brief    Class to handle the registration of a new data provider
 *           to enable publishing data via the middleware network
 */

#ifndef NEXT_UDEX_DATA_PUBLISHER_H_
#define NEXT_UDEX_DATA_PUBLISHER_H_

#define CREATE_PUBLISHERS_DEFAULT false
#define DEFAULT_FORMAT_IDENTIFIER "mts.mta"

#include <memory>
#include <string>
#include <unordered_map>

#include <next/sdk/sdk_macros.h>
#include <next/sdk/types/data_description_types.hpp>
#include <next/sdk/types/package_data_types.hpp>
#include <next/sdk/types/package_hash.hpp>

#include "../data_types.hpp"
#include "../udex_config.h"

namespace next {
namespace udex {
namespace publisher {

class DataPublisherImpl;

NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS

class UDEX_LIBRARY_EXPORT DataPublisher {
public:
  //! creates a DataPublisher with the given node name.
  /*! initialize ecal node
   *
   * @param data_publisher_name  Name of the node
   */
  DataPublisher(const std::string &data_publisher_name);

  //! standard destructor for DataPublisher
  virtual ~DataPublisher();

  //! Resets the previously registered data description of this data provider
  /*!
   * Removes all entries in the signalstorage that is stored under the given name
   *
   * @return true if successful, false if error occured
   */
  virtual bool Reset(void);

  //! returns the internally used name
  virtual std::string getName();

  //! set data source info
  /*!
   * Update data_source name, source_id and instance_number
   * The function is optional and should be used only if someone wants to register an sdl under a different device than
   * SIM VFB Currently used in tests to mock an sdl file without using a player
   *
   * @param data_source_name name of the data source (default value is 'SIM VFB')
   * @param source_id        source id (default value is 87)
   * @param instance_number  instance number (default value is 37)
   * @return true if successful, false if error occured
   */
  virtual bool SetDataSourceInfo(const std::string &data_source_name, uint16_t source_id, uint32_t instance_number,
                                 const std::string &format_type = DEFAULT_FORMAT_IDENTIFIER);

  //! register a data description for this data provider
  /*!
   * Injects data description into commonly used dynamic signal description. Supported formats are
   * .sdl, .fibex, .can, .ethernet...
   *  virtual void RegisterDataDescription(std::string filename);
   * TODO improve description type - maybe an enum?
   *
   * @param description_filepath  file path to the data description file
   * @param description_type      the type of the data description
   * @param callback              function used to return progress information
   * @return true if successful, false if error occured
   */
  virtual bool RegisterDataDescription(const std::string &description_filepath,
                                       const bool create_publishers = CREATE_PUBLISHERS_DEFAULT,
                                       DescriptionType description_type = DESCRIPTION_TYPE_SDL,
                                       statusCallbackT callback = NULL);

  //! register a data description for this data provider
  /*!
   * Injects data description into commonly used dynamic signal description. Supported formats are
   * virtual void RegisterDataDescription(std::vector<mts::extensibility::data_source> data_sources,
   * std::shared_ptr<mts::extensibility::data_source_provider> data_source_provider);
   * TODO align with player how the provider / device / ... has to be handled
   *
   * @param binary_description  cdl binary representation of signal definition
   * @param description_type    the type of the data description
   * @param callback            function used to return progress information
   * @return true if successful, false if error occured
   */
  virtual bool RegisterDataDescription(const char *file_name, void *binary_description, const size_t content_size,
                                       const bool create_publishers = CREATE_PUBLISHERS_DEFAULT,
                                       DescriptionType description_type = DESCRIPTION_TYPE_SDL,
                                       statusCallbackT callback = NULL);

  //! register a data description for this data provider
  /*!
   * @param data_description Data description which is registered and publisher are created
   * @return true if successful, false if error occured
   */
  virtual bool RegisterDataDescription(const next::sdk::types::DataDescription &data_description,
                                       statusCallbackT callback = NULL);

  //! register a single port for this data provider
  /*!
   * Injects "data description" into commonly used dynamic signal description.
   * There is no actual description for this port so it is treated as a array of bytes with size.
   *
   * @param port_name         cdl binary representation of signal definition
   * @param port_size         the type of the data description
   * @param description_type  the type of the data description
   * @param callback          function used to return progress information
   * @return true if successful, false if error occured
   */
  virtual bool RegisterDataDescription(const std::string &port_name, size_t port_size,
                                       const bool create_publishers = CREATE_PUBLISHERS_DEFAULT,
                                       const std::string &description_type = "NONE", statusCallbackT callback = NULL);

  //! get a List of all topics, that could be published by the instance
  /*!
   * Legacy function for creating the publishers externally
   * TODO align with player about the usage of the id
   *
   * @return list of topics
   */
  virtual const std::unordered_map<std::string, size_t> &getTopicsAndHashes();

  virtual bool publishData(const uint64_t vaddr, const void *binary_data, const size_t binary_size,
                           const uint64_t trigger_timestamp = 0);
  virtual bool publishData(const std::string &port_name, const void *binary_data, const size_t binary_size,
                           bool append_device = false, const uint64_t trigger_timestamp = 0);

  virtual bool publishPackage(std::unique_ptr<sdk::types::IPackage> &package);

  //! get filter expression from urls
  /*!
   * Retrieve a reader filter expression based on urls
   *
   * @param urls
   * @return filter expression
   */
  virtual std::string GetFilterExpressionFromURLs(const std::vector<std::string> &urls) const;

  bool LoadProcessors(const std::string &path);

private:
  std::unique_ptr<DataPublisherImpl> impl_;
};

NEXT_RESTORE_WARNINGS_WINDOWS

} // namespace publisher
} // namespace udex
} // namespace next

#endif // NEXT_UDEX_DATA_PUBLISHER_H_

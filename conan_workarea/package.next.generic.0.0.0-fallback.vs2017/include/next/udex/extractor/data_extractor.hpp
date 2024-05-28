/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
/*
 * @file     Data_extractor.h
 * @brief    Interface to extract actual data matching the requested formats
 *           signal names.
 */

#ifndef NEXT_UDEX_DATA_EXTRACTOR_H
#define NEXT_UDEX_DATA_EXTRACTOR_H

#include <list>
#include <map>
#include <memory>
#include <string>

#include <next/sdk/sdk_macros.h>

#include "../data_types.hpp"
#include "../udex_config.h"

namespace next {
namespace udex {
namespace extractor {

NEXT_DISABLE_DLL_INTERFACE_WARNING_WINDOWS

class DataExtractorImpl;

class UDEX_LIBRARY_EXPORT DataExtractor {
public:
  //! creates a DataExtractor instance
  DataExtractor();

  //! creates a DataExtractor instance
  /*!
   * @param adapter_dll name of the adapter dll, it is assumed it's in the same folder as the using node
   */
  DataExtractor(const std::string &adapter_dll);

  //! creates a DataExtractor instance
  /*!
   * @param extractionQueueMaxSize maximum size of the extraction queue
   */
  DataExtractor(unsigned short extractionQueueMaxSize);

  //! creates a DataExtractor instance
  /*!
   * @param adapter_dll name of the adapter dll, it is assumed it's in the same folder as the using node
   * @param extractionQueueMaxSize maximum size of the extraction queue
   */
  DataExtractor(const std::string &adapter_dll, unsigned short extractionQueueMaxSize);

  //! destroys the DataExtractor instance
  virtual ~DataExtractor();

  //! push the data received from DataSubscriber into the internal buffer mechanism
  /*!
   * @param topic_name  name of the topic that is being pushed
   * @param next::udex::UdexDataPackage  structure containing data package info (size and payload)
   */
  virtual void PushData(const std::string &topic_name, const next::udex::UdexDataPackage *data);

  //! gets the topic name and payload
  /*!
   * no processing is done on the payload, it is returned as it is received from the eCAL
   *
   * @param topic_name  topic that was received
   * @return raw udex data package
   */
  virtual UdexDataPackage GetRawData(const std::string &topic_name);

  //! returns true if data is available in the processed queue
  /*!
   * @param topic_name  topic that was received
   * @return true if data is available
   */
  virtual bool isDataAvailable(const std::string &topic_name);

  //! gets the newest interpreted data
  /*!
   * data is being processed based on data type (ECU data, CAN data) and is returned
   *
   * @param topic_name  topic that was received
   * @return data package containing pointer to paiload, size and timestamp

   * !!!note!!!: only port level urls are supported (structures/groups)
   */
  virtual TimestampedDataPacket GetExtractedData(const std::string &signal_url);

  //! gets the interpreted data as a queue
  /*!
   * Data is being processed based on data type (ECU data, CAN data) and stored in a queue.
   * The queue is returned to the user and locked from writing.
   * As the queue hast to be cleaned and freed for wirting, releaseExtractedQueue() has to be called
   * after the processing finished.
   *
   * @param topic_name  topic that was received
   * @return pointer to the payload
   */
  virtual std::list<std::shared_ptr<TimestampedDataPacket>>
  GetExtractedQueueWithTimestamp(const std::string &topic_name);

  //! frees the interpreted data queue
  /*!
   * The queue is cleaned and freed for wirting.
   * This is the counterpart to GetExtractedQueue()
   *
   * @param topic_name  topic that was received
   */
  virtual void ClearExtractedQueue(const std::string &topic_name);

  //! frees elements from the interpreted data queue
  /*!
   * The queue is partialy cleaned, usefull after data was extracted with syncronization
   * and old elements can be discarded
   *
   * @param topic_name  topic that was received
   * @param nr_of_elements_to_remove number of elements to be removed
   */
  virtual void RemoveElementsFromQueue(const std::string &topic_name, const size_t nr_of_elements_to_remove);

  //! gets a single signal value for a specific url
  /*!
   * return the value of one signal for the newest data in the processed queue,
   * and converts it to specified SignalType if possible and no loss of data
   *
   * @param signal_url  url of the signal that needs extracting
   * @param type  data type that will be returned
   * @return dataType variant type that can hold all the base types
   * @param (optional) signal_info can be provided if it was extracted before with GetInfo,
   * if not it's determined internally
   */
  virtual bool GetValue(const std::string &signal_url, const SignalType type, dataType &return_value,
                        const SignalInfo *signal_info = nullptr);

  //! gets a single signal value for a specific url if that url is a vector
  /*!
   * return the value of one signal for the newest data in the processed queue,
   * and converts it to specified SignalType and returns a vector with all values
   *
   * @param signal_url  url of the signal that needs extracting
   * @param type  data type that will be returned
   * @return vector containing dataType type that can hold all the base types
   * @param (optional) signal_info can be provided if it was extracted before with GetInfo,
   * if not it's determined internally
   */
  virtual bool GetVectorValue(const std::string &signal_url, const SignalType type,
                              std::vector<dataType> &return_vector, const SignalInfo *signal_info = nullptr);

  //! gets a single signal value for a specific url and info about the signal
  /*!
   * return the value of one signal for the newest data in the processed queue,
   * and converts it to specified SignalType and returns a vector with all values
   * or a vector containing only one value if it's a basic signal
   * signal_info will contain information about the extracted signal
   *
   * @param signal_url  url of the signal that needs extracting
   * @return signal_info  signal information about the specified url
   * @return vector containing dataType type that can hold all the base types
   */
  virtual bool GetValueAndInfo(const std::string &signal_url, SignalInfo &signal_info,
                               std::vector<dataType> &return_vector);

  //! gets the signal information about a specified url
  /*!
   * return the value of one signal for the current data in the internal buffer,
   *
   * @param signal_url  url of the signal that needs extracting
   * @return signal information
   */
  virtual SignalInfo GetInfo(const std::string &signal_url);

  //! specify a callback function for one url
  /*!
   * when the specified signal_url data is available, the callback function will be called
   *
   * @param cb  callback function that will be used to notify the user when data has been received
   * @param signal_url signal_url to be specified when callback is called
   * @return id of the callback connection
   */
  virtual size_t connectCallback(next::udex::UdexCallbackT cb, std::string signal_url);

  //! remove a callback function
  /*!
   * remove the specified id callback function
   *
   * @param id of the callback connection
   */
  virtual void disconnectCallback(size_t id);

  //! get callbacks info (url, topic, callback function)
  /*!
   * gets all the information related to current connected callback functions and urls
   *
   * @return map with id as key and information for all the callbacks
   */
  virtual std::map<size_t, CallbackInfo> getCallbacksInfo();

  //! sets the version and the converter name to be used
  /*!
   * For a specified url, set the converter name to be used to convert the data to the expected version.
   *
   * This function is internally using @ref ConvertVersion, thus an exception is thrown if the version format is in
   * invalid format.
   *
   * @param url url of the signal that will have a version set
   * @param size of the structure the will be received, or is expected
   * @param version expected version number of format major.minor.patch[.bugfix], only numbers
   * @param data_type_name  converter name, part of the adapter_dll
   */
  virtual void SetExtractionVersion(const std::string &port_name, const size_t port_size, const std::string &version,
                                    const std::string &data_type_name);

  //! sets the version and the converter name to be used
  /*!
   * for a specified url, set the converter name to be used to convert the data to the expected version
   *
   * @param url url of the signal that will have a version set
   * @param size of the structure the will be received, or is expected
   * @param version expected version number as unsigned int, can be converted with ConvertVersion
   * @param data_type_name  converter name, part of the adapter_dll
   */
  virtual void SetExtractionVersion(const std::string &port_name, const size_t port_size, const uint32_t version,
                                    const std::string &data_type_name);

  //! converts the version form string to a unsigned int value
  /*!
   * If the version string is not in the expected format an exception of std::invalid_argument is thrown.
   *
   * @param version expected version number of format major.minor.patch[.bugfix], only numbers
   * @return version represented in unsigned int
   */
  virtual uint32_t ConvertVersion(const std::string &version);

  //! sets the behavior of the extractor when data is received.
  /*!
   * If this flag is true then every time data is received it blocks the publisher
   * until all callbacks are done executing. This applies also to new subscribed topics.
   * For changing the behavior of a single subscription the SetBlockingExtractionForUrl should be used.
   * OVERRIDES existing settings in all urls!
   *
   * @param blocking_flag self explanatory
   */
  virtual void SetBlockingExtraction(const bool blocking_flag);

  //! sets the blocking behavior for a specific url/topic
  /*!
   * this can be used if a specific url needs to block or not,
   * it overwrites the default and can be used during runtime
   *
   * @param url  url of the signal that needs extracting
   * @param blocking_flag self explanatory
   */
  virtual void SetBlockingExtractionForUrl(const std::string &url, const bool blocking_flag);

  //! based on set synchronization algo selects one data sample
  /*!
   * selects one data sample from buffered extracted data.
   * if no synchronization algo was set before the default is used
   * default algo selects the last element in the buffering queue
   *
   * @param signal_url - topic that was received
   * @param types - vector containing data type for each sync value
   * @param values - vector containing sync values, in variant type, should corelate with the types array
   * @return data package containing pointer to paiload, size and timestamp

   * !!!note!!!: only port level urls are supported (structures/groups)
   */
  virtual TimestampedDataPacket GetSyncExtractedData(const std::string &signal_url,
                                                     const std::vector<SyncValue> &sync_values);

  //! sets the syncronization algo used to identify expected data
  /*!
   * set a sync algo configuration to be used for a signal url
   *
   * @param signal_url - topic that was received
   * @param sync_fun syncronization algo configuration

   * !!!note!!!: only port level urls are supported (structures/groups)
   */
  virtual void SetSyncAlgo(std::string signal_url, SyncAlgoCfg sync_cfg);

  //! sets the default syncronization for a url
  /*!
   * used by data subscriber when subscribing and setting default algo for each subscription
   *
   * @param signal_url - topic that was received
   */
  virtual void setDefaultSync(std::string signal_url);

  //! sets the drop extra input flag
  /*!
   * when used in combination with blocking flag, if extra raw data is buffered,
   * drop it and process only the last element
   *
   * @param drop_extra_input - self explanatory
   */
  virtual void SetDropExtraInput(const bool drop_extra_input);

  //! sets the size differenc threshold
  /*!
   * some data packages can contain extra padding bytes at the end of the strucutre
   * that make it larger than what is defined in the SDL, those can be discarded.
   * by using this flag, the binary blob will be adapted to the SDL size
   *
   * @param diff size in bytes
   */
  virtual void setSizeDiffThreshold(const size_t diff);

  /*!
   * inserts an empty element in the buffering queue so that
   * it may be used by a component to add some data
   *
   * @param signal_url - topic for which we want to add an elemnt
   * @param size - the size of the data buffer that should be set
   * @return data package containing pointer to payload, size and timestamp

   * !!!note!!!: only port level urls are supported (structures/groups)
   */
  virtual TimestampedDataPacket InsertEmptyDataPacket(const std::string &signal_url, const std::size_t size);

private:
  std::unique_ptr<DataExtractorImpl> impl_;
};

NEXT_RESTORE_WARNINGS_WINDOWS

} // namespace extractor
} // namespace udex
} // namespace next
#endif // NEXT_UDEX_DATA_EXTRACTOR_H

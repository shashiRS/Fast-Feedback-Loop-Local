/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     signal_description_handler.h
 * @brief    class to handle the serialization of the signal descripton together
 * with some meta information
 */

#ifndef NEXT_UDEX_SIGNAL_DESCRIPTION_HANDLER_H
#define NEXT_UDEX_SIGNAL_DESCRIPTION_HANDLER_H

#include <string>
#include <vector>

#include "signal_storage_types.h"

namespace next {
namespace udex {
namespace ecal_util {

class SignalDescriptionHandler {
public:
  //! creates a SignalDescriptionHandler instance
  SignalDescriptionHandler(){};

  //! descsrtucts SignalDescriptionHandler instance
  virtual ~SignalDescriptionHandler(){};

  //! resets the SignalDescriptionHandler by reinitializing the internal datastructure
  bool Reset();

  /*! Set the binary signal description to the SignalDescriptionHandler
   *
   * @param signal_description input signal description to set to the SignalDescriptionHandler
   * @return returns true after setting the binary signal description to the SignalDescriptionHandler
   */
  bool SetBinarySignalDescription(const std::vector<char> &signal_description);

  /*!Set meta information (parent_url, mode dummy) to the SignalDescriptionHandler
   *
   * @param meta input metainformation to set to the SignalDescriptionHandler
   * @return returns true after setting the meta information
   */
  bool SetBasicInformation(const BasicInformation &meta);

  /*!Convert SignalDescriptionHandler data to string fromat by making use of json  format
   *
   * @return returns the converted string
   */
  std::string ToString(void) const;

  /*!Parse the input signal description to signal description format and store in SignalDescriptionHandler
   *
   *@param serialized_string input string which is signal description in strign format
   * @return returns true if the parsing is successful
   */
  bool ParseSignalDescriptionFromString(const std::string &serialized_string);

  /*!Get the parent url from the SignalDescriptionHandler
   *
   * @return returns the parent url string
   */
  std::string GetParentUrl() const;

  /*!Get the meta information from the SignalDescriptionHandler
   *
   * @return the metainformation (parent url,mode, dummy)
   */
  BasicInformation GetBasicInformation() const;

  /*!Get signal description (both meta info and binary description) from the SignalDescriptionHandler
   *
   * @return signal description
   */
  SignalDescription GetDescription(void) const;

  /*!Get binary description from the SignalDescriptionHandler
   *
   * @return binary deccription
   */
  std::vector<char> GetSignalDescription(void) const;

  /*!convert vector of characters to string format,mainly to generate binary description of the signal
   *
   * @return converted string
   */
  std::string MakeStringFromVec(const std::vector<char> &_vec) const;

private:
  /*!convert vector of characters to string format,mainly to generate binary description of the signal
   *
   * @return converted string
   */
  PackageMode TranslateIntToPackageMode(const int &mode_msg_) const;

  SignalDescription description_container_;
  int count = 0;
};

} // namespace ecal_util
} // namespace udex
} // namespace next

#endif // NEXT_UDEX_SIGNAL_DESCRIPTION_HANDLER_H

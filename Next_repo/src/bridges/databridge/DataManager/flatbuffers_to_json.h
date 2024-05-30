/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 *  ========================== NEXT Project ==================================
 */
/**
 * @file     flattbuffers_to_json.h
 * @brief    create JSON files from Flatbuffer data
 */

#ifndef NEXT_OUTPUTMANAGER_FALTBUFFERS_TO_JSON_H
#define NEXT_OUTPUTMANAGER_FALTBUFFERS_TO_JSON_H

#include <mutex>
#include <string>

#include <flatbuffers/idl.h>
#include <json/json.h>

#include <next/bridgesdk/datatypes.h>

namespace next {
namespace databridge {
namespace outputmanager {

static const std::string kDataValuePlaceholder{"DATA_PLACEHOLDER"};

class FlatBufferToJSON {
public:
  bool toJsonFromFBSPlugin(std::string path_to_fbs, const void *flatbuffer, std::string &output);

private:
  std::mutex fbs_parsers_mtx_;
  std::unordered_map<std::string, flatbuffers::Parser> fbs_parsers_;

  void GenerateTextForFbs(const std::string &path_to_fbs, const void *flatbuffer, std::string &out_text);
  bool CreateParserForFbsFile(const std::string &path_to_fbs);
  bool CheckParserForFbsFile(const std::string &path_to_fbs);
};

} // namespace outputmanager
} // namespace databridge
} // namespace next
#endif // NEXT_OUTPUTMANAGER_FALTBUFFERS_TO_JSON_H

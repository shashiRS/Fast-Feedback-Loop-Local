/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     flatbuffers_to_json.cpp
 *  @brief    see flatbuffers_to_json.h
 */

#include "flatbuffers_to_json.h"

#include <flatbuffers/util.h>
#include <boost/filesystem.hpp>

#include <next/plugins_utils/plugins_types.h>
#include <next/sdk/logger/logger.hpp>
#include "../../PluginManager/OutputSchema/output_schema_checker.h"

namespace next {
namespace databridge {
namespace outputmanager {

void FlatBufferToJSON::GenerateTextForFbs(const std::string &path_to_fbs, const void *flatbuffer,
                                          std::string &out_text) {
  out_text.clear();

  std::lock_guard<std::mutex> lock(fbs_parsers_mtx_);
  auto fbs_parser = fbs_parsers_.find(path_to_fbs);
  if (fbs_parser == fbs_parsers_.end()) {
    error(__FILE__, "GenerateTextForFbs: Couldn't find the parser in the cache!");
    return;
  }
  try {
    GenerateText(fbs_parser->second, flatbuffer, &out_text);
  } catch (const std::exception &ex) {
    warn(__FILE__, "GenerateTextForFbs: flatbuffer binary to json does not work: {0}", ex.what());
    return;
  } catch (...) {
    warn(__FILE__, "GenerateTextForFbs: flatbuffer binary to json does not work");
    return;
  }
}

bool FlatBufferToJSON::toJsonFromFBSPlugin(std::string path_to_fbs, const void *flatbuffer, std::string &output) {
  if (path_to_fbs.empty()) {
    return false;
  }
  if (!flatbuffer) {
    warn(__FILE__, "toJsonFromFBSPlugin: Empty flatbuffer binary input!");
    return false;
  }
  if (!CheckParserForFbsFile(path_to_fbs)) {
    if (!CreateParserForFbsFile(path_to_fbs)) {
      warn(__FILE__, "CreateParserForFbsFile: Failed to create ParserForFbsFile");
      return false;
    }
  }

  GenerateTextForFbs(path_to_fbs, flatbuffer, output);
  return (!output.empty());
}

bool FlatBufferToJSON::CreateParserForFbsFile(const std::string &path_to_fbs) {
  boost::filesystem::path file_path(path_to_fbs);
  boost::filesystem::path file_dir = file_path.parent_path();
  std::string path_to_fbs_folder = file_dir.string();
  const char *include_directories[] = {path_to_fbs_folder.c_str(), nullptr};

  // load FlatBuffer schema (.fbs) from disk
  std::string schemafile;
  bool ok = flatbuffers::LoadFile(path_to_fbs.c_str(), false, &schemafile);
  if (!ok) {
    warn(__FILE__, "CreateParserForFbsFile: Couldn't load the fbs file!\n");
    return false;
  }
  flatbuffers::IDLOptions strict_json_option;
  strict_json_option.strict_json = true; // add "quotes" around field names
  strict_json_option.force_defaults = true;

  std::lock_guard<std::mutex> lock(fbs_parsers_mtx_);
  fbs_parsers_.try_emplace(path_to_fbs, strict_json_option);
  auto fbs_parser = fbs_parsers_.find(path_to_fbs);
  if (fbs_parser == fbs_parsers_.end()) {
    debug(__FILE__, "CreateParserForFbsFile: failed to construct flatbuffers::Parser object.");
    return false;
  }
  try {
    // parse schema first, so we can use it to parse the data later
    if (fbs_parser->second.Parse(schemafile.c_str(), include_directories)) {
      return true;
    } else {
      fbs_parsers_.erase(fbs_parser);
      debug(__FILE__, "CreateParserForFbsFile: flatbuffer schema is not parsable {0}", schemafile);
      return false;
    }
  } catch (...) {
    fbs_parsers_.erase(fbs_parser);
    warn(__FILE__, "CreateParserForFbsFile: exception occured while parsing the schema file");
    return false;
  }
  return false;
}

bool FlatBufferToJSON::CheckParserForFbsFile(const std::string &path_to_fbs) {
  std::lock_guard<std::mutex> lock(fbs_parsers_mtx_);
  if (auto fbs_parser = fbs_parsers_.find(path_to_fbs); fbs_parser != fbs_parsers_.end()) {
    return true;
  }
  return false;
}

} // namespace outputmanager
} // namespace databridge
} // namespace next

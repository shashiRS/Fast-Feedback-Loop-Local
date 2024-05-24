/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */

// clang-format off
#if defined(_MSC_VER)
__pragma(warning(push)) 
__pragma(warning(disable : 4005))
#else
_Pragma("GCC diagnostic push")
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wsign-compare"
#endif

#define POCKETLZMA_LZMA_C_DEFINE
#include "pocketlzma.hpp"

#if defined(_MSC_VER)
__pragma(warning(pop))
#else
#pragma GCC diagnostic pop
#endif

#include "pocketlzma_wrapper.hpp"

bool PocketlzmaHelper::WriteMemoryDumpToFile(void *dump_data, size_t dump_size, std::string dump_name) {

  plz::PocketLzma p;
  /*!
   *  Possibilities:
   *  Default
   *  Fastest
   *  Fast
   *  GoodCompression
   *  BestCompression
   */
  p.usePreset(plz::Preset::BestCompression); // Default is used when preset is not set.

  std::vector<uint8_t> data_to_dump;
  data_to_dump.assign((uint8_t *)dump_data, (uint8_t *)dump_data + dump_size);
  std::vector<uint8_t> compressedData;

  plz::StatusCode status = p.compress(data_to_dump, compressedData);

  if (status == plz::StatusCode::Ok) {
    plz::FileStatus writeStatus = plz::File::ToFile(dump_name, compressedData);
    if (writeStatus.status() == plz::FileStatus::Code::Ok) {
      return true;
    }
  }
  return false;
}

bool PocketlzmaHelper::ReadMemoryDumpFromFile(std::vector<uint8_t>& decompressedData, std::string dump_name) {
    std::vector<uint8_t> data;
    
    plz::FileStatus fileStatus = plz::File::FromFile(dump_name, data);
    if(fileStatus.status() == plz::FileStatus::Code::Ok)
    {
        //No settings / presets are used during decompression!
        plz::PocketLzma p;
        plz::StatusCode status = p.decompress(data, decompressedData);
        if(status == plz::StatusCode::Ok)
        {
            return true;
        }
    }
    return false;
}

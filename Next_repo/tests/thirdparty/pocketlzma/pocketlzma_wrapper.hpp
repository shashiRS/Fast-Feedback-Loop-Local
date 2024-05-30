/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */

#ifndef POCKETLZMA_WRAPPER_H
#define POCKETLZMA_WRAPPER_H

#include <string>
#include <vector>

class PocketlzmaHelper {
public:
  static bool WriteMemoryDumpToFile(void *dump_data, size_t dump_size, std::string dump_name = "dump.lzma");
  static bool ReadMemoryDumpFromFile(std::vector<uint8_t> &decompressedData, std::string dump_name = "dump.lzma");
};

#endif // POCKETLZMA_WRAPPER_H

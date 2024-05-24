/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_player
 * ========================== NEXT Project ==================================
 */
/**
 * @file     statswriter.hpp
 * @brief    A small class writing values into a scv file
 *
 **/

#ifndef PERFORMANCETEST_STATSWRITER_H_
#define PERFORMANCETEST_STATSWRITER_H_

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

class Statswriter {
public:
  Statswriter(const std::string &filename, const std::vector<std::string> &header) : columns_(header.size()) {
    fstream_ = std::ofstream(filename);
    if (fstream_.is_open()) {
      for (size_t i = 0; header.size() > i; ++i) {
        if (0 != i)
          fstream_ << ";";
        fstream_ << header[i];
      }
    } else {
      throw std::ios_base::failure(std::string("unable to open ") + filename);
    }
  }
  ~Statswriter() { fstream_.close(); }

  void push(const std::string &entry) {
    if (fstream_.is_open()) {
      if (0 != columns_written_) {
        fstream_ << ";";
      } else {
        fstream_ << "\n";
      }
      fstream_ << entry;
      ++columns_written_;
      if (columns_ == columns_written_) {
        columns_written_ = 0;
      }
    }
  }

private:
  const size_t columns_;
  size_t columns_written_{0};
  std::ofstream fstream_;
};

#endif // PERFORMANCETEST_STATSWRITER_H_

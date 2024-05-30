/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 * @file     strings.hpp
 * @brief    some string handling helper functions
 *
 **/

#ifndef NEXTSDK_STRINGS_H_
#define NEXTSDK_STRINGS_H_

#include <stdexcept>
#include <string>
#include <vector>

namespace next {
namespace sdk {
namespace string {

//! splits a string into a vector of strings, separation based on delimiter d
/**
 * If the given delimiter is empty a invalid argument exception will be thrown.
 *
 * Examples:
 * * `split("hello.world", ".")` -> `{"hello", "world"}`
 * * `split("hello world", ".")` -> `{"hello world"}`
 * * `split("", ".")` -> `{""}`
 * * `split("hello..world", ".")` -> `{"hello", "", "world"}`
 *
 * @param s the string to split
 * @param d the delimiter
 * @return a vector of strings
 */
inline std::vector<std::string> split(const std::string &s, const std::string &d) noexcept(false) {
  std::vector<std::string> r;
  size_t p1 = 0;
  size_t p2 = 0;
  const size_t dl = d.length();
  if (0 == dl)
    throw std::invalid_argument("The delimiter cannot be empty.");
  while ((p2 = s.find(d, p1)) != std::string::npos) {
    r.push_back(s.substr(p1, p2 - p1));
    p1 = p2 + dl;
  }
  r.push_back(s.substr(p1));
  return r;
}

//! removes unwanted leading and trailing chars from a string
/**
 * The given string will be manipulated, removing unwanted chars.
 * Examples:
 * * `strip(" foo bar ")` -> `"foo bar"`
 * * `strip("\"Hello World!\"")` -> `"Hello World"`
 * * `strip("www.examples.com", "cemowz.")` -> `"xamples"`
 *     Note that the first 'e' is removed, the other one not!
 *
 * @param [in,out] str the string to manipulate
 * @param w a string of unwanted chars, by default double quote ('"') and blank (' ')
 */
inline void strip(std::string &str, const std::string &w = "\" ") noexcept {
  const size_t s = str.find_first_not_of(w);
  const size_t e = str.find_last_not_of(w);
  if (std::string::npos == s || std::string::npos == e) {
    str.clear();
  } else {
    str = str.substr(s, e + 1 - s);
  }
}

//! removes on every string of the vector unwanted leading or trailing chars
/**
 * @param [in,out] v the vector of strings to manipulate
 * @param w a string of unwanted chars, by default double quote ('"') and blank (' ')
 */
inline void strip(std::vector<std::string> &v, const std::string &w = "\" ") noexcept {
  for (auto &s : v) {
    strip(s, w);
  }
}

} // namespace string
} // namespace sdk
} // namespace next

#endif // NEXTSDK_STRINGS_H_

/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     regex_helper.h
 * @brief    Hidden implementation of helper for regular expressions to hide boost dependency
 *
 **/

#ifndef NEXT_REGEX_HELPER_HPP
#define NEXT_REGEX_HELPER_HPP

#include <boost/regex.hpp>
#include <next/sdk/sdk.hpp>

namespace next {
namespace bridgesdk {
namespace plugin_addons {
class RegexHelper {
public:
  RegexHelper(){};
  virtual ~RegexHelper(){};

  struct InternalSearchRequest {
    std::string data_type_name;
    std::vector<boost::regex> search_expression;
    bool isOptional;
  };

  boost::regex array_reduction_pattern =
      boost::regex(R"(^[\.a-zA-Z0-9_\s]*\[0\][\.a-zA-Z0-9_\[\]]*)", boost::regex::icase);

  void ReduceArrayToOneEntry(std::unordered_set<std::string> &url_list) const {
    if (url_list.empty()) {
      return;
    }

    for (auto it = url_list.begin(); it != url_list.end();) {
      if (it->find("[", 0) != std::string::npos)
      // if url has opening bracket, continue checking for array[0] pattern
      {
        try {
          if (!boost::regex_search(*it, array_reduction_pattern)) {
            it = url_list.erase(it);
            continue;
          }
        } catch (boost::regex_error &e) {
          warn(__FILE__, "RegexSearcher::ReduceArrayToOneEntry::exception with url: {0} {1}", *it, e.what());
        }
      }
      it++;
    }
  };

  std::vector<InternalSearchRequest> internal_search_requests_;
  std::vector<boost::regex> GetRegexFromStrings(const std::vector<std::string> &regular_expressions) {
    std::vector<boost::regex> result{};

    if (regular_expressions.size() == 0) {
      warn(__FILE__, "RegexSearcher::GetRegexFromStrings::empty regular expression list received");
      return result;
    }

    for (const auto &expression : regular_expressions) {
      try {
        result.push_back(boost::regex(expression, boost::regex::icase));
      } catch (boost::regex_error &e) {
        warn(__FILE__, "RegexSearcher::GetRegexFromStrings::exception while creating regex from: {0} : {1}", expression,
             e.what());
      }
    }
    return result;
  }

  bool GetMatchFromURL(const std::string &url, const std::vector<boost::regex> &regular_expressions,
                       std::string &result, std::string &pattern_used) {

    if (url.empty()) {
      return false;
    }
    for (const auto &regex : regular_expressions) {
      try {
        boost::smatch match;
        if (boost::regex_search(url, match, regex)) {
          if (!match.empty()) {
            pattern_used = regex.str();
            result = match.str(1);
            return !result.empty();
          }
        }
      } catch (boost::regex_error &e) {
        warn(__FILE__, "RegexSearcher::GetMatchFromURL::exception with regex: {1} url: {2} {3}", regex.str(), url,
             e.what());
      }
    }
    return false;
  }
};
} // namespace plugin_addons
} // namespace bridgesdk
} // namespace next
#endif // NEXT_REGEX_HELPER_HPP

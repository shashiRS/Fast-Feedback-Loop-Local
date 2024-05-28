/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_regex_searcher.hpp
 * @brief    utility class for regex searching
 **/

#ifndef NEXT_BRIDGESDK_PLUGIN_REGEX_SEARCHER_H_
#define NEXT_BRIDGESDK_PLUGIN_REGEX_SEARCHER_H_

#include <unordered_set>

#include <next/bridgesdk/plugin_config.h>

#include <next/sdk/types/package_data_types.hpp>

namespace next {
namespace bridgesdk {
namespace plugin_addons {

class RegexHelper;

class DECLSPEC_plugin_config PluginRegexSearcher {
public:
  friend class RegexSearcherTester;

  PluginRegexSearcher();
  virtual ~PluginRegexSearcher();

  using DataTypeName = std::string;
  using SignalUrl = std::string;

  struct Result {
    SignalUrl result_url;
    std::string regex_pattern;
  };

  struct SearchResult {
    std::string root_url;
    std::unordered_map<DataTypeName, Result> data_type_to_result;
  };

  struct SearchRequest {
    std::string data_type_name;
    std::vector<std::string> search_expression;
    bool isOptional;
  };

  //! Store the compiled regex patterns in a vector of internal search requests
  /*!
   * Checks for invalid regex patterns.
   *
   * Compiles valid patterns for each search request in vector and store them
   *
   * @param search_requests Vector of SearchRequest to be compiled and stored
   */
  void InitializeSearchRequests(const std::vector<SearchRequest> &search_requests);

  //! Find all available root urls which contain all mandatory search requests
  /*!
   * @param urls_to_scan unordered set of urls which need to be parsed
   * @return vector of strings representing the group level from url
   */
  std::vector<std::string> FindAvailableRootUrls(const std::unordered_set<std::string> &urls_to_scan);

  //! Find search result for a specific url
  /*!
   * Filters the urls_to_scan to contain only strings which start with provided url
   *
   * Extract from urls_to_scan the matched patterns using internal search requests
   *
   * @param url group level url
   * @param urls_to_scan unordered set of urls to be parsed
   * @return search result found for the given url
   */
  SearchResult GetSearchResultForUrl(const std::string &url, const std::unordered_set<std::string> &urls_to_scan);

  //! Find all search results
  /*!
   * Extract from urls_to_scan the matched patterns using internal search requests
   *
   * @param urls_to_scan unordered set of urls to be parsed
   * @return vector of search results
   */
  std::vector<SearchResult> GetSearchResults(const std::unordered_set<std::string> &urls_to_scan);

private:
  bool initialized_;

  bool CheckMandatoryDataFields(SearchResult &search_result);
  std::string ExtractGroupFromURL(const std::string &url) const;
  inline std::string addLeadingDot(const std::string &str) const { return str.front() != '.' ? "." + str : str; };
  RegexHelper *regex_helper_;
};

} // namespace plugin_addons
} // namespace bridgesdk
} // namespace next

#endif // NEXT_BRIDGESDK_PLUGIN_REGEX_SEARCHER_H_

/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_bridges
 * ========================== NEXT Project ==================================
 */
/**
 * @file     plugin_regex_searcher.cpp
 * @brief    implements the regex searcher for plugins
 **/

#include <next/bridgesdk/plugin_addons/plugin_regex_searcher.hpp>
#include "regex_helper.hpp"

namespace next {
namespace bridgesdk {
namespace plugin_addons {

PluginRegexSearcher::PluginRegexSearcher() {
  initialized_ = false;
  regex_helper_ = new RegexHelper;
}

void PluginRegexSearcher::InitializeSearchRequests(
    const std::vector<next::bridgesdk::plugin_addons::PluginRegexSearcher::SearchRequest> &urls_to_regex) {
  if (urls_to_regex.empty()) {
    debug(__FILE__, "RegexSearcher::InitializeSearchRequests:: empty naming map received");
    return;
  }

  regex_helper_->internal_search_requests_.clear();

  for (const auto &entry : urls_to_regex) {
    regex_helper_->internal_search_requests_.emplace_back(RegexHelper::InternalSearchRequest{
        entry.data_type_name, regex_helper_->GetRegexFromStrings(entry.search_expression), entry.isOptional});
  }

  initialized_ = true;
}

std::string PluginRegexSearcher::ExtractGroupFromURL(const std::string &url) const {
  size_t pos = 0;
  unsigned int dot_counter = 0;
  const unsigned int url_level = 2;

  while (dot_counter < url_level + 1 && pos != std::string::npos) {
    pos = url.find('.', pos + 1);
    if (pos != std::string::npos) {
      dot_counter++;
    } else if (pos == std::string::npos && dot_counter == url_level) {
      // If we don't find enough dots, url is already group level, return the entire string
      return url;
    }
  }

  return pos != std::string::npos ? url.substr(0, pos) : "";
}

std::vector<std::string>
PluginRegexSearcher::FindAvailableRootUrls(const std::unordered_set<std::string> &urls_to_scan) {
  std::vector<std::string> result{};

  auto searchResults = GetSearchResults(urls_to_scan);
  for (const auto &search_result : searchResults) {
    result.push_back(search_result.root_url);
  }

  return result;
}

PluginRegexSearcher::SearchResult
PluginRegexSearcher::GetSearchResultForUrl(const std::string &url,
                                           const std::unordered_set<std::string> &urls_to_scan) {
  SearchResult result{};

  if (urls_to_scan.empty()) {
    debug(__FILE__, "RegexSearcher::GetSearchResultForUrl::empty url list received");
    return result;
  }

  std::unordered_set<std::string> reduced_list = urls_to_scan;
  regex_helper_->ReduceArrayToOneEntry(reduced_list);

  std::unordered_set<std::string> filtered_list;

  if (!url.empty()) {
    // make sure to filter out complete urls which do not contain url provided
    for (const auto &complete_url : reduced_list) {
      if (complete_url.find(url, 0) != std::string::npos) {
        filtered_list.insert(complete_url);
      }
    }
  } else {
    debug(__FILE__, "RegexSearcher::GetSearchResultForUrl::empty url provided");
    return result;
  }

  if (!initialized_) {
    debug(__FILE__, "RegexSearcher::GetSearchResultForUrl:: internal map not initialized");
    return result;
  }

  result.root_url = ExtractGroupFromURL(url);
  for (const auto &complete_url : filtered_list) {
    for (const auto &url_to_pattern_ : regex_helper_->internal_search_requests_) {
      std::string res;
      std::string matched_pattern;
      if (regex_helper_->GetMatchFromURL(complete_url, url_to_pattern_.search_expression, res, matched_pattern)) {
        result.data_type_to_result[url_to_pattern_.data_type_name] = Result{addLeadingDot(res), matched_pattern};
      };
    }
  }

  if (!CheckMandatoryDataFields(result)) {
    result = {};
  }

  return result;
}

std::vector<PluginRegexSearcher::SearchResult>
PluginRegexSearcher::GetSearchResults(const std::unordered_set<std::string> &urls_to_scan) {
  if (urls_to_scan.empty()) {
    debug(__FILE__, "RegexSearcher::GetSearchResults:: empty url list received");
    return {};
  }

  if (!initialized_) {
    debug(__FILE__, "RegexSearcher::GetSearchResults:: internal map not initialized");
    return {};
  }

  std::unordered_set<std::string> filtered_urls = urls_to_scan;
  regex_helper_->ReduceArrayToOneEntry(filtered_urls);

  // temporary map of root_url<naming_url, result>
  std::unordered_map<std::string, std::unordered_map<std::string, Result>> tmp;

  for (const auto &url : filtered_urls) {
    auto root_url = ExtractGroupFromURL(url);

    for (const auto &url_to_pattern_ : regex_helper_->internal_search_requests_) {
      std::string res;
      std::string matched_pattern;
      if (regex_helper_->GetMatchFromURL(url, url_to_pattern_.search_expression, res, matched_pattern)) {
        tmp[root_url][url_to_pattern_.data_type_name] = Result{addLeadingDot(res), matched_pattern};
      };
    }
  }

  std::vector<SearchResult> res{};
  for (const auto &entry : tmp) {
    res.push_back({entry.first, entry.second});
  }

  for (auto it = res.begin(); it < res.end();) {
    if (!CheckMandatoryDataFields(*it)) {
      it = res.erase(it);
    } else {
      it++;
    }
  }

  return res;
}

bool PluginRegexSearcher::CheckMandatoryDataFields(PluginRegexSearcher::SearchResult &search_result) {
  for (const auto &searchRequest : regex_helper_->internal_search_requests_) {
    if (search_result.data_type_to_result.count(searchRequest.data_type_name) == 0) {
      if (!searchRequest.isOptional) {
        return false;
      } else {
        search_result.data_type_to_result.emplace(std::make_pair(searchRequest.data_type_name, Result{"", ""}));
      }
    }
  }

  return true;
}

PluginRegexSearcher::~PluginRegexSearcher() {
  if (regex_helper_) {
    delete regex_helper_;
  }
}

} // namespace plugin_addons
} // namespace bridgesdk
} // namespace next

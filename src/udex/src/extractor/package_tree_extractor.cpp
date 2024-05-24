/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_udex
 * ========================== NEXT Project ==================================
 */
/*
 * @file     struct_tree_extractor.cpp
 * @brief    tree extractor source file
 */
#include <next/udex/struct_extractor/package_tree_extractor.hpp>

#include <regex>

#include <next/sdk/sdk.hpp>
#include <next/udex/explorer/signal_explorer.hpp>
#include <next/udex/extractor/data_extractor.hpp>
#include "../signal_storage/signal_watcher.h"

namespace next {
namespace udex {
namespace struct_extractor {

NEXT_DISABLE_DEPRECATED_WARNING
PackageTreeExtractor::PackageTreeExtractor(const std::string &parent_url) : parent_url_(parent_url) {
  root_ = std::make_shared<ExtractorContainer>(extractor::emptyRequestBasic);

  string_offset_root_url_ = FindPackageUrlPosFromFullUrl(parent_url);
  if (string_offset_root_url_ == 0) {
    next::sdk::logger::warn(__FILE__, "parent url is not parsed. Use a package url or lower.");
    init_successful_ = false;
    return;
  }

  root_->SetUrls(parent_url);
  auto explorer = next::udex::explorer::SignalExplorer();
  auto packages = explorer.GetPackageUrl(parent_url);
  bool exact_match = false;
  size_t match_index = 0;

  if (packages.size() != 1) {
    for (size_t i = 0; i < packages.size(); i++) {
      if (packages[i] == parent_url) {
        exact_match = true;
        match_index = i;
        break;
      }
    }
    if (!exact_match) {
      next::sdk::logger::warn(__FILE__,
                              "invalid PackageTreeExtractor root url {0}. "
                              "Not matching number of topics to match {1}",
                              parent_url, packages.size());
      init_successful_ = false;
      return;
    }
  }

  bool success;
  signal_tree_ = std::make_shared<ecal_util::SignalTree>(
      next::udex::ecal_util::SignalWatcher::GetInstance().getSignalTree(packages[match_index], success));
  if (!success) {
    next::sdk::logger::warn(__FILE__,
                            "invalid root url {0}. "
                            "Not matching SignalTree found",
                            parent_url, packages.size());
    init_successful_ = false;
    return;
  }

  if (!HandleLeafAsTreeExtractor(parent_url)) {
    CreateChildrenByUrl(parent_url, root_);
  }
  UpdateUrls();
  init_successful_ = true;
}
NEXT_RESTORE_WARNINGS

bool PackageTreeExtractor::SetupSuccessful() {
  NEXT_DISABLE_DEPRECATED_WARNING
  return init_successful_;
  NEXT_RESTORE_WARNINGS
}

std::string PackageTreeExtractor::GetParentUrl() { return parent_url_; }

bool PackageTreeExtractor::HandleLeafAsTreeExtractor(const std::string &root_url) {
  std::string signal_tree_search_string = CreateSearchStringFromFullUrl(root_url);
  auto children = signal_tree_->getChildrenInfo(signal_tree_search_string);
  if (children.size() == 0) {
    auto leaf = std::make_shared<AnyValue>(root_);
    // set emtpy url as the root container has the url already
    leaf->SetUrls("");
    signals_.push_back({root_url, leaf});
    return true;
  }
  return false;
}

void PackageTreeExtractor::UpdateUrls() {
  for (auto &signal : signals_) {
    signal.name = signal.value->GetFullUrl();
  }
}

void PackageTreeExtractor::SetMemory(const char *memory, size_t size_memory) { root_->SetAddress(memory, size_memory); }

void PackageTreeExtractor::CreateChildrenByUrl(const std::string &parent_url,
                                               std::shared_ptr<ExtractorContainer> container) {
  std::string signal_tree_search_string = CreateSearchStringFromFullUrl(parent_url);
  auto children = signal_tree_->getChildrenInfo(signal_tree_search_string);
  RemoveDuplicatesAndArrays(children);
  for (auto &child : children) {
    CreateSingleChildByUrl(parent_url, child, container);
  }
}

std::string PackageTreeExtractor::CreateSearchStringFromFullUrl(const std::string &parent_url) const {
  auto remove_index = [](const auto &signal_url) {
    std::string ret = std::regex_replace(signal_url, std::regex("\\[\\d+\\]"), "$1$2");
    return ret;
  };
  std::string signal_tree_search_string = parent_url.substr(string_offset_root_url_);
  signal_tree_search_string = remove_index(signal_tree_search_string);
  return signal_tree_search_string;
}

size_t PackageTreeExtractor::FindPackageUrlPosFromFullUrl(std::string full_url) {
  std::string cut = ".";
  size_t offset = 0;
  for (int i_substring = 0; i_substring < 2; i_substring++) {
    auto end = full_url.find(cut);
    if (end == std::string::npos) {
      return 0;
    }
    offset += end + 1u;
    int cast_safe_end = (int)end;
    full_url.erase(full_url.begin(), full_url.begin() + cast_safe_end + 1);
  }
  return offset;
}

void PackageTreeExtractor::CreateSingleChildByUrl(const std::string &current_url, const ChildInfo &child,
                                                  std::shared_ptr<ExtractorContainer> parent) {
  std::string full_url = current_url + "." + child.name;
  if (child.child_count == 0) {
    SetSingleLeaf(child, parent, full_url);
  }
  if (child.child_count > 0) {
    if (child.array_lenght == 1) {
      SetSingleContainer(child, parent, full_url);
    } else {
      SetArrayContainer(child, parent, full_url, child.array_lenght);
    }
  }
}
void PackageTreeExtractor::SetSingleContainer(const ChildInfo &child, const std::shared_ptr<ExtractorContainer> &parent,
                                              std::string &full_url) {
  auto container = std::make_shared<ExtractorContainer>(parent);
  container->SetUrls("." + child.name);
  CreateChildrenByUrl(full_url, container);
}
void PackageTreeExtractor::SetArrayContainer(const ChildInfo &child, const std::shared_ptr<ExtractorContainer> &parent,
                                             std::string &full_url, size_t array_length) {

  std::vector<std::shared_ptr<extractor::StructExtractorBasic>> cast_vector;
  std::vector<std::shared_ptr<ExtractorContainer>> containers;

  for (size_t i_child = 0; i_child < array_length; i_child++) {
    containers.push_back(std::make_shared<ExtractorContainer>(parent));
    containers[i_child]->SetUrls("." + child.name);
    CreateChildrenByUrl(full_url, containers[i_child]);
    cast_vector.push_back(containers[i_child]);
  }
  auto factory = next::udex::extractor::StructExtractorArrayFactory();
  factory.UpdateUrlInVector(cast_vector);
}

void PackageTreeExtractor::SetSingleLeaf(const ChildInfo &child, const std::shared_ptr<ExtractorContainer> &parent,
                                         const std::string &full_url) {
  auto leaf = std::make_shared<AnyValue>(parent);
  leaf->SetUrls("." + child.name);
  signals_.push_back({full_url, leaf});
}

template <typename T>
void PackageTreeExtractor::RemoveDuplicates(std::vector<T> &vec) {
  std::sort(vec.begin(), vec.end());
  vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
}

void PackageTreeExtractor::RemoveDuplicatesAndArrays(std::vector<ChildInfo> &children) {
  for (auto &child : children) {
    auto name = child.name;
    std::size_t pos = name.find("[");
    if (pos != std::string::npos) {
      name = name.substr(0, pos);
    }
    child.name = name;
  }
  RemoveDuplicates(children);
}

} // namespace struct_extractor
} // namespace udex
} // namespace next

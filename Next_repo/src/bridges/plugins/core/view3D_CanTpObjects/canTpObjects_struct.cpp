#include <regex>

#include "canTpObjects_struct.hpp"

namespace next::plugins {

CanTpObjects::CanTpObjects(StructExtractorBasic *parent) : StructExtractor(parent) {}

CanTpObjects CanTpObjects::Get() { return *this; }

CanTpObjectsList CanTpObjects::GetCanTpOjects() const { return can_tp_objects; }

bool CanTpObjects::SetUrls(const std::string &url) {
  if (!StructExtractorBasic::SetUrls(url)) {
    return false;
  }

  std::string first_id{""}, second_id{""};
  GetIds(url, first_id, second_id);

  auto updated_urls = AddIdToUrls(urls_, first_id);
  auto can_tp_object_1 = std::make_shared<TrafficObject>(this, TrafficParticipantType::CAN, updated_urls);
  can_tp_object_1->SetUrls(std::string());

  updated_urls = AddIdToUrls(urls_, second_id);
  auto can_tp_object_2 = std::make_shared<TrafficObject>(this, TrafficParticipantType::CAN, updated_urls);
  can_tp_object_2->SetUrls(std::string());

  can_tp_objects.insert({url, {can_tp_object_1, can_tp_object_2}});

  return true;
}

void CanTpObjects::GetIds(const std::string &url, std::string &first_id, std::string &second_id) {
  static const std::regex pattern(R"(_(\d{2})_(\d{2}))");
  std::sregex_iterator it(url.begin(), url.end(), pattern);
  std::sregex_iterator end;

  if (it != end) {
    first_id = "_" + it->str(1);
    second_id = "_" + it->str(2);
  }
}

std::unordered_map<std::string, std::string>
CanTpObjects::AddIdToUrls(const std::unordered_map<std::string, std::string> &urls, const std::string &id) {
  std::unordered_map<std::string, std::string> updated_urls;
  for (const auto &[key, url] : urls) {
    std::string updated_url = url + id;
    updated_urls.try_emplace(key, std::move(updated_url));
  }

  return updated_urls;
}

} // namespace next::plugins

/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_control
 * ========================== NEXT Project ==================================
 */
/**
 * @file     condition.hpp
 * @brief    condition with unique ID and can ve checked agains status code
 *
 **/

#ifndef NEXT_CONDITION_H
#define NEXT_CONDITION_H

#include <next/sdk/events/status_code_event.hpp>
#include <vector>

namespace next {
namespace controlbridge {

// Hash function for a generic type
template <typename T>
struct GenericHash {
  std::size_t operator()(const T &value) const {
    std::size_t hashValue = 0;
    for (const auto &element : value) {
      // Combine hash values using XOR (^) and a mixing function
      hashValue ^= std::hash<typename T::value_type>{}(element) + 0x9e3779b9 + (hashValue << 6) + (hashValue >> 2);
    }
    return hashValue;
  }
};

// Hash function for std::vector
template <typename... Types>
struct VectorHash {
  std::size_t operator()(const std::tuple<Types...> &tuple) const {
    std::size_t hashValue = 0;
    hashCombine(hashValue, tuple, std::index_sequence_for<Types...>{});
    return hashValue;
  }

private:
  template <typename Tuple, std::size_t... Indices>
  void hashCombine(std::size_t &seed, const Tuple &tuple, std::index_sequence<Indices...>) const {
    ((seed ^= GenericHash<std::tuple_element_t<Indices, Tuple>>{}(std::get<Indices>(tuple)) + 0x9e3779b9 + (seed << 6) +
              (seed >> 2)),
     ...);
  }
};

class Condition {
public:
  Condition(std::vector<std::string> component_name = {}, std::vector<int> status_codes = {},
            std::vector<next::sdk::events::StatusSeverity> severities = {},
            std::vector<next::sdk::events::StatusOrigin> origins = {})
      : component_name_(component_name), status_codes_(status_codes), severities_(severities), origins_(origins) {

    VectorHash<std::vector<std::string>, std::vector<int>, std::vector<next::sdk::events::StatusSeverity>,
               std::vector<next::sdk::events::StatusOrigin>>
        combinedHash;
    unique_id_ = combinedHash(std::make_tuple(component_name, status_codes, severities, origins));
  }

  bool isEqual(const Condition &other) const {
    return component_name_ == other.component_name_ && status_codes_ == other.status_codes_ &&
           severities_ == other.severities_ && origins_ == other.origins_ && unique_id_ == other.unique_id_;
  }

  bool Check(const next::sdk::events::StatusCodeMessage &msg) const;
  std::size_t GetId();

private:
  std::vector<std::string> component_name_;
  std::vector<int> status_codes_;
  std::vector<next::sdk::events::StatusSeverity> severities_;
  std::vector<next::sdk::events::StatusOrigin> origins_;
  std::size_t unique_id_ = 0;
};

} // namespace controlbridge
} // namespace next

#endif // NEXT_CONDITION_H

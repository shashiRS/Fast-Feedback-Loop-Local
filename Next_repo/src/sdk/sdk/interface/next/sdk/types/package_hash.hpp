/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */
#ifndef NEXT_PACKAGE_HASH_HPP
#define NEXT_PACKAGE_HASH_HPP

#include <string>
#include <unordered_map>

#include "package_data_types.hpp"

namespace next {
namespace sdk {
namespace types {

class PackageHash {
public:
  static inline size_t hashMetaInfo(const IPackage::PackageMetaInfo &meta) {
    std::size_t hash = 0;

    create_hash(hash, (uint64_t)meta.source_id, (uint64_t)meta.instance_number,
                (uint64_t)(meta.cycle_id + meta.service_id), (uint64_t)(meta.virtual_address + meta.method_id));
    return hash;
  }

  static inline size_t hashMetaInfoWithName(const IPackage::PackageMetaInfo &meta, const std::string package_name) {
    std::size_t hash = 0;
    std::hash<std::string> hasher;
    size_t hash_string = hasher(package_name);

    create_hash(hash, (uint64_t)meta.source_id, (uint64_t)meta.instance_number,
                (uint64_t)(meta.cycle_id + meta.service_id), (uint64_t)(meta.virtual_address + meta.method_id),
                hash_string);
    return hash;
  }
  static inline size_t hashManualPort(const std::string &port_name, size_t port_size) {
    std::size_t hash = 0;
    for (size_t i_char = 0; i_char < port_name.size(); i_char++) {
      create_hash(hash, (uint64_t)port_name[i_char]);
    }
    create_hash(hash, (uint64_t)port_size);
    return hash;
  }

  template <typename... Rest>
  static inline void create_hash(uint64_t &seed, const uint64_t v, Rest... rest) {
    seed ^= hash_value_fnv1a(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    create_hash(seed, rest...);
  }
  static inline void create_hash(uint64_t & /*seed*/) {}
  /*
   * See http://www.isthe.com/chongo/tech/comp/fnv/
   */
private:
  static inline uint64_t hash_value_fnv1a(const uint64_t value) {
    // 32 bit params
    // uint32_t constexpr fnv_prime = 16777619U;
    // uint32_t constexpr fnv_offset_basis = 2166136261U;

    // 64 bit params
    uint64_t constexpr fnv_prime = 1099511628211ULL;
    uint64_t constexpr fnv_offset_basis = 14695981039346656037ULL;

    uint64_t hash = fnv_offset_basis;

    hash ^= value;
    hash *= fnv_prime;

    return hash;
  }
};
} // namespace types
} // namespace sdk
} // namespace next
#endif // NEXT_PACKAGE_HASH_HPP

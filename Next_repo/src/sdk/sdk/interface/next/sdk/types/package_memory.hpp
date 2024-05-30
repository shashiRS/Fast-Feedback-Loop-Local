/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 *  ========================== NEXT Project ==================================
 */

#ifndef NEXT_PACKAGE_MEMORY_HPP
#define NEXT_PACKAGE_MEMORY_HPP
namespace next {
namespace sdk {
namespace types {

/*! @brief a simple memory holding class which can be used for safe memory together with IPackage
 *
 *
 */
class PackageArraySimpleMemory final {
public:
  PackageArraySimpleMemory(const size_t &bytes) {
    memory_ = new unsigned char[bytes];
    size_ = bytes;
  }

  PackageArraySimpleMemory(PackageArraySimpleMemory &&other) noexcept {
    this->memory_ = other.memory_;
    this->size_ = other.size_;

    other.memory_ = nullptr;
    other.size_ = 0;
  }

  PackageArraySimpleMemory &operator=(PackageArraySimpleMemory &&other) noexcept = default;
  ~PackageArraySimpleMemory() { delete[] memory_; }

  // delete this as for mts package
  PackageArraySimpleMemory() = delete;
  PackageArraySimpleMemory(const PackageArraySimpleMemory &other) = delete;
  PackageArraySimpleMemory operator=(const PackageArraySimpleMemory &other) = delete;

  unsigned char *GetPtr() { return memory_; };
  size_t GetSize() { return size_; };

private:
  unsigned char *memory_;
  size_t size_;
};

} // namespace types
} // namespace sdk
} // namespace next
#endif // NEXT_PACKAGE_MEMORY_HPP

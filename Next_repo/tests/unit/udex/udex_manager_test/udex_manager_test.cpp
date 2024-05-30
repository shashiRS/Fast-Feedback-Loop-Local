#include <thread>

#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <next/sdk/sdk.hpp>

#include "next/sdk/types/package_hash.hpp"
#include "udex_manager.hpp"

namespace next {
namespace udex {
namespace testing {

class UdexManagerTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  UdexManagerTestFixture() { this->instance_name_ = "UdexManagerTestFixture"; }
};

class testMemoryAlloc final {
public:
  testMemoryAlloc(size_t bytes) {
    memory = (unsigned char *)malloc(bytes);
    size = bytes;
  }
  testMemoryAlloc(testMemoryAlloc &&other) noexcept {
    this->memory = other.memory;
    this->size = other.size;

    other.memory = nullptr;
    other.size = 0;
  }
  testMemoryAlloc &operator=(testMemoryAlloc &&other) noexcept = default;

  // delete this as for mts package
  testMemoryAlloc() = delete;
  testMemoryAlloc(const testMemoryAlloc &other) = delete;
  testMemoryAlloc operator=(const testMemoryAlloc &other) = delete;

  static void deletePackage(testMemoryAlloc *object) { free(object->memory); }
  ~testMemoryAlloc() {}
  unsigned char *memory;
  size_t size;
};

class TestPackage : public next::sdk::types::IPackage {
public:
  TestPackage(testMemoryAlloc &&array)
      : IPackage(
            std::move(next::sdk::types::takePackageOwnership<testMemoryAlloc>(array, testMemoryAlloc::deletePackage))) {
    package_valid = true;
    send_always_test = false;
  };
  TestPackage(testMemoryAlloc &&array, bool send_always_in)
      : IPackage(
            std::move(next::sdk::types::takePackageOwnership<testMemoryAlloc>(array, testMemoryAlloc::deletePackage))) {
    package_valid = true;
    send_always_test = send_always_in;
  };
  ~TestPackage() override{};

  virtual const Payload GetPayload() override {
    testMemoryAlloc *test = (testMemoryAlloc *)internal_package_.get();
    package_valid = false;
    return {test->size, test->memory};
  }
  virtual size_t GetSize() override {
    testMemoryAlloc *test = (testMemoryAlloc *)internal_package_.get();
    return test->size;
  }
  virtual bool PayloadAvailable() override { return package_valid || send_always_test; };

  virtual PackageMetaType GetMetaType() const override { return type; }
  virtual const PackageMetaInfo &GetMetaInfo() override { return info; }
  virtual const PackageHeader &GetPackageHeader() override { return header; }
  virtual const std::string &GetPackageName() const override { return package_name; }
  virtual size_t GetPackageVaddr() const override { return 0; }
  virtual bool ResetPayloads() override { return package_valid = true; };
  PackageMetaType type = PACKAGE_META_TYPE_INFO;
  std::string package_name = "AlgoVehCycle.VehDyn";

private:
  bool package_valid = false;
  bool send_always_test = false;

  PackageMetaInfo info = {87, 37, 207, 0x20350000};
  PackageHeader header = {234, next::sdk::types::PACKAGE_FORMAT_TYPE_MTA, 0, 0};
};

TEST_F(UdexManagerTestFixture, check_only_single_instance) {

  UdexManager &instance_returned = UdexManager::GetInstance();
  auto thread_fun1 = [&instance_returned]() -> void {
    UdexManager &UdexManagerInstanceReturned = UdexManager::GetInstance();
    ASSERT_EQ(&UdexManagerInstanceReturned, &instance_returned) << "Multiple instance of UdexManager created";
  };

  auto thread_fun2 = [&instance_returned]() -> void {
    UdexManager &UdexManagerInstanceReturned = UdexManager::GetInstance();
    ASSERT_EQ(&UdexManagerInstanceReturned, &instance_returned) << "Multiple instance of UdexManager created";
  };

  auto thread_fun3 = [&instance_returned]() -> void {
    UdexManager &UdexManagerInstanceReturned = UdexManager::GetInstance();
    ASSERT_EQ(&UdexManagerInstanceReturned, &instance_returned) << "Multiple instance of UdexManager created";
  };

  std::thread test_thread_1(thread_fun1);
  std::thread test_thread_2(thread_fun2);
  std::thread test_thread_3(thread_fun3);

  test_thread_1.join();
  test_thread_2.join();
  test_thread_3.join();
}

TEST_F(UdexManagerTestFixture, dll_loader) {
  auto &udex_manager = UdexManager::GetInstance();
  udex_manager.loadProcessorDlls("./.");
  auto list = udex_manager.GetAllProcessors();
  EXPECT_EQ(list.size(), 2);
}

TEST_F(UdexManagerTestFixture, dll_loader_no_duplicates) {
  auto &udex_manager = UdexManager::GetInstance();
  udex_manager.loadProcessorDlls("./.");
  auto list = udex_manager.GetAllProcessors();
  EXPECT_EQ(list.size(), 2);
  udex_manager.loadProcessorDlls("./.");
  list = udex_manager.GetAllProcessors();
  EXPECT_EQ(list.size(), 2);
}

TEST_F(UdexManagerTestFixture, dll_loader_new_instances_success) {
  auto &udex_manager = UdexManager::GetInstance();
  udex_manager.loadProcessorDlls("./.");
  auto list = udex_manager.GetAllProcessors();
  EXPECT_EQ(list.size(), 2);
  udex_manager.loadProcessorDlls("./.");
  list = udex_manager.GetAllProcessors();
  EXPECT_EQ(list.size(), 2);
  auto listinfo = udex_manager.GetNewProcessors();
  EXPECT_EQ(listinfo.size(), 2);
  listinfo = udex_manager.GetNewProcessors();
  EXPECT_EQ(listinfo.size(), 0);
  list = udex_manager.GetAllProcessors();
  EXPECT_EQ(list.size(), 2);
  udex_manager.loadProcessorDlls("./paththatdoesnotexist/");
  listinfo = udex_manager.GetNewProcessors();
  EXPECT_EQ(listinfo.size(), 0);
  list = udex_manager.GetAllProcessors();
  EXPECT_EQ(list.size(), 2);
}

TEST_F(UdexManagerTestFixture, dll_loader_new_instances_with_reset_success) {
  auto &udex_manager = UdexManager::GetInstance();
  udex_manager.loadProcessorDlls("./.");
  auto list = udex_manager.GetNewProcessors();
  EXPECT_EQ(list.size(), 2);
  list = udex_manager.GetNewProcessors();
  EXPECT_EQ(list.size(), 0);

  udex_manager.Reset();
  udex_manager.loadProcessorDlls("./.");
  list = udex_manager.GetNewProcessors();
  EXPECT_EQ(list.size(), 2);
  list = udex_manager.GetNewProcessors();
  EXPECT_EQ(list.size(), 0);
}

TEST_F(UdexManagerTestFixture, dll_loader_reset_success) {
  auto &udex_manager = UdexManager::GetInstance();
  udex_manager.loadProcessorDlls("./.");
  auto list = udex_manager.GetAllProcessors();
  EXPECT_EQ(list.size(), 2);
  EXPECT_TRUE(udex_manager.Reset());
  udex_manager.loadProcessorDlls("./.");
  list = udex_manager.GetAllProcessors();
  EXPECT_EQ(list.size(), 2);
}

TEST_F(UdexManagerTestFixture, test_dummy_processor_interface) {
  auto &udex_manager = UdexManager::GetInstance();
  udex_manager.loadProcessorDlls("./.");
  auto list = udex_manager.GetAllProcessors();
  ASSERT_EQ(list.size(), 2);
  auto dummyProc = list.front();

  auto descriptions = dummyProc->provideDataDescription();
  ASSERT_EQ(descriptions.size(), 1);
  auto desc = descriptions[0];
  EXPECT_EQ(desc.instance_number, 1u);
  EXPECT_EQ(desc.source_id, 1234u);
  EXPECT_EQ(desc.binary_description.size(), 524);
  EXPECT_EQ(desc.device_name, "HwDeviceTest");
  EXPECT_EQ(desc.filename, "");
  EXPECT_EQ(desc.device_format_type, next::sdk::types::PACKAGE_FORMAT_TYPE_MTA_SW);
  EXPECT_EQ(desc.description_format_type, next::sdk::types::DESCRIPTION_FORMAT_TYPE_SDL);

  auto packageInfo = dummyProc->provideRequestedPackageInfo();
  ASSERT_EQ(packageInfo.size(), 1);
  auto meta = packageInfo[0];
  EXPECT_EQ(meta.virtual_address, 0x80000000);
  EXPECT_EQ(meta.cycle_id, 100u);
  EXPECT_EQ(meta.source_id, 9353u);
  EXPECT_EQ(meta.instance_number, 0u);

  auto proc_temp = udex_manager.GetProcessorByHash(sdk::types::PackageHash::hashMetaInfo(meta));
  ASSERT_EQ(proc_temp.size(), 2);
  testMemoryAlloc memory = testMemoryAlloc(10);
  std::unique_ptr<next::sdk::types::IPackage> test = std::make_unique<TestPackage>(std::move(memory));
  auto processed_package = proc_temp.front()->processPackage(test);
  ASSERT_TRUE(processed_package.get() != nullptr);
  EXPECT_EQ(processed_package->GetMetaInfo().instance_number, 1u);
}

} // namespace testing
} // namespace udex
} // namespace next
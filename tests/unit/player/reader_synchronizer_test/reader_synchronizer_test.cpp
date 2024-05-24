#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <iostream>

#include <next/sdk/sdk.hpp>
#include <next/sdk/types/package_data_types.hpp>

#include <next/sdk/types/data_description_types.hpp>
#include "reader/reader_synchronizer.hpp"

class ReaderSynchronizerTest : public next::sdk::testing::TestUsingEcal {
public:
  ReaderSynchronizerTest() { this->instance_name_ = "PlayerTestFixture"; }
};
namespace next {
namespace player {

// extension class for ReaderSynchronizer to enable injecting own test readers
class ReaderSynchronizerTester : public ReaderSynchronizer {
public:
  ReaderSynchronizerTester() { ReaderSynchronizer(); }

  bool openMemoryReaders(std::shared_ptr<next::player::plugin::ReaderInterface> reader1,
                         std::shared_ptr<next::player::plugin::ReaderInterface> reader2) {

    readers.insert(readers.end(), reader1);
    readers.insert(readers.end(), reader2);

    return initWithReaders();
  }
};

class PackageMemory final { // testMemroyAlloc from data_publisher_next_package_test.cpp

public:
  PackageMemory(size_t bytes) {
    memory_ = new unsigned char[bytes];
    size_ = bytes;
  }

  PackageMemory(PackageMemory &&other) noexcept {
    this->memory_ = other.memory_;
    this->size_ = other.size_;

    other.memory_ = nullptr;
    other.size_ = 0;
  }

  PackageMemory &operator=(PackageMemory &&other) noexcept = default;
  ~PackageMemory() { delete[] memory_; }

  // delete this as for mts package
  PackageMemory() = delete;
  PackageMemory(const PackageMemory &other) = delete;
  PackageMemory operator=(const PackageMemory &other) = delete;

  unsigned char *GetPtr() { return memory_; }

private:
  unsigned char *memory_;
  size_t size_;
};

class MemoryNextPackage : public next::sdk::types::IPackage {
public:
  MemoryNextPackage() = delete;
  ~MemoryNextPackage(){};

  MemoryNextPackage(PackageMemory &&pkg, uint64_t myTimestamp, size_t size)
      : next::sdk::types::IPackage(
            std::move(next::sdk::types::takePackageOwnership<PackageMemory>(pkg, [](void *) {}))) {
    packageHeader.timestamp = myTimestamp;
    size_ = size;
  };

  next::sdk::types::IPackage::PackageHeader packageHeader;
  next::sdk::types::IPackage::PackageMetaType packageMetaType;
  next::sdk::types::IPackage::PackageMetaInfo packageMetaInfo;

  bool payloadAvailable = true;

  bool PayloadAvailable() { return payloadAvailable; }

  const next::sdk::types::IPackage::Payload GetPayload() {
    Payload ret;
    ret.mem_pointer = 0x00;
    ret.size = 0x00;
    payloadAvailable = false;
    return ret;
  }

  size_t GetSize() { return size_; }

  const next::sdk::types::IPackage::PackageHeader &GetPackageHeader() { return packageHeader; }

  next::sdk::types::IPackage::PackageMetaType GetMetaType() const { return packageMetaType; }

  const next::sdk::types::IPackage::PackageMetaInfo &GetMetaInfo() { return packageMetaInfo; }

  const std::string &GetPackageName() const { return myPackageName; }

  size_t GetPackageVaddr() const { return 0u; }

  bool ResetPayloads() {
    payloadAvailable = true;
    return true;
  }

private:
  std::string myPackageName = "MemoryPackage";
  size_t size_;
};

// Memory Reader implementation to provide dummy data description and packages
// Memory reader 1 has packages with timestamps 2,4,6,8,10 and provides
// Data description with device "MemoryDevice1".
class MemoryReader : public next::player::plugin::ReaderInterface {
private:
  std::string file_type_;
  std::string device_name_;

public:
  MemoryReader(std::string file_type, std::string device_name) {
    file_type_ = file_type;
    device_name_ = device_name;
  }

  void pushPackage(size_t bytes, size_t timestamp) {
    if (bytes == 0) {
      myMemoryPackages.push_back({});
      return;
    }
    packagesProvided = false;
    PackageMemory packMem(bytes);
    std::unique_ptr<MemoryNextPackage> package(new MemoryNextPackage(std::move(packMem), timestamp, bytes));
    myMemoryPackages.push_back(std::move(package));
  }

  size_t num_package_publish_ = 1;

  std::set<std::string> GetSupportedFileTypes() {
    std::set<std::string> ret = std::set<std::string>();

    ret.insert(file_type_);

    return ret;
  }

  bool Reset() {
    packagesProvided = false;
    return true;
  }

  std::vector<std::unique_ptr<next::sdk::types::IPackage>> GetPackages() {
    std::vector<std::unique_ptr<next::sdk::types::IPackage>> ret = {};

    for (uint32_t i_package = 0; i_package < num_package_publish_; i_package++) {
      if (packagesProvided == false) {
        std::unique_ptr<next::sdk::types::IPackage> packageWithLowestTimestamp;
        // get first element
        packageWithLowestTimestamp = std::move(myMemoryPackages.at(0));

        // remove first element from package list
        myMemoryPackages.erase(myMemoryPackages.begin());

        if (myMemoryPackages.size() == 0) {
          packagesProvided = true;
        }
        // return the package with lowest timestamp
        ret.push_back(std::move(packageWithLowestTimestamp));
      }
    }
    return ret;
  }

  bool OpenFile(const std::vector<std::string> &file_paths,
                const next::player::plugin::LoadingProgressCallback loading_callback) {
    (void)file_paths;
    (void)loading_callback;
    // not required
    return true;
  }

  next::player::plugin::FileStats GetFileStats() {
    next::player::plugin::FileStats ret;

    // we have memory packages with timestamps from 2..10
    ret.start_timestamp = 2;
    ret.end_timestamp = 10;
    ret.file_size = 0x00;

    return ret;
  }

  std::vector<next::sdk::types::DataDescription> GetCurrentDataDescriptions() {
    next::sdk::types::DataDescription dataDescription;
    dataDescription.device_name = device_name_;

    std::vector<next::sdk::types::DataDescription> ret;
    ret.push_back(dataDescription);

    return ret;
  }

  bool SetCurrentPackageNames(const std::vector<std::string> &package_name_list,
                              std::vector<std::string> &additional_binary_package_names) {
    // not required in this test
    (void)package_name_list;
    (void)additional_binary_package_names;
    return true;
  }

  uint64_t GetCurrentTimestamp() {
    // not required in this test
    return 0x00;
  }

  bool CheckEndOfFile() {
    if (packagesProvided == true) {
      return true;
    } else {
      return false;
    }
  }

  bool JumpToTimestamp(uint64_t target_timestamp) {
    // not required in this test
    (void)target_timestamp;
    return true;
  }

  std::shared_ptr<ReaderInterface> CreateNewInstance() {
    // not required in this test
    return std::shared_ptr<ReaderInterface>(nullptr);
  }

  bool SetFilter(std::string filter_expression) {
    // not required in this test
    (void)filter_expression;

    return true;
  }

  bool SupportsFilter() {
    // not required in this test
    return false;
  }

  std::vector<CycleInformation> GetCycleDescription() {
    // not required in this test
    return {};
  }

private:
  bool packagesProvided = false;
  std::vector<std::unique_ptr<next::sdk::types::IPackage>> myMemoryPackages;
};

} // namespace player
} // namespace next

TEST_F(ReaderSynchronizerTest, check_device_names) {
  // memory readers to be created
  auto readerTest1 = new next::player::MemoryReader("fileType1", "MemoryDevice1");
  auto readerTest2 = new next::player::MemoryReader("fileType2", "MemoryDevice2");
  std::shared_ptr<next::player::plugin::ReaderInterface> reader1(readerTest1);
  std::shared_ptr<next::player::plugin::ReaderInterface> reader2(readerTest2);

  readerTest1->pushPackage(0x04, 2);
  readerTest1->pushPackage(0x04, 4);
  readerTest1->pushPackage(0x04, 6);
  readerTest1->pushPackage(0x04, 8);
  readerTest1->pushPackage(0x04, 10);

  readerTest2->pushPackage(0x04, 3);
  readerTest2->pushPackage(0x04, 5);
  readerTest2->pushPackage(0x04, 7);
  readerTest2->pushPackage(0x04, 9);

  next::player::ReaderSynchronizerTester *testClass = new next::player::ReaderSynchronizerTester();

  testClass->openMemoryReaders(std::move(reader1), std::move(reader2));

  std::vector<next::sdk::types::DataDescription> dataDescriptions = testClass->GetCurrentDataDescriptions();

  // Test would be:
  // data descriptions of ReaderSynchronizer should include device names from the MemoryReaders
  // that have been defined above
  bool memoryDevice1Available = false;
  bool memoryDevice2Available = false;
  for (next::sdk::types::DataDescription dataDescription : dataDescriptions) {
    if (dataDescription.device_name == "MemoryDevice1") {
      memoryDevice1Available = true;
    }
    if (dataDescription.device_name == "MemoryDevice2") {
      memoryDevice2Available = true;
    }
  }

  EXPECT_EQ(memoryDevice1Available, true)
      << "'MemoryDevice1' device name not available in list of device descriptions.";
  EXPECT_EQ(memoryDevice2Available, true)
      << "'MemoryDevice2' device name not available in list of device descriptions.";

  // test about Packages - should have timestamp order: 2,3,4,5,6,7,8,9,10
  // one reader has 2,4,6,8,10, other has 3,5,7,9
  //
  const uint64_t expectedTimestamps[9] = {2, 3, 4, 5, 6, 7, 8, 9, 10};
  const bool expectedEndOfFile[9] = {false, false, false, false, false, false, false, false, true};
  const size_t expectedNumberOfPackagesInEachCall = 1;
  for (int i = 0; i < 9; i++) {
    // call GetPackages 9 times where a package is expected as return value
    std::vector<std::unique_ptr<next::sdk::types::IPackage>> packages = testClass->GetPackages();

    // only one package is expected with each call
    EXPECT_EQ(packages.size(), expectedNumberOfPackagesInEachCall)
        << "The number of packages is " << packages.size() << " but expected is " << expectedNumberOfPackagesInEachCall;

    uint64_t currentTimestamp = packages[0]->GetPackageHeader().timestamp;
    EXPECT_EQ(currentTimestamp, expectedTimestamps[i])
        << "The timestamp of package with index " << i << " is " << currentTimestamp << " but expected is "
        << expectedTimestamps[i];

    bool currentEndOfFile = testClass->CheckEndOfFile();
    EXPECT_TRUE(currentEndOfFile == expectedEndOfFile[i])
        << "The EndOfFile check after GetPackage() with index " << i << " is " << currentEndOfFile
        << " but expected is " << expectedEndOfFile[i];
  }
}

TEST_F(ReaderSynchronizerTest, sort_multipackage) {
  // memory readers to be created
  auto readerTest1 = new next::player::MemoryReader("fileType1", "MemoryDevice1");
  auto readerTest2 = new next::player::MemoryReader("fileType2", "MemoryDevice2");
  std::shared_ptr<next::player::plugin::ReaderInterface> reader1(readerTest1);
  std::shared_ptr<next::player::plugin::ReaderInterface> reader2(readerTest2);

  readerTest1->num_package_publish_ = 3;
  readerTest1->pushPackage(0x04, 2);
  readerTest1->pushPackage(0x04, 3);
  readerTest1->pushPackage(0x04, 4);
  readerTest1->pushPackage(0x04, 8);
  readerTest1->pushPackage(0x04, 10);

  readerTest2->num_package_publish_ = 2;
  readerTest2->pushPackage(0x04, 5);
  readerTest2->pushPackage(0x04, 7);
  readerTest2->pushPackage(0x04, 9);

  next::player::ReaderSynchronizerTester *testClass = new next::player::ReaderSynchronizerTester();

  testClass->openMemoryReaders(std::move(reader1), std::move(reader2));

  size_t counter = 0;
  std::vector<size_t> timestamp_expected = {2, 3, 4, 5, 7, 8, 9, 10};
  while (!testClass->CheckEndOfFile()) {
    std::vector<std::unique_ptr<next::sdk::types::IPackage>> packages = testClass->GetPackages();
    for (auto &package : packages) {
      ASSERT_TRUE(counter < timestamp_expected.size());
      EXPECT_EQ(package->GetPackageHeader().timestamp, timestamp_expected[counter++]);
    }
  }
  EXPECT_EQ(counter, timestamp_expected.size());
}

TEST_F(ReaderSynchronizerTest, sort_multipackage_same_timestamp_reproducable) {
  // memory readers to be created
  for (int i = 0; i < 5; i++) {
    auto readerTest1 = new next::player::MemoryReader("fileType1", "MemoryDevice1");
    auto readerTest2 = new next::player::MemoryReader("fileType2", "MemoryDevice2");
    std::shared_ptr<next::player::plugin::ReaderInterface> reader1(readerTest1);
    std::shared_ptr<next::player::plugin::ReaderInterface> reader2(readerTest2);

    readerTest1->num_package_publish_ = 2;
    readerTest1->pushPackage(0x04, 2);
    readerTest1->pushPackage(0x04, 3);
    readerTest1->pushPackage(0x06, 5);
    readerTest1->pushPackage(0x06, 7);
    readerTest1->pushPackage(0x06, 9);

    readerTest2->num_package_publish_ = 1;
    readerTest2->pushPackage(0x04, 5);
    readerTest2->pushPackage(0x04, 7);
    readerTest2->pushPackage(0x04, 9);

    next::player::ReaderSynchronizerTester *testClass = new next::player::ReaderSynchronizerTester();
    testClass->openMemoryReaders(std::move(reader1), std::move(reader2));

    size_t counter = 0;
    std::vector<size_t> timestamp_expected = {2, 3, 5, 5, 7, 7, 9, 9};
    std::vector<size_t> size_expected = {0x04, 0x04, 0x06, 0x04, 0x06, 0x04, 0x06, 0x04};
    while (!testClass->CheckEndOfFile()) {
      std::vector<std::unique_ptr<next::sdk::types::IPackage>> packages = testClass->GetPackages();
      for (auto &package : packages) {
        ASSERT_TRUE(counter < timestamp_expected.size());
        ASSERT_TRUE(counter < size_expected.size());
        EXPECT_EQ(package->GetPackageHeader().timestamp, timestamp_expected[counter]);
        EXPECT_EQ(package->GetSize(), size_expected[counter]);
        counter++;
      }
    }
    EXPECT_EQ(counter, timestamp_expected.size());
  }
}

TEST_F(ReaderSynchronizerTest, sort_with_empty_packages_multipackage) {
  // memory readers to be created
  auto readerTest1 = new next::player::MemoryReader("fileType1", "MemoryDevice1");
  auto readerTest2 = new next::player::MemoryReader("fileType2", "MemoryDevice2");
  std::shared_ptr<next::player::plugin::ReaderInterface> reader1(readerTest1);
  std::shared_ptr<next::player::plugin::ReaderInterface> reader2(readerTest2);

  readerTest1->num_package_publish_ = 2;
  readerTest1->pushPackage(0, 2);
  readerTest1->pushPackage(0x04, 4);
  readerTest1->pushPackage(0x04, 5);
  readerTest1->pushPackage(0x04, 8);
  readerTest1->pushPackage(0x04, 10);

  readerTest2->num_package_publish_ = 3;
  readerTest2->pushPackage(0x04, 6);
  readerTest2->pushPackage(0x04, 7);
  readerTest2->pushPackage(0x04, 9);
  readerTest2->pushPackage(0x04, 20);

  next::player::ReaderSynchronizerTester *testClass = new next::player::ReaderSynchronizerTester();

  testClass->openMemoryReaders(std::move(reader1), std::move(reader2));

  size_t counter = 0;
  std::vector<size_t> timestamp_expected = {4, 5, 6, 7, 8, 9, 10, 20};
  while (!testClass->CheckEndOfFile()) {
    std::vector<std::unique_ptr<next::sdk::types::IPackage>> packages = testClass->GetPackages();
    for (auto &package : packages) {
      ASSERT_TRUE(counter < timestamp_expected.size());
      EXPECT_EQ(package->GetPackageHeader().timestamp, timestamp_expected[counter++]);
    }
  }
  EXPECT_EQ(counter, timestamp_expected.size());
}

TEST_F(ReaderSynchronizerTest, sort_with_gap_in_one_reader) {
  // memory readers to be created
  auto readerTest1 = new next::player::MemoryReader("fileType1", "MemoryDevice1");
  auto readerTest2 = new next::player::MemoryReader("fileType2", "MemoryDevice2");
  std::shared_ptr<next::player::plugin::ReaderInterface> reader1(readerTest1);
  std::shared_ptr<next::player::plugin::ReaderInterface> reader2(readerTest2);

  readerTest1->num_package_publish_ = 2;
  readerTest1->pushPackage(0x04, 4);
  readerTest1->pushPackage(0x04, 5);
  readerTest1->pushPackage(0x04, 6);
  readerTest1->pushPackage(0x04, 7);
  readerTest1->pushPackage(0x04, 8);
  readerTest1->pushPackage(0x04, 10);

  readerTest2->num_package_publish_ = 1;
  readerTest2->pushPackage(0x04, 9);
  readerTest2->pushPackage(0x04, 20);

  next::player::ReaderSynchronizerTester *testClass = new next::player::ReaderSynchronizerTester();

  testClass->openMemoryReaders(std::move(reader1), std::move(reader2));

  size_t counter = 0;
  std::vector<size_t> timestamp_expected = {4, 5, 6, 7, 8, 9, 10, 20};
  while (!testClass->CheckEndOfFile()) {
    std::vector<std::unique_ptr<next::sdk::types::IPackage>> packages = testClass->GetPackages();
    for (auto &package : packages) {
      ASSERT_TRUE(counter < timestamp_expected.size());
      EXPECT_EQ(package->GetPackageHeader().timestamp, timestamp_expected[counter++]);
    }
  }
  EXPECT_EQ(counter, timestamp_expected.size());
}

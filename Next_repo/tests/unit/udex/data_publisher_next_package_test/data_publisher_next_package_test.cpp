#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <thread>

#include <ecal/ecal.h>

#include <next/sdk/sdk.hpp>
#include <next/sdk/types/package_data_types.hpp>

#include <next/udex/publisher/data_publisher.hpp>
#include <next/udex/subscriber/data_subscriber.hpp>

using next::udex::extractor::DataExtractor;
using next::udex::publisher::DataPublisher;

class DataPublisherNextPackage : public next::sdk::testing::TestUsingEcal {
public:
  DataPublisherNextPackage() { this->instance_name_ = "DataPublisherTestFixture"; }
};

class CallbackTester {
public:
  CallbackTester(){};
  ~CallbackTester(){};

  void url_CallBack(const std::string signal_url) {
    callback_calls_counter++;
    std::cout << "url_CallBack called for url: " << signal_url << " " << callback_calls_counter << " times"
              << std::endl;
  }

  size_t connect_callback(std::shared_ptr<DataExtractor> extractor, const std::string signal_url) {
    auto bind_funcion = std::bind(&CallbackTester::url_CallBack, this, std::placeholders::_1);
    return extractor->connectCallback(bind_funcion, signal_url);
  }

  std::map<size_t, next::udex::CallbackInfo> get_callbacks_info(std::shared_ptr<DataExtractor> extractor) {
    return extractor->getCallbacksInfo();
  }

  size_t get_callback_calls_counter() { return callback_calls_counter; };

  void disconnect_callback(std::shared_ptr<DataExtractor> extractor, size_t id) { extractor->disconnectCallback(id); }

private:
  size_t callback_calls_counter = 0;
};

constexpr int wait_time_ms = 3000;
constexpr int wait_increment_ms = 100;
constexpr int after_send_delay_ms = 100;

bool DataWait(std::shared_ptr<DataExtractor> extractor, std::string url) {
  std::cout << "waiting for data in queue ";
  int waited_time = 0;
  while (!extractor->isDataAvailable(url)) {
    if (waited_time > wait_time_ms) {
      std::cerr << "wait_time expired" << std::flush;
      return false;
    }
    waited_time += wait_increment_ms;
    std::cout << "." << std::flush;
    std::this_thread::sleep_for(std::chrono::milliseconds(wait_increment_ms));
  }

  std::cout << "data extracted\n" << std::flush;
  return true;
}

static std::atomic<int> deleter_count = 0;

/* zolis mock package */
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

  static void deletePackage(testMemoryAlloc *object) {
    free(object->memory);
    deleter_count++;
  }
  ~testMemoryAlloc() {}
  unsigned char *memory;
  size_t size;
};

class TestPackage : public next::sdk::types::IPackage {
public:
  TestPackage(testMemoryAlloc &&array)
      : IPackage(
            std::move(next::sdk::types::takePackageOwnership<testMemoryAlloc>(array, testMemoryAlloc::deletePackage))) {
    payload_available_ = true;
    send_always_test = false;
  };
  TestPackage(testMemoryAlloc &&array, bool send_always_in)
      : IPackage(
            std::move(next::sdk::types::takePackageOwnership<testMemoryAlloc>(array, testMemoryAlloc::deletePackage))) {
    payload_available_ = true;
    send_always_test = send_always_in;
  };
  ~TestPackage() override{};

  virtual const Payload GetPayload() override {
    testMemoryAlloc *test = (testMemoryAlloc *)internal_package_.get();
    payload_available_ = false;
    return {test->size, test->memory};
  }
  virtual size_t GetSize() override {
    testMemoryAlloc *test = (testMemoryAlloc *)internal_package_.get();
    return test->size;
  }
  virtual bool PayloadAvailable() override { return payload_available_ || send_always_test; };

  virtual PackageMetaType GetMetaType() const override { return type; }
  virtual const PackageMetaInfo &GetMetaInfo() override { return info; }
  virtual const PackageHeader &GetPackageHeader() override { return header; }
  virtual const std::string &GetPackageName() const override { return package_name_; }
  virtual size_t GetPackageVaddr() const override { return 0; }
  virtual bool ResetPayloads() override { return payload_available_ = true; };
  PackageMetaType type = PACKAGE_META_TYPE_INFO;
  std::string package_name_ = "array_root.array_of_structs";

private:
  bool payload_available_ = false;
  bool send_always_test = false;

  PackageMetaInfo info = {87, 37, 2, 0x901103E8};
  PackageHeader header = {234, next::sdk::types::PACKAGE_FORMAT_TYPE_MTA, 0, 0};
};

TEST_F(DataPublisherNextPackage, IPackage_interfaceCheck_successfulSingleFreeMemory) {

  testMemoryAlloc memory = testMemoryAlloc(10);
  void *addres = memory.memory;
  EXPECT_NO_THROW({
    TestPackage package(std::move(memory));
    EXPECT_EQ(package.GetSize(), 10);
    EXPECT_NE(package.GetPayload().mem_pointer, nullptr);
    EXPECT_EQ(package.GetPayload().mem_pointer, addres);
    deleter_count = 0;
  });
  EXPECT_EQ(deleter_count, 1);
}

TEST_F(DataPublisherNextPackage, IPackage_uniquePointerinterfaceCheck_successfulSingleFreeMemory) {

  testMemoryAlloc memory = testMemoryAlloc(10);
  void *addres = memory.memory;
  {
    std::unique_ptr<next::sdk::types::IPackage> test = std::make_unique<TestPackage>(std::move(memory));
    EXPECT_EQ(test->GetSize(), 10);
    EXPECT_NE(test->GetPayload().mem_pointer, nullptr);
    EXPECT_EQ(test->GetPayload().mem_pointer, addres);
    deleter_count = 0;
  }
  EXPECT_EQ(deleter_count, 1);
}

template <typename datatype>
bool SetValue(void *memory, size_t byte_position, datatype value, size_t maxsize) {
  if (byte_position + sizeof(datatype) > maxsize) {
    return false;
  }
  datatype *valueposition = (datatype *)&(((char *)memory)[byte_position]);
  *valueposition = value;
  return true;
}

template <typename datatype>
datatype GetValue(void *memory, size_t byte_position, size_t maxsize) {
  if (byte_position + sizeof(datatype) > maxsize) {
    return 0;
  }
  datatype *valueposition = (datatype *)&(((char *)memory)[byte_position]);
  return *valueposition;
}
TEST_F(DataPublisherNextPackage, DataPublisherSendPackage_AbortToManyPackages) {
  eCAL::Util::EnableLoopback(true);
  auto publisher = DataPublisher("DataPublisherSendPackage_AbortToManyPackages");
  auto device_name = "DataPublisherSendPackage_AbortToManyPackages";
  ASSERT_TRUE(publisher.SetDataSourceInfo(device_name, 87, 37));

  size_t memory_size = 20;
  testMemoryAlloc memory = testMemoryAlloc(memory_size);

  ASSERT_TRUE(SetValue<unsigned long>((void *)memory.memory, 0, 1234, memory_size));
  ASSERT_TRUE(SetValue<unsigned long>((void *)memory.memory, 4, 2, memory_size));
  ASSERT_TRUE(SetValue<unsigned short>((void *)memory.memory, 8, 3, memory_size));
  ASSERT_TRUE(SetValue<unsigned short>((void *)memory.memory, 10, 4, memory_size));
  ASSERT_TRUE(SetValue<unsigned char>((void *)memory.memory, 12, 5, memory_size));
  ASSERT_TRUE(SetValue<unsigned char>((void *)memory.memory, 13, 6, memory_size));

  std::unique_ptr<next::sdk::types::IPackage> package = std::make_unique<TestPackage>(std::move(memory), true);

  EXPECT_FALSE(publisher.publishPackage(package)) << "watchdog to limit non-ending packages failed";
}

TEST_F(DataPublisherNextPackage, DataPublisherSendPackage_PackageWithName) {

  auto publisher = DataPublisher("DataPublisherSendPackage_PackageWithName");
  auto device_name = "DataPublisherSendPackage_PackageWithName";
  ASSERT_TRUE(publisher.SetDataSourceInfo(device_name, 87, 37));

  ASSERT_TRUE(publisher.RegisterDataDescription((std::string) "arrays.sdl", true)) << "RegisterDataDescription failed";
  size_t memory_size = 20;
  testMemoryAlloc memory = testMemoryAlloc(memory_size);
  std::unique_ptr<TestPackage> package = std::make_unique<TestPackage>(std::move(memory));
  package->type = next::sdk::types::IPackage::PACKAGE_META_TYPE_NAME;
  std::unique_ptr<next::sdk::types::IPackage> ipackage = std::move(package);
  EXPECT_FALSE(publisher.publishPackage(ipackage)) << "package name without device should fail";
}

TEST_F(DataPublisherNextPackage, DataPublisherSendPackage_sendRawMessageToSmallByName_ExpectDataFilledAndPadded) {
  eCAL::Util::EnableLoopback(true);

  auto publisher = DataPublisher("DataPublisherSendPackage_sendRawMessageToSmallByName_ExpectDataFilledAndPadded");
  std::string device_name = "DataPublisherSendPackage_sendRawMessageToSmallByName_ExpectDataFilledAndPadded";
  ASSERT_TRUE(publisher.SetDataSourceInfo(device_name, 87, 37));
  ASSERT_TRUE(publisher.RegisterDataDescription((std::string) "arrays.sdl", true)) << "RegisterDataDescription failed";
  auto subscriber = next::udex::subscriber::DataSubscriber(
      "DataPublisherSendPackage_sendRawMessageToSmallByName_ExpectDataFilledAndPadded");
  auto extractor = std::make_shared<DataExtractor>();

  EXPECT_NE(subscriber.Subscribe(device_name + ".array_root.array_of_structs", extractor), 0u);
  CheckConnection(subscriber, {device_name + ".array_root.array_of_structs"});

  size_t memory_size = 20;
  testMemoryAlloc memory = testMemoryAlloc(memory_size);

  ASSERT_TRUE(SetValue<uint32_t>((void *)memory.memory, 0, 1234, memory_size));
  ASSERT_TRUE(SetValue<uint32_t>((void *)memory.memory, 4, 2, memory_size));
  ASSERT_TRUE(SetValue<unsigned short>((void *)memory.memory, 8, 3, memory_size));
  ASSERT_TRUE(SetValue<unsigned short>((void *)memory.memory, 10, 4, memory_size));
  ASSERT_TRUE(SetValue<unsigned char>((void *)memory.memory, 12, 5, memory_size));
  ASSERT_TRUE(SetValue<unsigned char>((void *)memory.memory, 13, 6, memory_size));

  std::unique_ptr<next::sdk::types::IPackage> package = std::make_unique<TestPackage>(std::move(memory));
  ASSERT_TRUE(publisher.publishPackage(package));

  DataWait(extractor, device_name + ".array_root.array_of_structs");
  auto extracted_data = extractor->GetExtractedQueueWithTimestamp(device_name + ".array_root.array_of_structs");
  ASSERT_EQ(extracted_data.size(), 1);
  auto extracted_data_element = extracted_data.front();
  void *msg_element = extracted_data_element->data;
  size_t msg_size = extracted_data_element->size;
  EXPECT_EQ(GetValue<uint32_t>(msg_element, 0, msg_size), 1234u);
  EXPECT_EQ(GetValue<uint32_t>(msg_element, 4, msg_size), 2u);
  EXPECT_EQ(GetValue<unsigned short>(msg_element, 8, msg_size), 3u);
  EXPECT_EQ(GetValue<unsigned short>(msg_element, 10, msg_size), 4u);
  EXPECT_EQ(GetValue<unsigned char>(msg_element, 12, msg_size), 5u);
  EXPECT_EQ(GetValue<unsigned char>(msg_element, 13, msg_size), 6u);
  EXPECT_EQ(GetValue<unsigned char>(msg_element, 20, msg_size), 0u);
}

TEST_F(DataPublisherNextPackage, DataPublisherSendPackage_sendRawMessageWithName) {
  eCAL::Util::EnableLoopback(true);

  auto publisher = DataPublisher("DataPublisherSendPackage_sendRawMessageWithName");
  std::string device_name = "DataPublisherSendPackage_sendRawMessageWithName";
  ASSERT_TRUE(publisher.SetDataSourceInfo(device_name, 87, 37));

  ASSERT_TRUE(publisher.RegisterDataDescription((std::string) "arrays.sdl", true)) << "RegisterDataDescription failed";
  auto subscriber = next::udex::subscriber::DataSubscriber("DataPublisherNextPackage__send_raw_package");
  auto extractor = std::make_shared<DataExtractor>();

  EXPECT_NE(subscriber.Subscribe(device_name + ".array_root.array_of_structs", extractor), 0u);
  CheckConnection(subscriber, {device_name + ".array_root.array_of_structs"});

  size_t memory_size = 20;
  testMemoryAlloc memory = testMemoryAlloc(memory_size);

  ASSERT_TRUE(SetValue<uint32_t>((void *)memory.memory, 0, 1234, memory_size));
  ASSERT_TRUE(SetValue<uint32_t>((void *)memory.memory, 4, 2, memory_size));
  ASSERT_TRUE(SetValue<unsigned short>((void *)memory.memory, 8, 3, memory_size));
  ASSERT_TRUE(SetValue<unsigned short>((void *)memory.memory, 10, 4, memory_size));
  ASSERT_TRUE(SetValue<unsigned char>((void *)memory.memory, 12, 5, memory_size));
  ASSERT_TRUE(SetValue<unsigned char>((void *)memory.memory, 13, 6, memory_size));

  std::unique_ptr<TestPackage> package = std::make_unique<TestPackage>(std::move(memory));
  package->type = next::sdk::types::IPackage::PACKAGE_META_TYPE_NAME;
  package->package_name_ = device_name + ".array_root.array_of_structs";
  std::unique_ptr<next::sdk::types::IPackage> ipackage = std::move(package);
  ASSERT_TRUE(publisher.publishPackage(ipackage));

  DataWait(extractor, device_name + ".array_root.array_of_structs");
  auto extracted_data = extractor->GetExtractedQueueWithTimestamp(device_name + ".array_root.array_of_structs");
  ASSERT_EQ(extracted_data.size(), 1);
  auto extracted_data_element = extracted_data.front();
  void *msg_element = extracted_data_element->data;
  size_t msg_size = extracted_data_element->size;
  EXPECT_EQ(GetValue<uint32_t>(msg_element, 0, msg_size), 1234u);
  EXPECT_EQ(GetValue<uint32_t>(msg_element, 4, msg_size), 2u);
  EXPECT_EQ(GetValue<unsigned short>(msg_element, 8, msg_size), 3u);
  EXPECT_EQ(GetValue<unsigned short>(msg_element, 10, msg_size), 4u);
  EXPECT_EQ(GetValue<unsigned char>(msg_element, 12, msg_size), 5u);
  EXPECT_EQ(GetValue<unsigned char>(msg_element, 13, msg_size), 6u);
  EXPECT_EQ(GetValue<unsigned char>(msg_element, 20, msg_size), 0u);
}

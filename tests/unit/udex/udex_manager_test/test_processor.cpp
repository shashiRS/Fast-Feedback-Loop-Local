#include "test_processor.hpp"

#include <next/sdk/sdk.hpp>
#include <next/sdk/types/package_memory.hpp>

processor_creator_t create_processor() { return new TestProcessor(); }

TestProcessor::TestProcessor() {}
TestProcessor::~TestProcessor() {}

class TestPackage : public next::sdk::types::IPackage {
public:
  TestPackage(next::sdk::types::PackageArraySimpleMemory &&array)
      : next::sdk::types::IPackage(std::move(
            next::sdk::types::takePackageOwnership<next::sdk::types::PackageArraySimpleMemory>(array, [](void *) {}))),
        payload_available_(true), memory_(std::move(array)){};

  ~TestPackage() override{};

  virtual const Payload GetPayload() override {
    Payload ret;
    ret.mem_pointer = memory_.GetPtr();
    ret.size = memory_.GetSize();
    payload_available_ = false;
    return ret;
  }
  virtual size_t GetSize() override { return memory_.GetSize(); }
  virtual bool PayloadAvailable() override { return payload_available_; };

  virtual PackageMetaType GetMetaType() const override { return type_; }
  virtual const PackageMetaInfo &GetMetaInfo() override { return info_; }
  virtual const PackageHeader &GetPackageHeader() override { return header_; }
  virtual const std::string &GetPackageName() const override { return package_name_; }
  virtual size_t GetPackageVaddr() const override { return 0; }
  virtual bool ResetPayloads() override { return payload_available_ = true; };
  void SetHeaderTimestamp(const size_t &timestamp) { header_.timestamp = timestamp; }

private:
  PackageMetaType type_ = PACKAGE_META_TYPE_INFO;
  bool payload_available_;
  next::sdk::types::PackageArraySimpleMemory memory_;
  PackageHeader header_ = {234, next::sdk::types::PACKAGE_FORMAT_TYPE_MTA, 100, 0};
  std::string package_name_ = "ADC585_CAMERAS.Channel_0.VideoStream_0.parsed";
  PackageMetaInfo info_ = {9353, 1, 100, 2147483648};
};

std::shared_ptr<next::udex::publisher::PackageProcessor> TestProcessor::CreateNewInstance() {
  auto dummy = std::make_shared<TestProcessor>();
  return std::dynamic_pointer_cast<next::udex::publisher::PackageProcessor>(dummy);
}

inline static const std::string_view kDummyHWSDLContent{R"(
<SdlFile xmlns:xsd="http://www.w3.org/2001/XMLSchema-instance" xsd:noNamespaceSchemaLocation="sdl2-after-compiler.xsd" ByteAlignment="1" Version="2.0">
	<View Name="Dummy" CycleID="100">
		<Group Name="Picture" Address="80000000" ArrayLen="1" Size="560884">
			<Signal Name="size" Offset="0" ArrayLen="1" Type="ulong" Bitmask="ffffffff" ByteOrder="little-endian" Size="4"/>
			<Signal Name="pic" Offset="4" ArrayLen="1000" Type="uchar" Bitmask="ffffffff" ByteOrder="little-endian" Size="1"/>
		</Group>
	</View>
</SdlFile> )"};

std::vector<next::sdk::types::DataDescription> TestProcessor::provideDataDescription() {
  std::vector<next::sdk::types::DataDescription> result;
  next::sdk::types::DataDescription description;
  description.filename = "";
  description.device_name = "HwDeviceTest";
  description.description_format_type = next::sdk::types::DESCRIPTION_FORMAT_TYPE_SDL;
  description.device_format_type = next::sdk::types::PACKAGE_FORMAT_TYPE_MTA_SW;

  description.binary_description.resize(kDummyHWSDLContent.size());
  std::copy_n(reinterpret_cast<const char *>(kDummyHWSDLContent.data()), kDummyHWSDLContent.size(),
              description.binary_description.data());
  description.instance_number = 1;
  description.source_id = 1234u;

  result.push_back(description);
  return result;
}

std::vector<next::sdk::types::IPackage::PackageMetaInfo> TestProcessor::provideRequestedPackageInfo() {
  next::sdk::types::IPackage::PackageMetaInfo meta;
  meta.source_id = 9353;
  meta.instance_number = 0;
  meta.cycle_id = 100;
  meta.virtual_address = 0x80000000;
  meta.method_id = 0;
  meta.service_id = 0;
  return {meta};
}

std::unique_ptr<next::sdk::types::IPackage>
TestProcessor::processPackage(const std::unique_ptr<next::sdk::types::IPackage> &package) {
  auto size = package->GetSize();
  auto payload = package->GetPayload().mem_pointer;
  next::sdk::types::PackageArraySimpleMemory pack_mem(size);
  memcpy(pack_mem.GetPtr(), payload, size);
  std::unique_ptr<TestPackage> test = std::make_unique<TestPackage>(std::move(pack_mem));
  test->SetHeaderTimestamp(package->GetPackageHeader().timestamp);
  return test;
}
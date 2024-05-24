How to implement a Package Processor {#howto_package_processor_example}
=====

[TOC]

---

* @ref howto_package_processor_overview
* @ref howto_setup_package_processor
* @ref howto_package_processor_example
* @ref howto_load_package_processor
* @ref howto_debug_package_processor
* ---

# Summary

This tutorial will focus on the actual implemenation and provide a simple example.
The example code can be found in the unittests at ```examples/package_processor/dummy_processor.cpp```.

## provideDataDescription

Fill in the required meta information about your Data Description (format, device name, etc.).
Here we will provide data with help of an _SDL_ as _MTA_SW packages_. The _FileName_ is ignored here.

```
  next::sdk::types::DataDescription description;
  description.filename = ""; //<- will be ignored
  description.device_name = "myDevice"; //<- device name which will be shown e.g. in the table view of the NEXT-GUI
  description.description_format_type = next::sdk::types::DESCRIPTION_FORMAT_TYPE_SDL; //<- needs to match the provided binary format
  description.device_format_type = next::sdk::types::PACKAGE_FORMAT_TYPE_MTA_SW; //<- needs to match description
```

Copy over the DataDescription into the binary buffer. Here the actual
_SDL_ is loaded from some inline string of the processor instead of a file.
If you use a file please make sure to load the file content into memory already.

```
  description.binary_description.resize(kDummyHWSDLContent.size());
  std::copy_n(reinterpret_cast<const char *>(kDummyHWSDLContent.data()), kDummyHWSDLContent.size(),
              description.binary_description.data());
```

Lastly we need to add meta information about our description

```
  description.instance_number = 1;
  description.source_id = 9353u;
```

## provideRequestedPackageInfo

Add all the details to the specific package you want to process and re-publish.
You can register multiple MetaInformation here. All packages matching will be transferred to the processor.
> ❗ If no packages are registered the package processor will be dropped ❗

```
std::vector<next::sdk::types::IPackage::PackageMetaInfo> DummyProcessor::provideRequestedPackageInfo() {
  next::sdk::types::IPackage::PackageMetaInfo meta;
  meta.source_id = 9353;
  meta.instance_number = 0;
  meta.cycle_id = 100;
  meta.virtual_address = 0x80000000;
  meta.method_id = 0;
  meta.service_id = 0;
  return {meta};
}
```

## processPackage

Read out the memory from the package.

```
  auto size = package->GetSize();
  auto payload = package->GetPayload().mem_pointer;
```

Create new memory via the helper class PackageArraySimpleMemory ```src/sdk/sdk/interface/next/sdk/types/package_memory.hpp```. This ensures a memory safe allocation and automatic cleanup.

```
  next::sdk::types::PackageArraySimpleMemory pack_mem(size);
  // put your code in here and copy the transformed package into the pack_mem afterwards
  memcpy(pack_mem.GetPtr(), payload, size);
```

Move the memory and its ownership to the unique pointer of you TestPackage which then cleans up automatically.
Make sure also align any metadata if required within the testpackage.
> ❗ Don't forget to move the unique pointer as it will otherwise get deleted and no data will be published ❗

```
  std::unique_ptr<TestPackage> test = std::make_unique<TestPackage>(std::move(pack_mem));
  test->SetHeaderTimestamp(package->GetPackageHeader().timestamp);
  return std::move(test);
```
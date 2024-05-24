#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <thread>

#include <next/sdk/sdk.hpp>

#include <next/udex/explorer/signal_explorer.hpp>
#include <next/udex/publisher/data_publisher.hpp>
#include "signal_explorer_test_data.hpp"

using next::udex::publisher::DataPublisher;

namespace next {
namespace udex {
namespace explorer {

class SignalExplTestFixture : public next::sdk::testing::TestUsingEcal {
public:
  SignalExplTestFixture() { this->instance_name_ = "SignalExplTestFixture"; }
};

TEST_F(SignalExplTestFixture, searchSignalTree_search_different_urls) {
  std::unique_ptr<DataPublisher> publisher1 = std::make_unique<DataPublisher>("searchSignalTree_keyword1");
  std::unique_ptr<SignalExplorer> signal_explorer = std::make_unique<SignalExplorer>();

  ASSERT_TRUE(publisher1->SetDataSourceInfo("searchSignalTree_keyword", 22, 42)) << "Failed to set the data source";
  ASSERT_TRUE(publisher1->RegisterDataDescription((const char *)"SDL_CONTENT", (void *)test::SDL_CONTENT.data(),
                                                  test::SDL_CONTENT.size(), true))
      << "Failed to register data description";
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  std::string url_complete = "searchSignalTree_keyword.AlgoVehCycle.VehDyn.MotionState.bRollerTestBench";
  std::vector<std::string> results_url_complete = signal_explorer->searchSignalTree(url_complete);

  std::string url_start_from_view = "AlgoVehCycle.VehDyn.MotionState.bRollerTestBench";
  std::vector<std::string> results_url_from_view = signal_explorer->searchSignalTree(url_start_from_view);

  std::string url_start_from_group = "VehDyn.MotionState.bRollerTestBench";
  std::vector<std::string> results_url_from_group = signal_explorer->searchSignalTree(url_start_from_group);

  std::string url_start_from_subgroup = "MotionState.bRollerTestBench";
  std::vector<std::string> results_url_from_subgroup = signal_explorer->searchSignalTree(url_start_from_subgroup);

  // search now for each section of the url
  std::string url_device = "searchSignalTree_keyword";
  std::vector<std::string> results_device_level = signal_explorer->searchSignalTree(url_device);

  std::string url_view = "AlgoVehCycle";
  std::vector<std::string> results_view_level = signal_explorer->searchSignalTree(url_view);

  std::string url_group = "VehDyn";
  std::vector<std::string> results_group_level = signal_explorer->searchSignalTree(url_group);

  std::string url_subgroup = "MotionState";
  std::vector<std::string> results_subgroup_level = signal_explorer->searchSignalTree(url_subgroup);

  std::string url_leaf_signal = "bRollerTestBench";
  std::vector<std::string> results_url_leaf_signal = signal_explorer->searchSignalTree(url_leaf_signal);

  ASSERT_TRUE(results_url_complete.size() == 1) << "Returned different than expected result vector.";
  ASSERT_TRUE(results_url_from_view.size() == 1) << "Returned different than expected result vector.";
  ASSERT_TRUE(results_url_from_group.size() == 1) << "Returned different than expected result vector.";
  ASSERT_TRUE(results_url_from_subgroup.size() == 1) << "Returned different than expected result vector.";

  ASSERT_TRUE(results_device_level.size() == 354)
      << "Number of signals found for device level is different than expected.";
  ASSERT_TRUE(results_view_level.size() == 55) << "Number of signals found for view level is different than expected.";
  ASSERT_TRUE(results_group_level.size() == 55)
      << "Number of signals found for group level is different than expected.";
  ASSERT_TRUE(results_subgroup_level.size() == 3)
      << "Number of signals found for subgroup level is different than expected.";
  ASSERT_TRUE(results_url_leaf_signal.size() == 1)
      << "Number of signals found for leaf level is different than expected.";

  EXPECT_EQ(results_url_complete.front(), url_complete);
  EXPECT_EQ(results_url_from_view.front(), url_complete);
  EXPECT_EQ(results_url_from_group.front(), url_complete);
  EXPECT_EQ(results_url_from_subgroup.front(), url_complete);
  EXPECT_EQ(results_url_leaf_signal.front(), url_complete);
}

TEST_F(SignalExplTestFixture, check_object_creation) {
  std::unique_ptr<SignalExplorer> signal_explorer = std::make_unique<SignalExplorer>();

  EXPECT_NE(signal_explorer.get(), nullptr) << "ok";
}

TEST_F(SignalExplTestFixture, check_device_by_formats) {
  std::unique_ptr<DataPublisher> publisher = std::make_unique<DataPublisher>("check_device_by_formats_test");
  std::unique_ptr<DataPublisher> publisher2 = std::make_unique<DataPublisher>("check_device_by_formats_test1");
  std::unique_ptr<SignalExplorer> signal_explorer = std::make_unique<SignalExplorer>();

  ASSERT_TRUE(publisher->SetDataSourceInfo("check_device_by_formats_test", 22, 42)) << "Failed to set the data source";
  ASSERT_TRUE(publisher->RegisterDataDescription((const char *)"SDL_CONTENT", (void *)test::SDL_CONTENT.data(),
                                                 test::SDL_CONTENT.size(), true))
      << "Failed to register data description";

  ASSERT_TRUE(publisher2->SetDataSourceInfo("check_device_by_formats_test1", 23, 43))
      << "Failed to set the data source";
  ASSERT_TRUE(publisher2->RegisterDataDescription((const char *)"SDL_CONTENT2", (void *)test::SDL_CONTENT2.data(),
                                                  test::SDL_CONTENT2.size(), true))
      << "Failed to register data description";

  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  auto devices = signal_explorer->getDeviceByFormatType("mts.mta");
  EXPECT_TRUE(std::find(devices.begin(), devices.end(), "check_device_by_formats_test") != devices.end())
      << "Device name wrong";
  EXPECT_TRUE(std::find(devices.begin(), devices.end(), "check_device_by_formats_test1") != devices.end())
      << "Device name wrong";
}

TEST_F(SignalExplTestFixture, multi_thread_access) {
  std::unique_ptr<DataPublisher> publisher = std::make_unique<DataPublisher>("multi_thread_access_test");
  std::unique_ptr<DataPublisher> publisher2 = std::make_unique<DataPublisher>("multi_thread_access_test1");
  std::unique_ptr<SignalExplorer> signal_explorer = std::make_unique<SignalExplorer>();

  ASSERT_TRUE(publisher->SetDataSourceInfo("multi_thread_access_test", 22, 42)) << "Failed to set the data source";
  ASSERT_TRUE(publisher->RegisterDataDescription((const char *)"SDL_CONTENT", (void *)test::SDL_CONTENT.data(),
                                                 test::SDL_CONTENT.size(), true))
      << "Failed to register data description";

  ASSERT_TRUE(publisher2->SetDataSourceInfo("multi_thread_access_test1", 23, 43)) << "Failed to set the data source";
  ASSERT_TRUE(publisher2->RegisterDataDescription((const char *)"SDL_CONTENT2", (void *)test::SDL_CONTENT2.data(),
                                                  test::SDL_CONTENT2.size(), true))
      << "Failed to register data description";

  std::function<bool(const ChildInfo &, const ChildInfo &)> comparator = [](const ChildInfo &left,
                                                                            const ChildInfo &right) {
    return left.name == right.name && left.child_count == right.child_count;
  };

  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  std::atomic_bool keep_running = true;

  auto thread_fun1 = [&signal_explorer, &keep_running]() -> void {
    while (keep_running) {
      EXPECT_NO_THROW(signal_explorer->GetChildByUrl("multi_thread_access_test.AlgoVehCycle.VehDyn.sSigHeader"));
      std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
  };
  auto thread_fun2 = [&signal_explorer, &keep_running]() -> void {
    while (keep_running) {
      EXPECT_NO_THROW(signal_explorer->GetChildByUrl("multi_thread_access_test.AlgoVehCycle.VehDyn.sSigHeader"));
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  };

  std::thread test_thread_1(thread_fun1);
  std::thread test_thread_2(thread_fun2);

  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  keep_running = false;

  test_thread_1.join();
  test_thread_2.join();
}

bool findChildInfoInList(const std::vector<ChildInfo> &out_vec, const ChildInfo &ref) {
  for (auto out : out_vec) {
    if ((ref.name == out.name) && (ref.child_count == out.child_count)) {
      return true;
    }
  }
  return false;
}

bool compare_result(const std::vector<ChildInfo> &ref_vec, const std::vector<ChildInfo> &out_vec) {
  std::function<bool(const ChildInfo &, const ChildInfo &)> comparator = [](const ChildInfo &left,
                                                                            const ChildInfo &right) {
    return left.name == right.name && left.child_count == right.child_count;
  };
  if ((ref_vec.size() == 0) || (out_vec.size() == 0)) {
    return false;
  }
  if (ref_vec.size() == out_vec.size()) {
    return (std::equal(out_vec.begin(), out_vec.end(), ref_vec.begin(), comparator));
  }

  for (auto ref : ref_vec) {
    if (findChildInfoInList(out_vec, ref) == false) {
      return false;
    }
  }
  return true;
}
TEST_F(SignalExplTestFixture, get_child_by_url) {
  std::unique_ptr<DataPublisher> publisher = std::make_unique<DataPublisher>("get_child_by_url_test");
  std::unique_ptr<DataPublisher> publisher2 = std::make_unique<DataPublisher>("get_child_by_url_test1");
  std::unique_ptr<SignalExplorer> signal_explorer = std::make_unique<SignalExplorer>();

  ASSERT_TRUE(publisher->SetDataSourceInfo("get_child_by_url_test", 22, 42)) << "Failed to set the data source";
  ASSERT_TRUE(publisher->RegisterDataDescription((const char *)"SDL_CONTENT", (void *)test::SDL_CONTENT.data(),
                                                 test::SDL_CONTENT.size(), true))
      << "Failed to register data description";

  ASSERT_TRUE(publisher2->SetDataSourceInfo("get_child_by_url_test1", 23, 43)) << "Failed to set the data source";
  ASSERT_TRUE(publisher2->RegisterDataDescription((const char *)"SDL_CONTENT2", (void *)test::SDL_CONTENT2.data(),
                                                  test::SDL_CONTENT2.size(), true))
      << "Failed to register data description";

  std::this_thread::sleep_for(std::chrono::milliseconds(4000));
  std::vector<ChildInfo> childs_info =
      signal_explorer->GetChildByUrl("get_child_by_url_test.AlgoVehCycle.VehDyn.sSigHeader");
  std::vector<ChildInfo> childs_info_ref = test::GenerateSigHeader();

  auto lesThan = [](const ChildInfo &struct1, const ChildInfo &struct2) { return (struct1.name < struct2.name); };

  std::sort(childs_info.begin(), childs_info.end(), lesThan);
  std::sort(childs_info_ref.begin(), childs_info_ref.end(), lesThan);
  EXPECT_TRUE(compare_result(childs_info_ref, childs_info));

  childs_info.clear();
  childs_info_ref.clear();
  auto singal_explorer_output = signal_explorer->GetChildByUrl("get_child_by_url_test.array_root.simple_float_arrays");
  childs_info = test::set_array_members(singal_explorer_output);
  childs_info_ref = test::GenerateArraysOfFloats();
  EXPECT_TRUE(compare_result(childs_info_ref, childs_info));

  childs_info.clear();
  childs_info_ref.clear();
  singal_explorer_output = signal_explorer->GetChildByUrl("get_child_by_url_test.array_root.array_of_array");
  childs_info = test::set_array_members(singal_explorer_output);
  childs_info_ref = test::GenerateArraysOfArrays();
  EXPECT_TRUE(compare_result(childs_info_ref, childs_info));

  childs_info.clear();
  childs_info_ref.clear();
  singal_explorer_output = signal_explorer->GetChildByUrl("get_child_by_url_test.array_root.array_of_structs");
  childs_info = test::set_array_members(singal_explorer_output);
  childs_info_ref = test::GenerateArraysOfStructs();
  EXPECT_TRUE(compare_result(childs_info_ref, childs_info));

  childs_info.clear();
  childs_info_ref.clear();
  singal_explorer_output =
      signal_explorer->GetChildByUrl("get_child_by_url_test.array_root.array_of_array.array_group");
  childs_info = test::set_array_members(singal_explorer_output);
  childs_info_ref = test::GenerateSubgroupOfArrays();
  EXPECT_TRUE(compare_result(childs_info_ref, childs_info));

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  childs_info.clear();
  childs_info_ref.clear();
  childs_info = signal_explorer->GetChildByUrl("");
  childs_info_ref = test::GenerateDevices();
  EXPECT_TRUE(compare_result(childs_info_ref, childs_info));

  childs_info.clear();
  childs_info_ref.clear();
  childs_info = signal_explorer->GetChildByUrl("get_child_by_url1");
  childs_info_ref = test::GenerateViews();
  EXPECT_FALSE(compare_result(childs_info_ref, childs_info));

  childs_info.clear();
  childs_info_ref.clear();
  childs_info = signal_explorer->GetChildByUrl("get_child_by_url_test.AlgoVehCycle");
  childs_info_ref = test::GenerateAlgoVehCycleTopics();
  EXPECT_TRUE(compare_result(childs_info_ref, childs_info));

  childs_info.clear();
  childs_info_ref.clear();
  childs_info = signal_explorer->GetChildByUrl("get_child_by_url_test.array_root");
  childs_info_ref = test::GenerateArrayRootTopics();
  EXPECT_TRUE(compare_result(childs_info_ref, childs_info));
}

TEST_F(SignalExplTestFixture, get_full_url_tree_ethernet) {

  std::unique_ptr<DataPublisher> publisher = std::make_unique<DataPublisher>("get_full_url_tree_ethernet");
  std::unique_ptr<SignalExplorer> signal_explorer = std::make_unique<SignalExplorer>();

  std::string data_source_name = "GetFullUrlTreeEthernet";
  if (!publisher->SetDataSourceInfo(data_source_name.c_str(), 22, 42)) {
    FAIL() << "SetDataSourceInfo failed";
  }

  std::string description_file_name = "FBX4_35up_KW25_13_V5_ETHERNET1_V2_lokal_withoutETS";
  std::string description_file_path = description_file_name + ".xml";
  if (!publisher->RegisterDataDescription(description_file_path.c_str(), true, DESCRIPTION_TYPE_FIBEX)) {
    FAIL() << "RegisterDataDescription failed";
  }

  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  auto url_tree = signal_explorer->GetFullUrlTree();
  EXPECT_EQ(url_tree.size(), 1);

  for (auto &[key, value] : url_tree) {
    if (key != data_source_name) {
      continue;
    }

    auto result = false;
    for (const auto &url : test::url_ref_ethernet) {
      result = false;
      if (std::find(value.begin(), value.end(), url) != value.end()) {
        result = true;
      }

      EXPECT_TRUE(result) << "Signal not found: " << url;
    }
  }
}

TEST_F(SignalExplTestFixture, get_full_url_tree) {
  std::unique_ptr<DataPublisher> publisher1 = std::make_unique<DataPublisher>("get_full_url_tree_test1");
  std::unique_ptr<DataPublisher> publisher2 = std::make_unique<DataPublisher>("get_full_url_tree_test2");
  std::unique_ptr<SignalExplorer> signal_explorer = std::make_unique<SignalExplorer>();

  ASSERT_TRUE(publisher1->SetDataSourceInfo("GetFullUrlTree1", 22, 42)) << "Failed to set the data source";
  ASSERT_TRUE(publisher1->RegisterDataDescription((const char *)"SDL_CONTENT", (void *)test::SDL_CONTENT.data(),
                                                  test::SDL_CONTENT.size(), true))
      << "Failed to register data description";

  ASSERT_TRUE(publisher2->SetDataSourceInfo("GetFullUrlTree2", 23, 43)) << "Failed to set the data source";
  ASSERT_TRUE(publisher2->RegisterDataDescription((const char *)"SDL_CONTENT", (void *)test::SDL_CONTENT.data(),
                                                  test::SDL_CONTENT.size(), true))
      << "Failed to register data description";
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  std::unordered_map<std::string, std::vector<std::string>> url_tree = signal_explorer->GetFullUrlTree();
  for (auto &[key, value] : url_tree) {
    if (key == "GetFullUrlTree1") {
      std::sort(test::url_ref_1.begin(), test::url_ref_1.end());
      std::sort(value.begin(), value.end());
      ASSERT_EQ(test::url_ref_1.size(), value.size());
      EXPECT_TRUE(std::equal(test::url_ref_1.begin(), test::url_ref_1.end(), value.begin()));
    } else if (key == "GetFullUrlTree2") {
      std::sort(test::url_ref_2.begin(), test::url_ref_2.end());
      std::sort(value.begin(), value.end());
      ASSERT_EQ(test::url_ref_1.size(), value.size());
      EXPECT_TRUE(std::equal(test::url_ref_2.begin(), test::url_ref_2.end(), value.begin()));
    }
  }
}

TEST_F(SignalExplTestFixture, get_package_url) {
  std::unique_ptr<DataPublisher> publisher = std::make_unique<DataPublisher>("get_package_url_test");
  std::unique_ptr<SignalExplorer> signal_explorer = std::make_unique<SignalExplorer>();

  ASSERT_TRUE(publisher->SetDataSourceInfo("get_package_url_test", 22, 42)) << "Failed to set the data source";
  ASSERT_TRUE(publisher->RegisterDataDescription((const char *)"SDL_CONTENT", (void *)test::SDL_CONTENT.data(),
                                                 test::SDL_CONTENT.size(), true))
      << "Failed to register data description";
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));
  std::vector<std::string> package_url =
      signal_explorer->GetPackageUrl("get_package_url_test.AlgoVehCycle.VehDyn.sSigHeader");
  std::vector<std::string> package_url_ref = {"get_package_url_test.AlgoVehCycle.VehDyn"};

  bool result = std::equal(package_url.begin(), package_url.end(), package_url_ref.begin());
  EXPECT_TRUE(result);

  package_url.clear();
  package_url_ref.clear();
  package_url = signal_explorer->GetPackageUrl("get_package_url_test.array_root.simple_float_arrays.ten_floats");
  package_url_ref = {"get_package_url_test.array_root.simple_float_arrays"};

  result = std::equal(package_url.begin(), package_url.end(), package_url_ref.begin());
  EXPECT_TRUE(result);

  package_url.clear();
  package_url_ref.clear();
  package_url =
      signal_explorer->GetPackageUrl("get_package_url_test.array_root.array_of_array.array_group.unsigned_char");
  package_url_ref = {"get_package_url_test.array_root.array_of_array"};

  result = std::equal(package_url.begin(), package_url.end(), package_url_ref.begin());
  EXPECT_TRUE(result);

  package_url.clear();
  package_url_ref.clear();
  package_url = signal_explorer->GetPackageUrl("get_package_url_test.array_root.array_of_structs.first_group.big_int");
  package_url_ref = {"get_package_url_test.array_root.array_of_structs"};

  result = std::equal(package_url.begin(), package_url.end(), package_url_ref.begin());
  EXPECT_TRUE(result);

  package_url.clear();
  package_url_ref.clear();
  package_url =
      signal_explorer->GetPackageUrl("get_package_url_test.array_root.array_of_array.array_group.unsigned_char");
  package_url_ref = {"get_package_url_test.array_root.array_of_array"};

  result = std::equal(package_url.begin(), package_url.end(), package_url_ref.begin());
  EXPECT_TRUE(result);

  package_url.clear();
  package_url_ref.clear();
  package_url = signal_explorer->GetPackageUrl("get_package_url_test.array_root.array_of_array.array_group");
  package_url_ref = {"get_package_url_test.array_root.array_of_array"};

  result = std::equal(package_url.begin(), package_url.end(), package_url_ref.begin());
  EXPECT_TRUE(result);

  package_url.clear();
  package_url_ref.clear();
  package_url = signal_explorer->GetPackageUrl("get_package_url_test.array_root.array_of_array");
  package_url_ref = {"get_package_url_test.array_root.array_of_array"};

  for (auto &url : package_url) {
    EXPECT_TRUE(std::find(package_url_ref.begin(), package_url_ref.end(), url) != package_url_ref.end())
        << "url not found for " << url;
  }

  package_url.clear();
  package_url_ref.clear();
  package_url = signal_explorer->GetPackageUrl("get_package_url_test.array_root");
  package_url_ref = {"get_package_url_test.array_root.simple_float_arrays",
                     "get_package_url_test.array_root.array_of_array",
                     "get_package_url_test.array_root.array_of_structs"};
  for (auto &url : package_url) {
    EXPECT_TRUE(std::find(package_url_ref.begin(), package_url_ref.end(), url) != package_url_ref.end())
        << "url not found for " << url;
  }
}

TEST_F(SignalExplTestFixture, DISABLED_searchSignalTree_keyword) {
  std::unique_ptr<DataPublisher> publisher1 = std::make_unique<DataPublisher>("searchSignalTree_keyword1");
  std::unique_ptr<DataPublisher> publisher2 = std::make_unique<DataPublisher>("searchSignalTree_keyword2");
  std::unique_ptr<SignalExplorer> signal_explorer = std::make_unique<SignalExplorer>();

  ASSERT_TRUE(publisher1->SetDataSourceInfo("searchSignalTree_keyword", 22, 42)) << "Failed to set the data source";
  ASSERT_TRUE(publisher1->RegisterDataDescription((const char *)"SDL_CONTENT", (void *)test::SDL_CONTENT.data(),
                                                  test::SDL_CONTENT.size(), true))
      << "Failed to register data description";
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  std::vector<std::string> url_tree = signal_explorer->searchSignalTree("Roller");
  bool result = false;
  if (url_tree.size() != 0) {
    std::sort(test::sigtree_ref_roller.begin(), test::sigtree_ref_roller.end());
    std::sort(url_tree.begin(), url_tree.end());
    result = std::equal(test::sigtree_ref_roller.begin(), test::sigtree_ref_roller.end(), url_tree.begin());
  }
  EXPECT_TRUE(result);

  url_tree = signal_explorer->searchSignalTree("roller");
  result = false;
  if (url_tree.size() != 0) {
    std::sort(test::sigtree_ref_roller.begin(), test::sigtree_ref_roller.end());
    std::sort(url_tree.begin(), url_tree.end());
    result = std::equal(test::sigtree_ref_roller.begin(), test::sigtree_ref_roller.end(), url_tree.begin());
  }
  EXPECT_TRUE(result);

  url_tree = signal_explorer->searchSignalTree("RoLler");
  result = false;
  if (url_tree.size() != 0) {
    std::sort(test::sigtree_ref_roller.begin(), test::sigtree_ref_roller.end());
    std::sort(url_tree.begin(), url_tree.end());
    result = std::equal(test::sigtree_ref_roller.begin(), test::sigtree_ref_roller.end(), url_tree.begin());
  }
  EXPECT_TRUE(result);

  url_tree = signal_explorer->searchSignalTree("RollER");
  result = false;
  if (url_tree.size() != 0) {
    std::sort(test::sigtree_ref_roller.begin(), test::sigtree_ref_roller.end());
    std::sort(url_tree.begin(), url_tree.end());
    result = std::equal(test::sigtree_ref_roller.begin(), test::sigtree_ref_roller.end(), url_tree.begin());
  }
  EXPECT_TRUE(result);
}

TEST_F(SignalExplTestFixture, searchSignalTree_empty_keyword) {
  std::unique_ptr<DataPublisher> publisher1 = std::make_unique<DataPublisher>("searchSignalTree_empty_keyword1");
  std::unique_ptr<DataPublisher> publisher2 = std::make_unique<DataPublisher>("searchSignalTree_empty_keyword2");
  std::unique_ptr<SignalExplorer> signal_explorer = std::make_unique<SignalExplorer>();

  ASSERT_TRUE(publisher1->SetDataSourceInfo("GetFullUrlTree1", 22, 42)) << "Failed to set the data source";
  ASSERT_TRUE(publisher1->RegisterDataDescription((const char *)"SDL_CONTENT", (void *)test::SDL_CONTENT.data(),
                                                  test::SDL_CONTENT.size(), true))
      << "Failed to register data description";
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  std::vector<std::string> url_tree = signal_explorer->searchSignalTree("");
  bool result = false;
  if (url_tree.size() != 0) {
    std::sort(test::url_ref_1.begin(), test::url_ref_1.end());
    std::sort(url_tree.begin(), url_tree.end());
    result = std::equal(test::url_ref_1.begin(), test::url_ref_1.end(), url_tree.begin());
  }
  EXPECT_TRUE(result);
}

TEST_F(SignalExplTestFixture, searchSignalTree_no_result_found) {
  std::unique_ptr<DataPublisher> publisher1 = std::make_unique<DataPublisher>("searchSignalTree_no_result_found1");
  std::unique_ptr<DataPublisher> publisher2 = std::make_unique<DataPublisher>("searchSignalTree_no_result_found2");
  std::unique_ptr<SignalExplorer> signal_explorer = std::make_unique<SignalExplorer>();

  ASSERT_TRUE(publisher1->SetDataSourceInfo("GetFullUrlTree1", 22, 42)) << "Failed to set the data source";
  ASSERT_TRUE(publisher1->RegisterDataDescription((const char *)"SDL_CONTENT", (void *)test::SDL_CONTENT.data(),
                                                  test::SDL_CONTENT.size(), true))
      << "Failed to register data description";
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  std::vector<std::string> url_tree = signal_explorer->searchSignalTree("ASDFGH");
  bool result = false;
  if (url_tree.size() == 0) {
    result = true;
  }
  EXPECT_TRUE(result);
}

TEST_F(SignalExplTestFixture, searchSignalTree_search_for_parent) {
  std::unique_ptr<DataPublisher> publisher1 = std::make_unique<DataPublisher>("searchSignalTree_search_for_parent1");
  std::unique_ptr<DataPublisher> publisher2 = std::make_unique<DataPublisher>("searchSignalTree_search_for_parent2");
  std::unique_ptr<SignalExplorer> signal_explorer = std::make_unique<SignalExplorer>();

  ASSERT_TRUE(publisher1->SetDataSourceInfo("GetFullUrlTree1", 22, 42)) << "Failed to set the data source";
  ASSERT_TRUE(publisher1->RegisterDataDescription((const char *)"SDL_CONTENT", (void *)test::SDL_CONTENT.data(),
                                                  test::SDL_CONTENT.size(), true))
      << "Failed to register data description";
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  std::vector<std::string> url_tree = signal_explorer->searchSignalTree("FullUrl");
  bool result = false;
  if (url_tree.size() != 0) {
    std::sort(test::url_ref_1.begin(), test::url_ref_1.end());
    std::sort(url_tree.begin(), url_tree.end());
    result = std::equal(test::url_ref_1.begin(), test::url_ref_1.end(), url_tree.begin());
  }
  EXPECT_TRUE(result);
}

TEST_F(SignalExplTestFixture, searchSignalTree_search_for_all_children) {
  std::unique_ptr<DataPublisher> publisher1 =
      std::make_unique<DataPublisher>("searchSignalTree_search_for_all_children1");
  std::unique_ptr<DataPublisher> publisher2 =
      std::make_unique<DataPublisher>("searchSignalTree_search_for_all_children2");
  std::unique_ptr<SignalExplorer> signal_explorer = std::make_unique<SignalExplorer>();

  ASSERT_TRUE(publisher1->SetDataSourceInfo("GetFullUrlTree1", 22, 42)) << "Failed to set the data source";
  ASSERT_TRUE(publisher1->RegisterDataDescription((const char *)"SDL_CONTENT", (void *)test::SDL_CONTENT.data(),
                                                  test::SDL_CONTENT.size(), true))
      << "Failed to register data description";
  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  std::vector<std::string> url_tree = signal_explorer->searchSignalTree("Lat");
  bool result = false;
  if (url_tree.size() != 0) {
    std::sort(test::url_ref_Lat.begin(), test::url_ref_Lat.end());
    std::sort(url_tree.begin(), url_tree.end());
    result = std::equal(test::url_ref_Lat.begin(), test::url_ref_Lat.end(), url_tree.begin());
  }
  EXPECT_TRUE(result);

  url_tree = signal_explorer->searchSignalTree("el");
  result = false;
  if (url_tree.size() != 0) {
    std::sort(test::url_ref_el.begin(), test::url_ref_el.end());
    std::sort(url_tree.begin(), url_tree.end());
    result = std::equal(test::url_ref_el.begin(), test::url_ref_el.end(), url_tree.begin());
  }
  EXPECT_TRUE(result);
}

TEST_F(SignalExplTestFixture, generate_sdl) {
  std::unique_ptr<DataPublisher> publisher = std::make_unique<DataPublisher>("generate_sdl");
  std::unique_ptr<SignalExplorer> signal_explorer = std::make_unique<SignalExplorer>();

  ASSERT_TRUE(publisher->SetDataSourceInfo("generate_sdl", 22, 42)) << "Failed to set the data source";
  ASSERT_TRUE(publisher->RegisterDataDescription((const char *)"SDL_CONTENT_SINGLE_GROUP",
                                                 (void *)test::SDL_CONTENT_SINGLE_GROUP.data(),
                                                 test::SDL_CONTENT_SINGLE_GROUP.size(), true))
      << "Failed to register data description";

  std::this_thread::sleep_for(std::chrono::milliseconds(2000));

  auto sdl = signal_explorer->generateSdl("generate_sdl.AlgoVehCycle.VehDyn");
  bool sdls_match = false;

  EXPECT_EQ(sdl.second, std::string(test::SDL_CONTENT_SINGLE_GROUP.data()));
  if (sdl.second.compare(test::SDL_CONTENT_SINGLE_GROUP.data()) == 0) {
    sdls_match = true;
  }

  EXPECT_TRUE(sdls_match);

  auto sdl2 = signal_explorer->generateSdl("invalid.topic.url");
  bool empty_response = false;

  if (sdl2.first == false && sdl2.second.compare("") == 0) {
    empty_response = true;
  }

  EXPECT_TRUE(empty_response);
}

} // namespace explorer
} // namespace udex
} // namespace next

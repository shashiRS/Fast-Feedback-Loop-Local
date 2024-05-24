#include <cip_test_support/gtest_reporting.h>

#include "dll_loader.hpp"

int dummyMain(int, char *[]) { return 0; }

const char *dummyName(void) {
  static std::string test = "";
  return test.data();
}

TEST(next_container_dll_loader, load_dll) {

  std::vector<next::container::NextDll> dlls;
  EXPECT_TRUE(next::container::DllLoader::loadLibrariesInWorkingDirectory(dlls));

  ASSERT_EQ(dlls.size(), 1) << "no dll found";
  EXPECT_EQ(std::string(dlls[0].name), "sicore") << "wrong name of dlls";
}

TEST(next_container_dll_loader, load_dll_filter_no_names) {

  std::vector<next::container::NextDll> dlls;
  dlls.push_back({"asdf", dummyMain, dummyName, nullptr});
  EXPECT_TRUE(next::container::DllLoader::filterLibraries(dlls));
  ASSERT_EQ(dlls.size(), 1) << "no dll found";
  EXPECT_EQ(std::string(dlls[0].name), "asdf") << "wrong name of dlls";
}

TEST(next_container_dll_loader, load_dll_filter_names_duplicated_lib) {

  std::vector<next::container::NextDll> dlls;
  dlls.push_back({"asdf", dummyMain, dummyName, nullptr});
  dlls.push_back({"asdf", dummyMain, dummyName, nullptr});
  EXPECT_FALSE(next::container::DllLoader::filterLibraries(dlls, {"notasdf"}));
  ASSERT_EQ(dlls.size(), 0) << "no dll found";
}

TEST(next_container_dll_loader, load_dll_filter_names_success) {

  std::vector<next::container::NextDll> dlls;
  dlls.push_back({"asdf", dummyMain, dummyName, nullptr});

  EXPECT_TRUE(next::container::DllLoader::filterLibraries(dlls, {"asdf"}));
  ASSERT_EQ(dlls.size(), 1) << "no dll found";
  EXPECT_EQ(std::string(dlls[0].name), "asdf") << "wrong name of dlls";
}

TEST(next_container_dll_loader, load_dll_filter_multiplelibs_and_names_success) {

  std::vector<next::container::NextDll> dlls;
  dlls.push_back({"asdf", dummyMain, dummyName, nullptr});
  dlls.push_back({"asdf1", dummyMain, dummyName, nullptr});
  dlls.push_back({"asdf2", dummyMain, dummyName, nullptr});

  EXPECT_TRUE(next::container::DllLoader::filterLibraries(dlls, {"asdf1", "asdf", "asdf2"}));
  ASSERT_EQ(dlls.size(), 3) << "no dll found";
  EXPECT_EQ(std::string(dlls[0].name), "asdf") << "wrong name of dlls";
  EXPECT_EQ(std::string(dlls[1].name), "asdf1") << "wrong name of dlls";
  EXPECT_EQ(std::string(dlls[2].name), "asdf2") << "wrong name of dlls";
}

#include <cip_test_support/gtest_reporting.h>
#include <gtest/gtest.h>

#include <boost/bind/bind.hpp>
#include <string>
#include <vector>
#include "simulation_environment_cache.h"

namespace next {
namespace databridge {
namespace unit_test {

class SimulationEnvironmentCacheTest : public SimulationEnvironmentCache {
public:
  unsigned int getUpdateTime() { return update_rate_ms_; }
  SimulationEnvironmentCacheTest() { start_flush_ = false; } // disable the thread for testing

  std::list<CacheData> getCache() {
    std::lock_guard lck(cache_mtx_);
    std::list<CacheData> cache(cache_);
    return cache;
  }
  size_t getMaxPackages() { return max_packages_; }
  void publishData(const uint8_t *payload, const size_t payload_size,
                   [[maybe_unused]] const std::vector<size_t> &session_ids = {},
                   [[maybe_unused]] const std::chrono::milliseconds = std::chrono::milliseconds(500),
                   [[maybe_unused]] const bool binary = false) override {
    published_data_.push_back(std::string((const char *)payload, payload_size));
  }
  std::vector<std::string> published_data_;

  void setMaxPackages(size_t max_packages) { this->max_packages_ = max_packages; }
};

TEST(SimulationEnvironmentTest, SetSendingFrequency) {
  SimulationEnvironmentCacheTest simulation_environment_test;
  unsigned int set_time, update_time;
  update_time = 12345;
  simulation_environment_test.setUpdateRateMilliseconds(update_time);
  set_time = simulation_environment_test.getUpdateTime();
  EXPECT_EQ(set_time, update_time);
}

TEST(SimulationEnvironmentTest, AddPackage) {
  SimulationEnvironmentCacheTest simulation_environment;
  if (simulation_environment.getMaxPackages() <= 1u) {
    // Skip test if no packages shall be stored
    EXPECT_TRUE(true);
  } else {
    std::string package1;
    package1 = "Hello World!";

    ASSERT_TRUE(simulation_environment.addPackage(package1.data(), package1.size(), {}, false,
                                                  std::chrono::milliseconds(500), false));

    const auto cache_content = simulation_environment.getCache();

    EXPECT_EQ(cache_content.front().data, package1);
  }
}

TEST(SimulationEnvironmentTest, AddPackage_CallFlushMultipleTimes) {
  SimulationEnvironmentCacheTest simulation_environment;
  std::string package{"Hello World!"};
  std::string package_stitched{"["};
  const uint32_t iterations = 101u;

  for (int i = 0; i < (int)simulation_environment.getMaxPackages() - 1; i++) {
    package_stitched += package;
    package_stitched += ",";
  }
  package_stitched += package;
  package_stitched += "]";

  ASSERT_GT(iterations, simulation_environment.getMaxPackages())
      << "there must be more iterations than the buffer can hold";

  for (uint32_t i = 0u; i < iterations; i++) {
    simulation_environment.addPackage(package.data(), package.size(), {}, false, std::chrono::milliseconds(500), false);
  }
  EXPECT_EQ(simulation_environment.getCache().size(), iterations % simulation_environment.getMaxPackages());
  EXPECT_EQ(simulation_environment.published_data_.size(), iterations / simulation_environment.getMaxPackages());

  const size_t expected_msgs = iterations / simulation_environment.getMaxPackages();
  ASSERT_EQ(simulation_environment.published_data_.size(), expected_msgs);
  for (size_t j = 0u; j < expected_msgs; j++) {
    EXPECT_EQ(simulation_environment.published_data_[j], package_stitched);
  }
}

TEST(SimulationEnvironmentTest, AddPackage_AddTwoPackageSendOne) {
  SimulationEnvironmentCacheTest simulation_environment;
  const std::string package{"Hello World!"};
  const std::string package1{"Hello World!1"};
  const uint32_t num_packages = 2u;

  simulation_environment.setMaxPackages(num_packages);
  EXPECT_EQ(num_packages, simulation_environment.getMaxPackages()) << "the max packages number was not correct";

  simulation_environment.addPackage(package.data(), package.size(), {}, false, std::chrono::milliseconds(500), false);
  simulation_environment.addPackage(package1.data(), package1.size(), {}, false, std::chrono::milliseconds(500), false);
  size_t k_expected_single_message = 1;
  ASSERT_EQ(simulation_environment.published_data_.size(), k_expected_single_message);
  EXPECT_EQ(simulation_environment.published_data_[0], "[" + package + "," + package1 + "]");
}

TEST(SimulationEnvironmentTest, DISABLED_AddPackage_SplitBySessionId) {
  SimulationEnvironmentCacheTest simulation_environment;
  const std::string package{"Hello World!"};
  const std::string package1{"Hello World!1"};
  const uint32_t num_packages = 2u;

  simulation_environment.setMaxPackages(num_packages);
  EXPECT_EQ(num_packages, simulation_environment.getMaxPackages()) << "the max packages number was not correct";

  simulation_environment.addPackage(package.data(), package.size(), {1}, false, std::chrono::milliseconds(500), false);
  simulation_environment.addPackage(package1.data(), package1.size(), {2}, false, std::chrono::milliseconds(500),
                                    false);
  size_t k_expected_single_message = 2;
  simulation_environment.flush();
  ASSERT_EQ(simulation_environment.published_data_.size(), k_expected_single_message);
}

TEST(SimulationEnvironmentTest, DISABLED_AddPackage_SplitByBinary) {
  SimulationEnvironmentCacheTest simulation_environment;
  const std::string package{"Hello World!"};
  const std::string package1{"Hello World!1"};
  const uint32_t num_packages = 2u;

  simulation_environment.setMaxPackages(num_packages);
  EXPECT_EQ(num_packages, simulation_environment.getMaxPackages()) << "the max packages number was not correct";

  simulation_environment.addPackage(package.data(), package.size(), {}, false, std::chrono::milliseconds(500), false);
  simulation_environment.addPackage(package1.data(), package1.size(), {}, false, std::chrono::milliseconds(500), true);
  size_t k_expected_single_message = 2;
  simulation_environment.flush();
  ASSERT_EQ(simulation_environment.published_data_.size(), k_expected_single_message);
}

TEST(SimulationEnvironmentTest, DISABLED_AddPackage_MergedWithBinaryMiddle) {
  SimulationEnvironmentCacheTest simulation_environment;
  const std::string package{"Hello World!"};
  const std::string package1{"Hello World!1"};
  const std::string package2{"Hello World!1"};
  const uint32_t num_packages = 3u;

  simulation_environment.setMaxPackages(num_packages);
  EXPECT_EQ(num_packages, simulation_environment.getMaxPackages()) << "the max packages number was not correct";

  simulation_environment.addPackage(package.data(), package.size(), {}, false, std::chrono::milliseconds(500), false);
  simulation_environment.addPackage(package1.data(), package1.size(), {}, false, std::chrono::milliseconds(500), true);
  simulation_environment.addPackage(package2.data(), package1.size(), {}, false, std::chrono::milliseconds(500), false);
  size_t k_expected_single_message = 2;
  simulation_environment.flush();
  ASSERT_EQ(simulation_environment.published_data_.size(), k_expected_single_message);
}

TEST(SimulationEnvironmentTest, DISABLED_AddPackage_MergedWithDifferentSessionMiddle) {
  SimulationEnvironmentCacheTest simulation_environment;
  const std::string package{"Hello World!"};
  const std::string package1{"Hello World!1"};
  const std::string package2{"Hello World!1"};
  const uint32_t num_packages = 3u;

  simulation_environment.setMaxPackages(num_packages);
  EXPECT_EQ(num_packages, simulation_environment.getMaxPackages()) << "the max packages number was not correct";

  simulation_environment.addPackage(package.data(), package.size(), {1}, false, std::chrono::milliseconds(500), false);
  simulation_environment.addPackage(package1.data(), package1.size(), {2}, false, std::chrono::milliseconds(500),
                                    false);
  simulation_environment.addPackage(package2.data(), package1.size(), {1}, false, std::chrono::milliseconds(500),
                                    false);
  size_t k_expected_single_message = 2;
  simulation_environment.flush();
  ASSERT_EQ(simulation_environment.published_data_.size(), k_expected_single_message);
}

TEST(SimulationEnvironmentTest, DISABLED_AddPackage_MergedWithDifferentSessionAndBinary) {
  SimulationEnvironmentCacheTest simulation_environment;
  const std::string package{"Hello World!"};
  const std::string package1{"Hello World!1"};
  const std::string package2{"Hello World!1"};
  const uint32_t num_packages = 4u;

  simulation_environment.setMaxPackages(num_packages);
  EXPECT_EQ(num_packages, simulation_environment.getMaxPackages()) << "the max packages number was not correct";

  simulation_environment.addPackage(package.data(), package.size(), {1}, false, std::chrono::milliseconds(500), false);
  simulation_environment.addPackage(package1.data(), package1.size(), {2}, false, std::chrono::milliseconds(500),
                                    false);
  simulation_environment.addPackage(package2.data(), package1.size(), {1}, false, std::chrono::milliseconds(500),
                                    false);
  simulation_environment.addPackage(package2.data(), package1.size(), {}, false, std::chrono::milliseconds(500), true);
  size_t k_expected_single_message = 3;
  simulation_environment.flush();
  ASSERT_EQ(simulation_environment.published_data_.size(), k_expected_single_message);
}

TEST(SimulationEnvironmentTest, DISABLED_AddPackage_SplitWithDifferentSessionAndBinary) {
  SimulationEnvironmentCacheTest simulation_environment;
  const std::string package{"Hello World!"};
  const std::string package1{"Hello World!1"};
  const std::string package2{"Hello World!1"};
  const uint32_t num_packages = 2u;

  simulation_environment.setMaxPackages(num_packages);
  EXPECT_EQ(num_packages, simulation_environment.getMaxPackages()) << "the max packages number was not correct";

  simulation_environment.addPackage(package.data(), package.size(), {1}, false, std::chrono::milliseconds(500), false);
  simulation_environment.addPackage(package1.data(), package1.size(), {2}, false, std::chrono::milliseconds(500),
                                    false);
  simulation_environment.addPackage(package2.data(), package1.size(), {1}, false, std::chrono::milliseconds(500),
                                    false);
  simulation_environment.addPackage(package2.data(), package1.size(), {}, false, std::chrono::milliseconds(500), true);
  size_t k_expected_single_message = 4;
  simulation_environment.flush();
  ASSERT_EQ(simulation_environment.published_data_.size(), k_expected_single_message);
}

} // namespace unit_test
} // namespace databridge
} // namespace next

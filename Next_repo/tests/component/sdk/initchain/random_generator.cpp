/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     random_generator.cpp
 *  @brief    Random number generater for testing the init chain
 */

#include "random_generator.hpp"

#include <cassert>
#include <memory>
#include <random>

class RandomGeneratorImpl {
public:
  RandomGeneratorImpl(const long unsigned int seed, const unsigned int min, const unsigned int max)
      : generator_(seed), distribution_{min, max}, min_{min}, max_{max} {}

  unsigned int get() { return distribution_(generator_); }

private:
  std::minstd_rand0 generator_;
  std::uniform_int_distribution<unsigned int> distribution_;
  const unsigned int min_;
  const unsigned int max_;
};

std::unique_ptr<RandomGeneratorImpl> rgi{nullptr};

RandomGenerator::RandomGenerator() noexcept { self_ = this; }

RandomGenerator::~RandomGenerator() noexcept {}

bool RandomGenerator::init(next::sdk::InitChain::ConfigMap const &config, int) noexcept {
  if (!init_done_) {
    new RandomGenerator();
    init_done_ = true;
    long unsigned int seed = 0;
    unsigned int min = 0;
    unsigned int max = 0;
    assert(config.size() == 3);
    if (auto it = config.find(getSeedCfgKey()); it != config.end()) {
      assert(it->second.type() == typeid(decltype(seed)));
      seed = std::any_cast<decltype(seed)>(it->second);
    }
    if (auto it = config.find(getMinCfgKey()); it != config.end()) {
      assert(it->second.type() == typeid(decltype(min)));
      min = std::any_cast<decltype(min)>(it->second);
    }
    if (auto it = config.find(getMaxCfgKey()); it != config.end()) {
      assert(it->second.type() == typeid(decltype(max)));
      max = std::any_cast<decltype(max)>(it->second);
    }

    rgi = std::make_unique<RandomGeneratorImpl>(seed, min, max);
    assert(rgi);

    return true;
  }
  return false;
}

void RandomGenerator::reset(next::sdk::InitChain::ConfigMap const &, int) noexcept {
  if (init_done_) {
    assert(nullptr != self_);
    delete self_;
    init_done_ = false;
    self_ = nullptr;
    rgi.reset(nullptr);
  }
}

std::string RandomGenerator::getSeedCfgKey() noexcept { return std::string{"randomgenerator-seed"}; }

std::string RandomGenerator::getMinCfgKey() noexcept { return std::string{"randomgenerator-min"}; }

std::string RandomGenerator::getMaxCfgKey() noexcept { return std::string{"randomgenerator-max"}; }

unsigned int RandomGenerator::get() {
  assert(init_done_);
  assert(rgi);
  return rgi->get();
}

RandomGenerator *RandomGenerator::self_{nullptr};
bool RandomGenerator::init_done_{false};

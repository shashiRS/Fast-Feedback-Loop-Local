/* ========================== NEXT Project ==================================
 * Copyright (C) A.D.C. Automotive Distance Control Systems GmbH
 * All Rights Reserved
 * COMPONENT: next_sdk
 * ========================== NEXT Project ==================================
 */
/**
 *  @file     random_generator.hpp
 *  @brief    Random number generater for testing the init chain
 */

#include <next/sdk/sdk.hpp>

#ifndef RANDOM_GENERATOR_H_
#define RANDOM_GENERATOR_H_

#if defined(_WIN32)
#if defined(random_generator_EXPORTS)
#define DECLSPEC_random_generator __declspec(dllexport)
#else
#define DECLSPEC_random_generator __declspec(dllimport)
#endif
#else // non windows
#define DECLSPEC_random_generator
#endif

#ifndef DECLSPEC_random_generator
#define DECLSPEC_random_generator
#endif

class DECLSPEC_random_generator RandomGenerator {
public:
  static bool init(next::sdk::InitChain::ConfigMap const &, int) noexcept;
  static void reset(next::sdk::InitChain::ConfigMap const &, int) noexcept;

  static std::string getSeedCfgKey() noexcept;
  static std::string getMinCfgKey() noexcept;
  static std::string getMaxCfgKey() noexcept;

  static unsigned int get();

private:
  RandomGenerator() noexcept;
  RandomGenerator(RandomGenerator const &) = delete;
  RandomGenerator(RandomGenerator &&) = delete;
  RandomGenerator &operator=(RandomGenerator const &) = delete;
  RandomGenerator &operator=(RandomGenerator &&) = delete;
  ~RandomGenerator() noexcept;

  static RandomGenerator *self_;
  static bool init_done_;
};

#endif // RANDOM_GENERATOR_H_

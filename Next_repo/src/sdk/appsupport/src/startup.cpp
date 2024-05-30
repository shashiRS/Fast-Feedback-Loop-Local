#if defined(_WIN32) && defined(_MSC_VER)
// nothing to do so far
#elif defined(__linux__) && defined(__GNUG__)
#include <sys/resource.h>
#else
#error "not supported OS (either Windows (MSVC) or Linux (GCC))"
#endif
#include <next/appsupport/startup.hpp>

#include "ecal/ecal.h"

#include <next/appsupport/cmd_options/cmd_options.hpp>
#include <next/appsupport/config/config_client.hpp>

#include <next/sdk/logger/logger.hpp>
#include <next/sdk/sdk.hpp>

#include "next/appsupport/session_handler/session_client.hpp"

namespace next {
namespace appsupport {

namespace logger = next::sdk::logger;

bool NextStartUp::InitializeNext(std::string instance_name, const std::vector<char *> &args,
                                 bool interprocess_enabled) {

  next::sdk::InitChain::ConfigMap initmap;
  initmap.emplace(next::sdk::EcalSingleton::getInstancenameCfgKey(), instance_name);
  initmap.emplace(logger::getFilenameCfgKey(), instance_name + ".log");
  initmap.emplace(logger::getSentinelProductNameCfgKey(), instance_name);

  next::sdk::logger::register_to_init_chain();
  next::sdk::EcalSingleton::register_to_init_chain();
  next::sdk::profiler::register_to_init_chain();
#ifdef _DEBUG
  initmap.emplace(next_profiling::getProfileLevelCfgKey(), next_profiling::ProfilingLevel::DETAIL);
#else
  initmap.emplace(next_profiling::getProfileLevelCfgKey(), next_profiling::ProfilingLevel::OFF);
#endif

  if (!next::sdk::InitChain::Run(initmap)) {
    std::cerr << "Failed to initialize " << instance_name << std::endl;
    return false;
  }
  eCAL::Util::EnableLoopback(interprocess_enabled);

  ConfigClient::do_init(instance_name);
  auto cmd_options = next::appsupport::CmdOptions(instance_name);
  cmd_options.parseCmd((int)args.size() - 1, (const char **)args.data());
  ConfigClient::finish_init();

#if defined(__linux__)
  struct rlimit limit;
  bool readSucceeded = true;
  if (getrlimit(RLIMIT_NOFILE, &limit) != 0) {
    logger::error(__FILE__, "getrlimit() failed with errno {}", errno);
    readSucceeded = false;
  }

  if (readSucceeded) {
    limit.rlim_cur = limit.rlim_max;
    if (setrlimit(RLIMIT_NOFILE, &limit) != 0) {
      logger::error(__FILE__, "setrlimit() failed with errno {0}", errno);
    }
  }
#endif
  auto session_client = next::appsupport::session::SessionClient(instance_name);

  return true;
}

} // namespace appsupport
} // namespace next

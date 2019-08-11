#include "pch.h"

#include <basalt/Log.h>

#include <memory>
#include <vector>

#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

namespace basalt::log {
namespace {

std::shared_ptr<spdlog::logger> s_coreLogger;
std::shared_ptr<spdlog::logger> s_clientLogger;

} // namespace


void Init() {
  spdlog::set_pattern("%^[%n][%l]: %v%$");

  std::vector<spdlog::sink_ptr> sinks;
  sinks.reserve(2u);

  sinks.push_back(
    std::make_shared<spdlog::sinks::basic_file_sink_st>("log.txt", true)
  );

#if defined(_MSC_VER) && defined(BS_DEBUG_BUILD)
  sinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_st>());
#endif

  s_coreLogger =
      std::make_shared<spdlog::logger>("Basalt", sinks.cbegin(), sinks.cend());
  s_clientLogger =
      std::make_shared<spdlog::logger>("App", sinks.cbegin(), sinks.cend());

#if defined(BS_DEBUG_BUILD)
  s_coreLogger->set_level(spdlog::level::trace);
  s_clientLogger->set_level(spdlog::level::trace);
  s_coreLogger->flush_on(spdlog::level::trace);
  s_clientLogger->flush_on(spdlog::level::trace);
#else
  s_coreLogger->set_level(spdlog::level::info);
  s_clientLogger->set_level(spdlog::level::info);
  s_coreLogger->flush_on(spdlog::level::warn);
  s_clientLogger->flush_on(spdlog::level::warn);
#endif
}


const std::shared_ptr<spdlog::logger>& GetCoreLogger() {
  return s_coreLogger;
}


const std::shared_ptr<spdlog::logger>& GetClientLogger() {
  return s_clientLogger;
}

} // namespace basalt::log

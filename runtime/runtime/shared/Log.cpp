#include "Log.h"

#include <spdlog/spdlog.h> // set_pattern
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

#include <memory> // make_shared
#include <vector>

using std::shared_ptr;
using std::vector;

using spdlog::logger;
using spdlog::sink_ptr;

#ifdef BS_DEBUG_BUILD
using spdlog::level::trace;
#else
using spdlog::level::info;
using spdlog::level::warn;
#endif

namespace basalt::log {
namespace {

shared_ptr<logger> sCoreLogger;
shared_ptr<logger> sClientLogger;

} // namespace

void init() {
  spdlog::set_pattern("%^[%n][%l]: %v%$");

  vector<sink_ptr> sinks;
  sinks.reserve(2u);

  sinks.push_back(
    std::make_shared<spdlog::sinks::basic_file_sink_st>("log.txt", true));

#if defined(_MSC_VER) && defined(BS_DEBUG_BUILD)
  sinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_st>());
#endif

  sCoreLogger = std::make_shared<logger>(
    "Basalt", sinks.cbegin(), sinks.cend());
  sClientLogger = std::make_shared<logger>("App", sinks.cbegin(), sinks.cend());

#if defined(BS_DEBUG_BUILD)
  sCoreLogger->set_level(trace);
  sClientLogger->set_level(trace);
  sCoreLogger->flush_on(trace);
  sClientLogger->flush_on(trace);
#else
  sCoreLogger->set_level(info);
  sClientLogger->set_level(info);
  sCoreLogger->flush_on(warn);
  sClientLogger->flush_on(warn);
#endif
}

auto get_core_logger() -> const shared_ptr<logger>& {
  return sCoreLogger;
}

auto get_client_logger() -> const shared_ptr<logger>& {
  return sClientLogger;
}

} // namespace basalt::log

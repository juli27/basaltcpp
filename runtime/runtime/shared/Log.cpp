#include "Log.h"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/msvc_sink.h>

#include <memory>
#include <vector>

using std::shared_ptr;
using std::vector;

using spdlog::logger;
using spdlog::sink_ptr;

namespace basalt {
namespace {

shared_ptr<logger> sCoreLogger;
shared_ptr<logger> sClientLogger;

constexpr auto LOG_FILE_NAME = "log.txt";
constexpr auto LOGGER_PATTERN = "[%n][%l] %v";

} // namespace

void Log::init() {
  vector<sink_ptr> sinks;
  sinks.reserve(2u);

  sinks.push_back(
    std::make_shared<spdlog::sinks::basic_file_sink_st>(LOG_FILE_NAME)
  );

#ifdef BASALT_DEBUG_BUILD
  sinks.push_back(std::make_shared<spdlog::sinks::msvc_sink_st>());
#endif // BASALT_DEBUG_BUILD

  sCoreLogger = std::make_shared<logger>(
    "Engine", sinks.cbegin(), sinks.cend()
  );
  register_logger(sCoreLogger);
  sClientLogger = std::make_shared<logger>(
    "Client", sinks.cbegin(), sinks.cend()
  );
  register_logger(sClientLogger);

  spdlog::set_pattern(LOGGER_PATTERN);

#ifdef BASALT_DEBUG_BUILD

  spdlog::flush_on(spdlog::level::trace);
  spdlog::set_level(spdlog::level::trace);

#else // BASALT_DEBUG_BUILD

  spdlog::flush_on(spdlog::level::err);
  spdlog::set_level(spdlog::level::info);

#endif // BASALT_DEBUG_BUILD
}

auto Log::get_core_logger() -> const shared_ptr<logger>& {
  return sCoreLogger;
}

auto Log::get_client_logger() -> const shared_ptr<logger>& {
  return sClientLogger;
}

} // namespace basalt

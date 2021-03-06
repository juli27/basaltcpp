#include <basalt/api/shared/log.h>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>

#if BASALT_DEV_BUILD

#include <basalt/api/base/platform.h>

#include <spdlog/sinks/msvc_sink.h>

#endif // BASALT_DEV_BUILD

#include <memory>
#include <vector>

using std::shared_ptr;
using std::vector;

using spdlog::logger;
using spdlog::sink_ptr;
using spdlog::sinks::basic_file_sink_st;

#if BASALT_DEV_BUILD
using spdlog::sinks::msvc_sink_st;
#endif // BASALT_DEV_BUILD

namespace basalt {
namespace {

shared_ptr<logger> sCoreLogger;
shared_ptr<logger> sClientLogger;

constexpr auto LOG_FILE_NAME = SPDLOG_FILENAME_T("log.txt");
constexpr auto LOGGER_PATTERN = "[%n][%l] %v";

} // namespace

void Log::init() {
  vector<sink_ptr> sinks;
  sinks.reserve(2u);
  sinks.emplace_back(std::make_shared<basic_file_sink_st>(LOG_FILE_NAME));

#if BASALT_DEV_BUILD
  if (Platform::is_debugger_attached()) {
    sinks.emplace_back(std::make_shared<msvc_sink_st>());
  }
#endif // BASALT_DEV_BUILD

  for (auto& sink : sinks) {
    sink->set_pattern(LOGGER_PATTERN);
  }

  sCoreLogger =
    std::make_shared<logger>("Engine", sinks.cbegin(), sinks.cend());

#if BASALT_DEBUG_BUILD
  sCoreLogger->flush_on(spdlog::level::trace);
  sCoreLogger->set_level(spdlog::level::trace);
#else // !BASALT_DEBUG_BUILD
  sCoreLogger->flush_on(spdlog::level::err);
#endif // BASALT_DEBUG_BUILD

  sClientLogger = sCoreLogger->clone("Client");
}

void Log::shutdown() {
  sClientLogger.reset();
  sCoreLogger.reset();
  spdlog::shutdown();
}

auto Log::core_logger() noexcept -> logger& {
  return *sCoreLogger;
}

auto Log::client_logger() noexcept -> logger& {
  return *sClientLogger;
}

} // namespace basalt

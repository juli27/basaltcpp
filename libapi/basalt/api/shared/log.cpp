#include <basalt/api/shared/log.h>

#include <spdlog/sinks/basic_file_sink.h>

#include <memory>
#include <string>
#include <string_view>
#include <vector>

#if BASALT_DEV_BUILD

#include <basalt/api/base/platform.h>

#include <spdlog/sinks/msvc_sink.h>

#endif // BASALT_DEV_BUILD

using namespace std::literals;

using std::shared_ptr;
using std::string;
using std::vector;

using spdlog::logger;
using spdlog::sink_ptr;
using spdlog::sinks::basic_file_sink_st;

#if BASALT_DEV_BUILD
using spdlog::sinks::msvc_sink_st;
#endif // BASALT_DEV_BUILD

namespace basalt {
namespace {

auto sCoreLogger = shared_ptr<logger>{};
auto sClientLogger = shared_ptr<logger>{};

constexpr auto LOG_FILE_NAME = SPDLOG_FILENAME_T("log.txt");
constexpr auto LOGGER_PATTERN = "[%n][%l] %v"sv;

} // namespace

auto Log::init() -> void {
  auto sinks = vector<sink_ptr> {};
  sinks.reserve(2u);
  sinks.emplace_back(std::make_shared<basic_file_sink_st>(LOG_FILE_NAME));

#if BASALT_DEV_BUILD
  if (Platform::is_debugger_attached()) {
    sinks.emplace_back(std::make_shared<msvc_sink_st>());
  }
#endif // BASALT_DEV_BUILD

  sCoreLogger =
    std::make_shared<logger>("Engine"s, sinks.cbegin(), sinks.cend());

  sCoreLogger->set_pattern(string{LOGGER_PATTERN});

#if BASALT_DEBUG_BUILD
  sCoreLogger->flush_on(spdlog::level::trace);
  sCoreLogger->set_level(spdlog::level::trace);
#else // !BASALT_DEBUG_BUILD
  sCoreLogger->flush_on(spdlog::level::err);
#endif // BASALT_DEBUG_BUILD

  sClientLogger = sCoreLogger->clone("Client"s);
}

auto Log::shutdown() -> void {
  sClientLogger.reset();
  sCoreLogger.reset();
}

auto Log::core_logger() noexcept -> logger& {
  return *sCoreLogger;
}

auto Log::client_logger() noexcept -> logger& {
  return *sClientLogger;
}

} // namespace basalt

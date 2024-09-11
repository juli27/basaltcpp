#pragma once

#include <spdlog/logger.h>

namespace basalt {

struct Log final {
  static auto init() -> void;
  static auto shutdown() -> void;

  static auto core_logger() noexcept -> spdlog::logger&;
  static auto client_logger() noexcept -> spdlog::logger&;
};

} // namespace basalt

#if BASALT_BUILD

#define BASALT_LOGGER ::basalt::Log::core_logger()

#else // !BASALT_BUILD

#define BASALT_LOGGER ::basalt::Log::client_logger()

#endif // BASALT_BUILD

#if BASALT_DEBUG_BUILD

#define BASALT_LOG_TRACE(...) BASALT_LOGGER.trace(__VA_ARGS__)
#define BASALT_LOG_DEBUG(...) BASALT_LOGGER.debug(__VA_ARGS__)

#else // !BASALT_DEBUG_BUILD

#define BASALT_LOG_TRACE(...)
#define BASALT_LOG_DEBUG(...)

#endif // BASALT_DEBUG_BUILD

#define BASALT_LOG_INFO(...) BASALT_LOGGER.info(__VA_ARGS__)
#define BASALT_LOG_WARN(...) BASALT_LOGGER.warn(__VA_ARGS__)
#define BASALT_LOG_ERROR(...) BASALT_LOGGER.error(__VA_ARGS__)
#define BASALT_LOG_FATAL(...) BASALT_LOGGER.critical(__VA_ARGS__)

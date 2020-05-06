#pragma once
#ifndef BASALT_SHARED_LOG_H
#define BASALT_SHARED_LOG_H

#include <spdlog/logger.h>

namespace basalt {

struct Log final {
  static void init();
  static void shutdown();

  static auto core_logger() -> spdlog::logger*;
  static auto client_logger() -> spdlog::logger*;
};

} // namespace basalt::log

#if BASALT_BUILD

#if BASALT_DEBUG_BUILD

#define BASALT_LOG_TRACE(...) \
  ::basalt::Log::core_logger()->trace(__VA_ARGS__)
#define BASALT_LOG_DEBUG(...) \
  ::basalt::Log::core_logger()->debug(__VA_ARGS__)

#else // !BASALT_DEBUG_BUILD

#define BASALT_LOG_TRACE(...)
#define BASALT_LOG_DEBUG(...)

#endif // BASALT_DEBUG_BUILD

#define BASALT_LOG_INFO(...) ::basalt::Log::core_logger()->info(__VA_ARGS__)
#define BASALT_LOG_WARN(...) ::basalt::Log::core_logger()->warn(__VA_ARGS__)
#define BASALT_LOG_ERROR(...) \
  ::basalt::Log::core_logger()->error(__VA_ARGS__)
#define BASALT_LOG_FATAL(...) \
  ::basalt::Log::core_logger()->critical(__VA_ARGS__)

#else // !BASALT_BUILD

#if BASALT_DEBUG_BUILD

#define BASALT_LOG_TRACE(...) \
  ::basalt::Log::client_logger()->trace(__VA_ARGS__)
#define BASALT_LOG_DEBUG(...) \
  ::basalt::Log::client_logger()->debug(__VA_ARGS__)

#else // !BASALT_DEBUG_BUILD
#define BASALT_LOG_TRACE(...)
#define BASALT_LOG_DEBUG(...)
#endif // BASALT_DEBUG_BUILD

#define BASALT_LOG_INFO(...) \
  ::basalt::Log::client_logger()->info(__VA_ARGS__)
#define BASALT_LOG_WARN(...) \
  ::basalt::Log::client_logger()->warn(__VA_ARGS__)
#define BASALT_LOG_ERROR(...) \
  ::basalt::Log::client_logger()->error(__VA_ARGS__)
#define BASALT_LOG_FATAL(...) \
  ::basalt::Log::client_logger()->critical(__VA_ARGS__)

#endif // BASALT_BUILD

#endif // !BASALT_SHARED_LOG_H

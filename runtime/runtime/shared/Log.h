#pragma once
#ifndef BASALT_SHARED_LOG_H
#define BASALT_SHARED_LOG_H

#include <spdlog/spdlog.h>

#include <memory>

namespace basalt {

struct Log final {
  static void init();

  static auto get_core_logger() -> const std::shared_ptr<spdlog::logger>&;
  static auto get_client_logger() -> const std::shared_ptr<spdlog::logger>&;
};

} // namespace basalt::log

#ifdef BASALT_BUILD

#ifdef BASALT_DEBUG_BUILD

#define BASALT_LOG_TRACE(...) \
  ::basalt::Log::get_core_logger()->trace(__VA_ARGS__)
#define BASALT_LOG_DEBUG(...) \
  ::basalt::Log::get_core_logger()->debug(__VA_ARGS__)

#else // BASALT_DEBUG_BUILD

#define BASALT_LOG_TRACE(...)
#define BASALT_LOG_DEBUG(...)

#endif // BASALT_DEBUG_BUILD

#define BASALT_LOG_INFO(...) ::basalt::Log::get_core_logger()->info(__VA_ARGS__)
#define BASALT_LOG_WARN(...) ::basalt::Log::get_core_logger()->warn(__VA_ARGS__)
#define BASALT_LOG_ERROR(...) \
  ::basalt::Log::get_core_logger()->error(__VA_ARGS__)
#define BASALT_LOG_FATAL(...) \
  ::basalt::Log::get_core_logger()->critical(__VA_ARGS__)

#else // BASALT_BUILD

#ifdef BASALT_DEBUG_BUILD

#define BASALT_LOG_TRACE(...) \
  ::basalt::Log::get_client_logger()->trace(__VA_ARGS__)
#define BASALT_LOG_DEBUG(...) \
  ::basalt::Log::get_client_logger()->debug(__VA_ARGS__)

#else // BASALT_DEBUG_BUILD
#define BASALT_LOG_TRACE(...)
#define BASALT_LOG_DEBUG(...)
#endif // BASALT_DEBUG_BUILD

#define BASALT_LOG_INFO(...) \
  ::basalt::Log::get_client_logger()->info(__VA_ARGS__)
#define BASALT_LOG_WARN(...) \
  ::basalt::Log::get_client_logger()->warn(__VA_ARGS__)
#define BASALT_LOG_ERROR(...) \
  ::basalt::Log::get_client_logger()->error(__VA_ARGS__)
#define BASALT_LOG_FATAL(...) \
  ::basalt::Log::get_client_logger()->critical(__VA_ARGS__)

#endif // BASALT_BUILD

#endif // !BASALT_SHARED_LOG_H

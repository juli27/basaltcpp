#pragma once
#ifndef BS_LOG_H
#define BS_LOG_H

#include <memory>

#include <spdlog/spdlog.h>
// interferes with Key::DELETE
#if defined(DELETE)
#undef DELETE
#endif

// interferes with PlatformEventType::MOUSE_MOVED
#if defined(MOUSE_MOVED)
#undef MOUSE_MOVED
#endif

namespace basalt::log {

void init();
auto get_core_logger() -> const std::shared_ptr<spdlog::logger>&;
auto get_client_logger() -> const std::shared_ptr<spdlog::logger>&;

}  // namespace basalt::log

#ifdef BS_BUILD

#ifdef BS_DEBUG_BUILD
#define BS_TRACE(...) ::basalt::log::get_core_logger()->trace(__VA_ARGS__)
#define BS_DEBUG(...) ::basalt::log::get_core_logger()->debug(__VA_ARGS__)
#else
#define BS_TRACE(...)
#define BS_DEBUG(...)
#endif
#define BS_INFO(...) ::basalt::log::get_core_logger()->info(__VA_ARGS__)
#define BS_WARN(...) ::basalt::log::get_core_logger()->warn(__VA_ARGS__)
#define BS_ERROR(...) ::basalt::log::get_core_logger()->error(__VA_ARGS__)
#define BS_FATAL(...) ::basalt::log::get_core_logger()->critical(__VA_ARGS__)

#else // !BS_BUILD

#ifdef BS_APP_DEBUG_BUILD
#define BS_TRACE(...) ::basalt::log::get_client_logger()->trace(__VA_ARGS__)
#define BS_DEBUG(...) ::basalt::log::get_client_logger()->debug(__VA_ARGS__)
#else
#define BS_TRACE(...)
#define BS_DEBUG(...)
#endif
#define BS_INFO(...) ::basalt::log::get_client_logger()->info(__VA_ARGS__)
#define BS_WARN(...) ::basalt::log::get_client_logger()->warn(__VA_ARGS__)
#define BS_ERROR(...) ::basalt::log::get_client_logger()->error(__VA_ARGS__)
#define BS_FATAL(...) ::basalt::log::get_client_logger()->critical(__VA_ARGS__)

#endif // BS_BUILD

#endif // !BS_LOG_H

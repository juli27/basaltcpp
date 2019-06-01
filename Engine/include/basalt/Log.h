#pragma once
#ifndef BS_LOG_H
#define BS_LOG_H

#include <memory>

#include <spdlog/spdlog.h>
// interferes with KeyCode::DELETE
#if defined(DELETE)
#undef DELETE
#endif

// interferes with PlatformEventType::MOUSE_MOVED
#if defined(MOUSE_MOVED)
#undef MOUSE_MOVED
#endif

namespace basalt {
namespace log {

void Init();

const std::shared_ptr<spdlog::logger>& GetCoreLogger();

const std::shared_ptr<spdlog::logger>& GetClientLogger();

} // namespace log
}  // namespace basalt

#ifdef BS_BUILD

#ifdef BS_DEBUG_BUILD
#define BS_TRACE(...) ::basalt::log::GetCoreLogger()->trace(__VA_ARGS__)
#define BS_DEBUG(...) ::basalt::log::GetCoreLogger()->debug(__VA_ARGS__)
#else
#define BS_TRACE(...)
#define BS_DEBUG(...)
#endif
#define BS_INFO(...) ::basalt::log::GetCoreLogger()->info(__VA_ARGS__)
#define BS_WARN(...) ::basalt::log::GetCoreLogger()->warn(__VA_ARGS__)
#define BS_ERROR(...) ::basalt::log::GetCoreLogger()->error(__VA_ARGS__)
#define BS_FATAL(...) ::basalt::log::GetCoreLogger()->critical(__VA_ARGS__)

#else // !BS_BUILD

#ifdef BS_APP_DEBUG_BUILD
#define BS_TRACE(...) ::basalt::log::GetClientLogger()->trace(__VA_ARGS__)
#define BS_DEBUG(...) ::basalt::log::GetClientLogger()->debug(__VA_ARGS__)
#else
#define BS_TRACE(...)
#define BS_DEBUG(...)
#endif
#define BS_INFO(...) ::basalt::log::GetClientLogger()->info(__VA_ARGS__)
#define BS_WARN(...) ::basalt::log::GetClientLogger()->warn(__VA_ARGS__)
#define BS_ERROR(...) ::basalt::log::GetClientLogger()->error(__VA_ARGS__)
#define BS_FATAL(...) ::basalt::log::GetClientLogger()->critical(__VA_ARGS__)

#endif // BS_BUILD

#endif // !BS_LOG_H

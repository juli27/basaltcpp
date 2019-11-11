#pragma once
#ifndef BASALT_LOG_H
#define BASALT_LOG_H

#include <spdlog/spdlog.h>

#include <memory>

namespace basalt::log {

void init();
auto get_core_logger() -> const std::shared_ptr<spdlog::logger>&;
auto get_client_logger() -> const std::shared_ptr<spdlog::logger>&;

}  // namespace basalt::log

#ifdef BS_BUILD

#ifdef BS_DEBUG_BUILD
#define BASALT_LOG_TRACE(...) ::basalt::log::get_core_logger()->trace(__VA_ARGS__)
#define BASALT_LOG_DEBUG(...) ::basalt::log::get_core_logger()->debug(__VA_ARGS__)
#else
#define BASALT_LOG_TRACE(...)
#define BASALT_LOG_DEBUG(...)
#endif
#define BASALT_LOG_INFO(...) ::basalt::log::get_core_logger()->info(__VA_ARGS__)
#define BASALT_LOG_WARN(...) ::basalt::log::get_core_logger()->warn(__VA_ARGS__)
#define BASALT_LOG_ERROR(...) ::basalt::log::get_core_logger()->error(__VA_ARGS__)
#define BASALT_LOG_FATAL(...) ::basalt::log::get_core_logger()->critical(__VA_ARGS__)

#else // !BS_BUILD

#ifdef BS_APP_DEBUG_BUILD
#define BASALT_LOG_TRACE(...) ::basalt::log::get_client_logger()->trace(__VA_ARGS__)
#define BASALT_LOG_DEBUG(...) ::basalt::log::get_client_logger()->debug(__VA_ARGS__)
#else
#define BASALT_LOG_TRACE(...)
#define BASALT_LOG_DEBUG(...)
#endif
#define BASALT_LOG_INFO(...) ::basalt::log::get_client_logger()->info(__VA_ARGS__)
#define BASALT_LOG_WARN(...) ::basalt::log::get_client_logger()->warn(__VA_ARGS__)
#define BASALT_LOG_ERROR(...) ::basalt::log::get_client_logger()->error(__VA_ARGS__)
#define BASALT_LOG_FATAL(...) ::basalt::log::get_client_logger()->critical(__VA_ARGS__)

#endif // BS_BUILD

#endif // !BASALT_LOG_H

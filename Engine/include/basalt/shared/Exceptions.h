#pragma once
#ifndef BASALT_COMMON_EXCEPTIONS_H
#define BASALT_COMMON_EXCEPTIONS_H

#include <stdexcept>
#include <string>

namespace basalt {

struct ApiNotSupportedException final : std::runtime_error {
  explicit inline ApiNotSupportedException(const char* message);
  explicit inline ApiNotSupportedException(const std::string& message);
};

inline ApiNotSupportedException::ApiNotSupportedException(const char* message)
  : std::runtime_error(message) {}

inline ApiNotSupportedException::ApiNotSupportedException(
  const std::string& message
) : std::runtime_error(message) {}

} // namespace basalt

#endif // !BASALT_COMMON_EXCEPTIONS_H

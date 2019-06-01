#pragma once
#ifndef BS_UTIL_EXCEPTIONS_H
#define BS_UTIL_EXCEPTIONS_H

#include <stdexcept>
#include <string>

namespace basalt {

class ApiNotSupportedException : public std::runtime_error {
public:
  explicit inline ApiNotSupportedException(const char* message);
  explicit inline ApiNotSupportedException(const std::string& message);
};

inline ApiNotSupportedException::ApiNotSupportedException(const char* message)
  : std::runtime_error(message) {}

inline ApiNotSupportedException::ApiNotSupportedException(
  const std::string& message
) : std::runtime_error(message) {}

} // namespace basalt

#endif // !BS_UTIL_EXCEPTIONS_H

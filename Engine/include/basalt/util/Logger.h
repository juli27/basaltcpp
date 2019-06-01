#pragma once
#ifndef BS_UTIL_LOGGER_H
#define BS_UTIL_LOGGER_H

namespace basalt {

class Logger final {
public:
  template<typename... Args>
  void log(const char* msg, const Args&... args);
};

} // namespace basalt

#endif // !BS_UTIL_LOGGER_H

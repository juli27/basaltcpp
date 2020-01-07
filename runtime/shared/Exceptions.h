#pragma once
#ifndef BASALT_SHARED_EXCEPTIONS_H
#define BASALT_SHARED_EXCEPTIONS_H

#include <stdexcept>

namespace basalt {

struct ApiNotSupportedException final : std::runtime_error {
  using std::runtime_error::runtime_error;
};

} // namespace basalt

#endif // !BASALT_SHARED_EXCEPTIONS_H

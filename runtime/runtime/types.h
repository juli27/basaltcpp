#pragma once
#ifndef BASALT_RUNTIME_TYPES_H
#define BASALT_RUNTIME_TYPES_H

#include "shared/Size2D.h"
#include "shared/Types.h"

namespace basalt {

struct UpdateContext final {
  f64 deltaTime {};
  Size2Du16 windowSize {Size2Du16::dont_care()};
};

} // namespace basalt

#endif // BASALT_RUNTIME_TYPES_H

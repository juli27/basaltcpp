#pragma once

#include <basalt/api/types.h>

namespace basalt {

class CanvasPrivate {
public:
  static auto set_mode(Canvas&, CanvasMode) -> void;
};

} // namespace basalt

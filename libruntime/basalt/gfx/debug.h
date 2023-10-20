#pragma once

#include <basalt/gfx/backend/types.h>

namespace basalt::gfx {

struct Debug {
  Debug() = delete;

  static auto update(Composite const&) -> void;
};

} // namespace basalt::gfx

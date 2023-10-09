#pragma once

#include <basalt/gfx/backend/types.h>

namespace basalt::gfx {

struct Debug final {
  Debug() = delete;

  Debug(Debug const&) = delete;
  Debug(Debug&&) = delete;

  ~Debug() = delete;

  auto operator=(Debug const&) -> Debug& = delete;
  auto operator=(Debug&&) -> Debug& = delete;

  static auto update(Composite const&) -> void;
};

} // namespace basalt::gfx

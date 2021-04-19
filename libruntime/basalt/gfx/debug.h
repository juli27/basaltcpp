#pragma once

#include <basalt/gfx/types.h>

#include <basalt/api/gfx/backend/types.h>

namespace basalt::gfx {

struct Debug final {
  Debug() = delete;

  Debug(const Debug&) = delete;
  Debug(Debug&&) = delete;

  ~Debug() = delete;

  auto operator=(const Debug&) -> Debug& = delete;
  auto operator=(Debug &&) -> Debug& = delete;

  static void update(const AdapterInfo& currentAdapter, const Composite&);
};

} // namespace basalt::gfx

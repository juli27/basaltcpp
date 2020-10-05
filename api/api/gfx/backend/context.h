#pragma once

#include "api/shared/size2d.h"

namespace basalt::gfx {

struct Composite;
struct Device;

// A gfx context is directly tied to a window. If another gfx context is
// required (e.g. change of rendering API), the window must be recreated.
struct Context {
  Context(const Context&) = delete;
  Context(Context&&) = delete;

  virtual ~Context() noexcept = default;

  auto operator=(const Context&) -> Context& = delete;
  auto operator=(Context &&) -> Context& = delete;

  [[nodiscard]] virtual auto surface_size() const noexcept -> Size2Du16 = 0;

  virtual void resize(Size2Du16) = 0;

  [[nodiscard]] virtual auto device() const noexcept -> Device& = 0;

  virtual void submit(const Composite&) = 0;
  virtual void present() = 0;

protected:
  Context() noexcept = default;
};

} // namespace basalt::gfx

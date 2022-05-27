#pragma once

#include <basalt/gfx/backend/types.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/size2d.h>
#include <basalt/api/shared/types.h>

namespace basalt::gfx {

// A gfx context is directly tied to a window. If another gfx context is
// required (e.g. change of rendering API), the window must be recreated.
struct Context {
  Context(const Context&) = delete;
  Context(Context&&) = delete;

  virtual ~Context() noexcept = default;

  auto operator=(const Context&) -> Context& = delete;
  auto operator=(Context&&) -> Context& = delete;

  [[nodiscard]] virtual auto surface_size() const noexcept -> Size2Du16 = 0;

  [[nodiscard]] virtual auto get_status() const noexcept -> ContextStatus = 0;

  virtual auto reset() -> void = 0;

  struct ResetDesc final {
    // ignored when exclusive == true
    Size2Du16 windowBackBufferSize {};
    // ignored when exclusive == false
    DisplayMode exclusiveDisplayMode;
    ImageFormat renderTargetFormat {ImageFormat::Unknown};
    MultiSampleCount sampleCount {MultiSampleCount::One};
    bool exclusive {false};
  };

  virtual auto reset(const ResetDesc&) -> void = 0;

  [[nodiscard]] virtual auto device() const noexcept -> Device& = 0;

  virtual void submit(const Composite&) = 0;
  [[nodiscard]] virtual auto present() -> PresentResult = 0;

protected:
  Context() noexcept = default;
};

} // namespace basalt::gfx

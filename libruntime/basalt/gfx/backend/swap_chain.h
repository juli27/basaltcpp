#pragma once

#include <basalt/gfx/backend/types.h>

#include <basalt/api/gfx/types.h>

#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/size2d.h>
#include <basalt/api/shared/types.h>

namespace basalt::gfx {

// A swap chain is directly tied to a window. If another swap chain is
// required (e.g. change of rendering API), the window must be recreated.
class SwapChain {
public:
  SwapChain(SwapChain const&) = delete;
  SwapChain(SwapChain&&) = delete;

  virtual ~SwapChain() noexcept = default;

  auto operator=(SwapChain const&) -> SwapChain& = delete;
  auto operator=(SwapChain&&) -> SwapChain& = delete;

  struct Info {
    Size2Du16 backBufferSize{};
    u32 refreshRate{};
    ImageFormat renderTargetFormat{ImageFormat::Unknown};
    MultiSampleCount sampleCount{MultiSampleCount::One};
    bool exclusive{false};
  };

  [[nodiscard]] virtual auto device() const noexcept -> DevicePtr = 0;
  [[nodiscard]] virtual auto get_info() const noexcept -> Info = 0;

  struct ResetDesc final {
    // ignored when exclusive == true
    Size2Du16 windowBackBufferSize{};
    // ignored when exclusive == false
    DisplayMode exclusiveDisplayMode;
    ImageFormat renderTargetFormat{ImageFormat::Unknown};
    MultiSampleCount sampleCount{MultiSampleCount::One};
    bool exclusive{false};
  };

  virtual auto reset(ResetDesc const&) -> void = 0;

  [[nodiscard]] virtual auto present() -> PresentResult = 0;

protected:
  SwapChain() noexcept = default;
};

} // namespace basalt::gfx

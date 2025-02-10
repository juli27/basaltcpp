#pragma once

#include "types.h"

#include <basalt/api/gfx/backend/adapter.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/size2d.h>
#include <basalt/api/shared/types.h>

#include <basalt/api/base/functional.h>
#include <basalt/api/base/types.h>

#include <optional>
#include <variant>

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

  struct ExclusiveModeInfo {
    DisplayMode displayMode;
  };

  struct SharedModeInfo {
    Size2Du16 size;
  };

  using ModeInfo = std::variant<SharedModeInfo, ExclusiveModeInfo>;

  struct Info {
    ModeInfo modeInfo;
    ImageFormat colorFormat;
    std::optional<ImageFormat> depthStencilFormat;
    MultiSampleCount sampleCount;

    [[nodiscard]]
    auto is_exclusive() const -> bool;

    [[nodiscard]]
    auto size() const -> Size2Du16;
  };

  [[nodiscard]]
  virtual auto device() const noexcept -> DevicePtr = 0;

  [[nodiscard]]
  virtual auto get_info() const noexcept -> Info = 0;

  virtual auto reset(Info const&) -> void = 0;

  [[nodiscard]]
  virtual auto present() -> PresentResult = 0;

protected:
  SwapChain() noexcept = default;
};

} // namespace basalt::gfx

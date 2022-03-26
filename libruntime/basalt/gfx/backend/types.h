#pragma once

#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/base/types.h>

#include <memory>
#include <vector>

namespace basalt::gfx {

using Composite = std::vector<CommandList>;

struct Context;
using ContextPtr = std::unique_ptr<Context>;

enum class ContextStatus : u8 {
  Ok,
  Error,
  DeviceLost,
  ResetNeeded,
};

enum class PresentResult : u8 {
  Ok,
  DeviceLost,
};

} // namespace basalt::gfx

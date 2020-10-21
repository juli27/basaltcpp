#pragma once

#include "extension.h"

#include "api/gfx/backend/types.h"

#include <string_view>

namespace basalt::gfx::ext {

struct XModelSupport : ExtensionT<ExtensionId::XModelSupport> {
  virtual auto load(std::string_view filePath) -> XModelHandle = 0;

protected:
  XModelSupport() = default;
};

struct CommandDrawXModel final : CommandT<CommandType::ExtDrawXModel> {
  XModelHandle handle;

  constexpr explicit CommandDrawXModel(const XModelHandle handle_) noexcept
    : handle {handle_} {
  }
};

static_assert(sizeof(CommandDrawXModel) == 8);

} // namespace basalt::gfx::ext
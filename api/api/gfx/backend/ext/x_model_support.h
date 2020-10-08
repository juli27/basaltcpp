#pragma once

#include "extension.h"
#include "types.h"

#include "api/gfx/backend/types.h"

#include <string_view>

namespace basalt::gfx::ext {

struct XModelSupport : ExtensionT<ExtensionId::XModelSupport> {
  virtual auto load(std::string_view filePath) -> ModelHandle = 0;

protected:
  XModelSupport() = default;
};

struct CommandDrawXModel final : CommandT<CommandType::ExtDrawXModel> {
  ModelHandle handle;

  constexpr explicit CommandDrawXModel(const ModelHandle handle_) noexcept
    : handle {handle_} {
  }
};

static_assert(sizeof(CommandDrawXModel) == 8);

} // namespace basalt::gfx::ext
#pragma once

#include <basalt/api/gfx/backend/ext/extension.h>

#include <basalt/api/gfx/backend/types.h>

#include <string_view>

namespace basalt::gfx::ext {

struct XModelSupport : ExtensionT<ExtensionId::XModelSupport> {
  virtual auto load(std::string_view filePath) -> XModel = 0;
};

struct CommandDrawXModel final : CommandT<CommandType::ExtDrawXModel> {
  XModel handle;

  constexpr explicit CommandDrawXModel(const XModel handle_) noexcept
    : handle {handle_} {
  }
};

static_assert(sizeof(CommandDrawXModel) == 8);

} // namespace basalt::gfx::ext
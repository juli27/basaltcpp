#pragma once

#include "extension.h"

#include "api/gfx/backend/types.h"

#include "api/shared/handle.h"

#include <string_view>

namespace basalt::gfx::ext {

namespace detail {

struct ModelTag;

} // namespace detail

using ModelHandle = Handle<detail::ModelTag>;

struct XModelSupport : Extension {
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
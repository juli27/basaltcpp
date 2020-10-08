#pragma once

#include "device.h"
#include "ext/types.h"

#include <memory>
#include <optional>
#include <type_traits>

namespace basalt::gfx {

template <typename T>
auto query_device_extension(Device& device)
  -> std::optional<std::shared_ptr<T>> {
  static_assert(std::is_base_of_v<ext::Extension, T>);

  auto maybeExt = device.query_extension(T::ID);
  if (!maybeExt) {
    return std::nullopt;
  }

  return std::static_pointer_cast<T>(maybeExt.value());
}

} // namespace basalt::gfx

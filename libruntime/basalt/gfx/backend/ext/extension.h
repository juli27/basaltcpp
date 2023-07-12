#pragma once

#include <basalt/gfx/backend/ext/types.h>

namespace basalt::gfx::ext {

struct Extension {
  Extension(const Extension&) = delete;
  Extension(Extension&&) = delete;

  virtual ~Extension() noexcept = default;

  auto operator=(const Extension&) -> Extension& = delete;
  auto operator=(Extension&&) -> Extension& = delete;

protected:
  Extension() noexcept = default;
};

template <ExtensionId Id>
class ExtensionT : public Extension {
public:
  static constexpr ExtensionId ID = Id;
};

} // namespace basalt::gfx::ext
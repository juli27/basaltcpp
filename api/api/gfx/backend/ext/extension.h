#pragma once

namespace basalt::gfx::ext {

struct Extension {
  Extension(const Extension&) = delete;
  Extension(Extension&&) = delete;

  virtual ~Extension() = default;

  auto operator=(const Extension&) -> Extension& = delete;
  auto operator=(Extension &&) -> Extension& = delete;

protected:
  Extension() = default;
};

} // namespace basalt::gfx::ext

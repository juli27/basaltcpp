#pragma once

#include "command_list.h"

#include "api/shared/color.h"

#include <vector>

namespace basalt::gfx {

struct Composite final {
  explicit Composite(const Color& background) noexcept;

  Composite(const Composite&) = delete;
  Composite(Composite&&) = default;

  ~Composite() noexcept = default;

  auto operator=(const Composite&) -> Composite& = delete;
  auto operator=(Composite &&) -> Composite& = default;

  [[nodiscard]] auto background() const noexcept -> const Color&;

  [[nodiscard]] auto parts() const noexcept -> const std::vector<CommandList>&;

  void add_part(CommandList);

private:
  Color mBackground;
  std::vector<CommandList> mParts;
};

} // namespace basalt::gfx

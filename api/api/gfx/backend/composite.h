#pragma once

#include "command_list.h"

#include "api/shared/color.h"

#include <vector>

namespace basalt::gfx {

struct Composite final {
  Composite() noexcept = default;

  Composite(const Composite&) = delete;
  Composite(Composite&&) = default;

  ~Composite() noexcept = default;

  auto operator=(const Composite&) -> Composite& = delete;
  auto operator=(Composite &&) -> Composite& = default;

  [[nodiscard]] auto background() const noexcept -> const Color&;
  void set_background(const Color&) noexcept;

  [[nodiscard]] auto parts() const noexcept -> const std::vector<CommandList>&;

  void add_part(CommandList);

private:
  Color mBackground {Colors::BLACK};
  std::vector<CommandList> mParts;
};

} // namespace basalt::gfx

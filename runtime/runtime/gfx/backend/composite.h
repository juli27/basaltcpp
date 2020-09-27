#pragma once

#include "command_list.h"

#include "runtime/shared/color.h"

#include <vector>

namespace basalt::gfx {

struct Composite final {
  Composite() = default;

  explicit Composite(const Color& background);

  Composite(const Composite&) = delete;
  Composite(Composite&&) = default;

  ~Composite() = default;

  auto operator=(const Composite&) -> Composite& = delete;
  auto operator=(Composite&&) -> Composite& = default;

  [[nodiscard]]
  auto background() const -> const Color&;

  [[nodiscard]]
  auto parts() const -> const std::vector<CommandList>&;

  void add_part(CommandList);

private:
  Color mBackground {};
  std::vector<CommandList> mParts {};
};

} // namespace basalt::gfx

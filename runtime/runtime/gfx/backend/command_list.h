#pragma once

#include "types.h"

#include <runtime/shared/color.h>

#include <memory>
#include <vector>

namespace basalt {

struct DirectionalLight;
struct Mat4;
using Mat4f32 = Mat4;

namespace gfx {

enum class TransformType : u8;

struct Command;
struct CommandLegacy;

using CommandPtr = std::unique_ptr<Command>;

// associates commands with their common transform (camera) and
// defines defaults for render state flags (lighting on/off, ...)
// (TODO: can every state flag be overridden by each command
//        or only some, or none)
struct CommandList final {
  CommandList() = default;
  explicit CommandList(const Color& clearColor);

  CommandList(const CommandList&) = delete;
  CommandList(CommandList&&) = default;

  ~CommandList();

  auto operator=(const CommandList&) -> CommandList& = delete;
  auto operator=(CommandList&&) -> CommandList& = default;

  [[nodiscard]]
  auto commands() const -> const std::vector<CommandPtr>&;

  [[nodiscard]]
  auto clear_color() const -> const Color&;

  void add(const CommandLegacy&);

  void set_ambient_light(const Color&);
  void set_directional_lights(const std::vector<DirectionalLight>&);
  void set_transform(TransformType, const Mat4f32&);

private:
  std::vector<CommandPtr> mCommands {};
  // TODO: drawable need to be able to clear their area of the draw target
  Color mClearColor {};
};

} // namespace gfx
} // namespace basalt

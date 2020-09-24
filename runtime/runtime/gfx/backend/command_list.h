#pragma once

#include <runtime/math/mat4.h>
#include <runtime/shared/color.h>

#include <memory>
#include <vector>

namespace basalt {

struct DirectionalLight;

namespace gfx {

struct Command;
struct CommandLegacy;

using CommandPtr = std::unique_ptr<Command>;

// associates commands with their common transform (camera) and
// defines defaults for render state flags (lighting on/off, ...)
// (TODO: can every state flag be overridden by each command
//        or only some, or none)
struct CommandList final {
  CommandList() = default;
  CommandList(
    const Mat4f32& view, const Mat4f32& projection
  , const Color& clearColor);

  CommandList(const CommandList&) = delete;
  CommandList(CommandList&&) = default;

  ~CommandList();

  auto operator=(const CommandList&) -> CommandList& = delete;
  auto operator=(CommandList&&) -> CommandList& = default;

  [[nodiscard]]
  auto commands() const -> const std::vector<CommandPtr>&;

  [[nodiscard]]
  auto view() const -> const Mat4f32&;

  [[nodiscard]]
  auto projection() const -> const Mat4f32&;

  [[nodiscard]]
  auto clear_color() const -> const Color&;

  void add(const CommandLegacy&);

  void set_ambient_light(const Color&);
  void set_directional_lights(const std::vector<DirectionalLight>&);

private:
  std::vector<CommandPtr> mCommands {};
  Mat4f32 mView {Mat4f32::identity()};
  Mat4f32 mProjection {Mat4f32::identity()};
  // TODO: drawable need to be able to clear their area of the draw target
  Color mClearColor {};
};

} // namespace gfx
} // namespace basalt

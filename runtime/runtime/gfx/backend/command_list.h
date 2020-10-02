#pragma once

#include "types.h"

#include <memory>
#include <type_traits>
#include <vector>

namespace basalt {

struct Color;
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

  CommandList(const CommandList&) = delete;
  CommandList(CommandList&&) = default;

  ~CommandList() = default;

  auto operator=(const CommandList&) -> CommandList& = delete;
  auto operator=(CommandList &&) -> CommandList& = default;

  [[nodiscard]] auto commands() const noexcept
    -> const std::vector<CommandPtr>&;

  void add(const CommandLegacy&);

  void set_ambient_light(const Color&);
  void set_directional_lights(const std::vector<DirectionalLight>&);
  void set_transform(TransformType, const Mat4f32&);
  void set_render_state(RenderState, u32 value);

  template <typename T, typename... Args>
  void add(Args&&... args) {
    static_assert(std::is_base_of_v<Command, T>,
                  "CommandLists only accept commands derived from Command");
    static_assert(std::is_trivially_destructible_v<T>);

    mCommands.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
  }

private:
  std::vector<CommandPtr> mCommands {};
};

} // namespace gfx
} // namespace basalt

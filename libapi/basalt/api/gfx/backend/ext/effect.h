#pragma once

#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <gsl/zstring>

#include <optional>
#include <string>

namespace basalt::gfx::ext {

class Effect {
public:
  Effect(Effect const&) = default;
  Effect(Effect&&) = default;

  virtual ~Effect() noexcept = default;

  auto operator=(Effect const&) -> Effect& = default;
  auto operator=(Effect&&) -> Effect& = default;

  [[nodiscard]] virtual auto get_num_techniques() const -> u32 = 0;
  [[nodiscard]] virtual auto get_technique(u32 index)
    -> EffectTechniqueHandle = 0;
  [[nodiscard]] virtual auto
    get_technique_name(EffectTechniqueHandle = {}) const -> gsl::czstring = 0;
  [[nodiscard]] virtual auto
    get_technique_num_passes(EffectTechniqueHandle = {}) const -> u32 = 0;
  // TODO: return validation result instead of a boolean
  [[nodiscard]] virtual auto validate_technique(EffectTechniqueHandle) const
    -> bool = 0;
  virtual auto set_technique(EffectTechniqueHandle) const -> bool = 0;

  [[nodiscard]] virtual auto get_string(gsl::czstring parameter) const
    -> std::optional<std::string> = 0;
  virtual auto set_string(gsl::czstring parameter,
                          gsl::czstring value) const noexcept -> bool = 0;

  virtual auto set_texture(gsl::czstring parameter, TextureHandle) const
    -> bool = 0;

protected:
  Effect() noexcept = default;
};

class EffectCommandEncoder final {
public:
  static auto begin_effect(CommandList&, EffectId) -> void;
  static auto end_effect(CommandList&) -> void;
  static auto begin_effect_pass(CommandList&, u32 passIndex) -> void;
  static auto end_effect_pass(CommandList&) -> void;
};

} // namespace basalt::gfx::ext

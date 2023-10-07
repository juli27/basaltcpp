#pragma once

#include <basalt/gfx/backend/ext/effect.h>

#include <basalt/gfx/backend/types.h>
#include <basalt/gfx/backend/d3d9/d3d9_custom.h>
#include <basalt/gfx/backend/d3d9/types.h>

#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/shared/handle_pool.h>

namespace basalt::gfx::ext {

class D3D9XEffects final : public Effects {
public:
  static auto create(D3D9Device*) -> D3D9XEffectsPtr;

  auto execute(CommandBeginEffect const&) -> void;
  auto execute(CommandEndEffect const&) -> void;
  auto execute(CommandBeginEffectPass const&) -> void;
  auto execute(CommandEndEffectPass const&) -> void;

  auto on_device_lost() const -> void;
  auto on_device_reset() const -> void;

  [[nodiscard]] auto compile(std::filesystem::path const&)
    -> CompileResult override;

  auto destroy(EffectId) noexcept -> void override;
  [[nodiscard]] auto get(EffectId) -> Effect& override;

  explicit D3D9XEffects(D3D9Device*);

private:
  D3D9Device* mDevice;
  HandlePool<D3D9XEffect, EffectId> mEffects;
  EffectId mActiveEffect {};
};

class D3D9XEffect final : public Effect {
public:
  explicit D3D9XEffect(ID3DXEffectPtr, D3D9Device* device);

  auto begin() const -> void;
  auto end() const -> void;
  auto begin_pass(u32 passIndex) const -> void;
  auto end_pass() const -> void;

  auto on_device_lost() const -> void;
  auto on_device_reset() const -> void;

  [[nodiscard]] auto get_num_techniques() const -> u32 override;
  [[nodiscard]] auto get_technique(u32 index) -> EffectTechniqueHandle override;
  [[nodiscard]] auto get_technique_name(EffectTechniqueHandle) const
    -> gsl::czstring override;
  [[nodiscard]] auto get_technique_num_passes(EffectTechniqueHandle) const
    -> u32 override;
  [[nodiscard]] auto validate_technique(EffectTechniqueHandle) const
    -> bool override;
  auto set_technique(EffectTechniqueHandle) const -> bool override;

  [[nodiscard]] auto get_string(gsl::czstring parameter) const
    -> std::optional<std::string> override;
  auto set_string(gsl::czstring parameter, gsl::czstring value) const noexcept
    -> bool override;

  auto set_texture(gsl::czstring parameter, Texture) const -> bool override;

private:
  ID3DXEffectPtr mEffect;
  D3D9Device* mDevice;
  HandlePool<D3DXHANDLE, EffectTechniqueHandle> mTechniques;

  [[nodiscard]] auto get_technique_desc(EffectTechniqueHandle = {}) const
    -> D3DXTECHNIQUE_DESC;
};

} // namespace basalt::gfx::ext

#include "effect.h"

#include "d3d9_error.h"
#include "device.h"

#include <basalt/gfx/backend/commands.h>

#include <basalt/api/base/log.h>

#include <gsl/zstring>

#include <algorithm>
#include <stdexcept>
#include <utility>

namespace basalt::gfx::ext {

using gsl::czstring;
using std::nullopt;
using std::optional;
using std::string;
using std::filesystem::path;

auto D3D9XEffects::create(D3D9DevicePtr device) -> D3D9XEffectsPtr {
  return std::make_shared<D3D9XEffects>(std::move(device));
}

auto D3D9XEffects::execute(CommandBeginEffect const& cmd) -> void {
  mActiveEffect = cmd.effect;

  auto const& effect = mEffects[mActiveEffect];
  effect.begin();
}

auto D3D9XEffects::execute(CommandEndEffect const&) -> void {
  auto const& effect = mEffects[mActiveEffect];
  effect.end();

  mActiveEffect = nullhdl;
}

auto D3D9XEffects::execute(CommandBeginEffectPass const& cmd) -> void {
  auto const& effect = mEffects[mActiveEffect];
  effect.begin_pass(cmd.passIndex);
}

auto D3D9XEffects::execute(CommandEndEffectPass const&) -> void {
  auto const& effect{mEffects[mActiveEffect]};
  effect.end_pass();
}

auto D3D9XEffects::on_device_lost() const -> void {
  for (auto const effectId : mEffects) {
    mEffects[effectId].on_device_lost();
  }
}

auto D3D9XEffects::on_device_reset() const -> void {
  for (auto const effectId : mEffects) {
    mEffects[effectId].on_device_reset();
  }
}

auto D3D9XEffects::compile(path const& filePath) -> CompileResult {
  auto effect = ID3DXEffectPtr{};
  auto compilationErrors = ID3DXBufferPtr{};
  if (FAILED(D3DXCreateEffectFromFileW(mDevice->device().Get(),
                                       filePath.c_str(), nullptr, nullptr, 0,
                                       nullptr, &effect, &compilationErrors))) {
    if (compilationErrors) {
      return string{
        static_cast<char const*>(compilationErrors->GetBufferPointer())};
    }

    throw std::runtime_error{"failed to load effect file"};
  }

  return mEffects.emplace(std::move(effect), mDevice);
}

auto D3D9XEffects::destroy(EffectId const id) noexcept -> void {
  mEffects.destroy(id);
}

auto D3D9XEffects::get(EffectId const id) -> Effect& {
  return mEffects[id];
}

D3D9XEffects::D3D9XEffects(D3D9DevicePtr device) : mDevice{std::move(device)} {
}

D3D9XEffect::D3D9XEffect(ID3DXEffectPtr effect, D3D9DevicePtr device)
  : mEffect{std::move(effect)}
  , mDevice{std::move(device)} {
}

auto D3D9XEffect::begin() const -> void {
  auto unused = UINT{};
  D3D9CHECK(mEffect->Begin(&unused, 0));
}

auto D3D9XEffect::end() const -> void {
  D3D9CHECK(mEffect->End());
}

auto D3D9XEffect::begin_pass(u32 const passIndex) const -> void {
  D3D9CHECK(mEffect->BeginPass(passIndex));
}

auto D3D9XEffect::end_pass() const -> void {
  D3D9CHECK(mEffect->EndPass());
}

auto D3D9XEffect::on_device_lost() const -> void {
  D3D9CHECK(mEffect->OnLostDevice());
}

auto D3D9XEffect::on_device_reset() const -> void {
  D3D9CHECK(mEffect->OnResetDevice());
}

auto D3D9XEffect::get_num_techniques() const -> u32 {
  auto desc = D3DXEFFECT_DESC{};
  D3D9CHECK(mEffect->GetDesc(&desc));

  return desc.Techniques;
}

auto D3D9XEffect::get_technique(u32 const index) -> EffectTechniqueHandle {
  auto const technique = mEffect->GetTechnique(index);
  if (!technique) {
    return nullhdl;
  }

  if (auto const it = std::find_if(mTechniques.begin(), mTechniques.end(),
                                   [&](EffectTechniqueHandle const h) -> bool {
                                     return mTechniques[h] == technique;
                                   });
      it != mTechniques.end()) {
    return *it;
  }

  return mTechniques.emplace(technique);
}

auto D3D9XEffect::get_technique_name(EffectTechniqueHandle const handle) const
  -> czstring {
  auto const desc = get_technique_desc(handle);

  return desc.Name;
}

auto D3D9XEffect::get_technique_num_passes(
  EffectTechniqueHandle const handle) const -> u32 {
  auto const desc = get_technique_desc(handle);

  return desc.Passes;
}

auto D3D9XEffect::validate_technique(EffectTechniqueHandle const handle) const
  -> bool {
  auto const technique = mTechniques[handle];

  return SUCCEEDED(mEffect->ValidateTechnique(technique));
}

auto D3D9XEffect::set_technique(EffectTechniqueHandle const handle) const
  -> bool {
  auto const technique = mTechniques[handle];

  return SUCCEEDED(mEffect->SetTechnique(technique));
}

auto D3D9XEffect::get_string(czstring const parameter) const
  -> optional<string> {
  auto value = czstring{};

  if (SUCCEEDED(mEffect->GetString(parameter, &value))) {
    return string{value};
  }

  return nullopt;
}

auto D3D9XEffect::set_string(czstring const parameter,
                             czstring const value) const noexcept -> bool {
  return SUCCEEDED(mEffect->SetString(parameter, value));
}

auto D3D9XEffect::set_texture(czstring const parameter,
                              TextureHandle const id) const -> bool {
  auto const texture = id ? mDevice->get_d3d9(id) : IDirect3DBaseTexture9Ptr{};

  return SUCCEEDED(mEffect->SetTexture(parameter, texture.Get()));
}

auto D3D9XEffect::get_technique_desc(EffectTechniqueHandle const handle) const
  -> D3DXTECHNIQUE_DESC {
  auto const technique = handle ? mTechniques[handle] : D3DXHANDLE{};
  auto desc = D3DXTECHNIQUE_DESC{};
  D3D9CHECK(mEffect->GetTechniqueDesc(technique, &desc));

  return desc;
}

} // namespace basalt::gfx::ext

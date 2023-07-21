#pragma once

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/color.h>

#include <basalt/api/math/types.h>

#include <gsl/span>

#include <vector>

namespace basalt::gfx {

class Environment final {
public:
  Environment() noexcept = default;

  [[nodiscard]] auto background() const noexcept -> const Color&;
  auto set_background(const Color&) noexcept -> void;

  [[nodiscard]] auto ambient_light() const noexcept -> const Color&;
  auto set_ambient_light(const Color&) noexcept -> void;

  [[nodiscard]] auto directional_lights() const noexcept
    -> gsl::span<const DirectionalLight>;
  auto add_directional_light(const Vector3f32& direction, const Color& diffuse,
                             const Color& specular, const Color& ambient)
    -> void;
  auto clear_directional_lights() noexcept -> void;

private:
  Color mBackground;
  Color mAmbientLight;
  std::vector<DirectionalLight> mDirectionalLights;
};

} // namespace basalt::gfx

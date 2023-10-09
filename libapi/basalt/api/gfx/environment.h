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

  [[nodiscard]] auto background() const noexcept -> Color const&;
  auto set_background(Color const&) noexcept -> void;

  [[nodiscard]] auto ambient_light() const noexcept -> Color const&;
  auto set_ambient_light(Color const&) noexcept -> void;

  [[nodiscard]] auto directional_lights() const noexcept
    -> gsl::span<DirectionalLight const>;
  auto add_directional_light(Vector3f32 const& direction, Color const& diffuse,
                             Color const& specular, Color const& ambient)
    -> void;
  auto clear_directional_lights() noexcept -> void;

private:
  Color mBackground;
  Color mAmbientLight;
  std::vector<DirectionalLight> mDirectionalLights;
};

} // namespace basalt::gfx

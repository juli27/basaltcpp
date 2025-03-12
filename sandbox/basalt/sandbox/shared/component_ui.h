#pragma once

#include <basalt/api/gfx/types.h>

#include <basalt/api/scene/types.h>

class ComponentUi {
public:
  static auto transform(basalt::Transform&) -> void;
  static auto local_to_world(basalt::LocalToWorld const&) -> void;
  static auto camera(basalt::gfx::Camera&) -> void;
  static auto model(basalt::gfx::Model const&) -> void;
  static auto x_model(basalt::gfx::ext::XModel const&) -> void;
  static auto light(basalt::gfx::Light&) -> void;
  static auto point_light(basalt::gfx::PointLight&) -> void;
  static auto spot_light(basalt::gfx::SpotLight&) -> void;
};

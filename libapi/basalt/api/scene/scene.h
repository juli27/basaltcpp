#pragma once

#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/scene/ecs.h>
#include <basalt/api/scene/system.h>
#include <basalt/api/scene/types.h>

#include <basalt/api/shared/color.h>
#include <basalt/api/shared/handle_pool.h>
#include <basalt/api/shared/types.h>

#include <basalt/api/math/vector3.h>

#include <utility>
#include <vector>

namespace basalt {

class Scene final {
public:
  static auto create() -> ScenePtr;

  Scene() = default;

  Scene(const Scene&) = delete;
  Scene(Scene&&) = delete;

  ~Scene() noexcept = default;

  auto operator=(const Scene&) -> Scene& = delete;
  auto operator=(Scene&&) -> Scene& = delete;

  [[nodiscard]] auto entity_registry() const -> const EntityRegistry&;
  [[nodiscard]] auto entity_registry() -> EntityRegistry&;

  [[nodiscard]] auto
  create_entity(const Vector3f32& position = Vector3f32 {0.0f},
                const Vector3f32& rotation = Vector3f32 {0.0f},
                const Vector3f32& scale = Vector3f32 {1.0f}) -> Entity;
  [[nodiscard]] auto get_handle(EntityId) -> Entity;

  template <typename T, typename... Args>
  auto create_system(Args&&... args) -> SystemId {
    SystemPtr system {std::make_unique<T>(std::forward<Args>(args)...)};

    return mSystems.allocate(std::move(system));
  }

  auto destroy_system(SystemId) -> void;

  struct UpdateContext final {
    SecondsF32 deltaTime;
  };
  auto on_update(const UpdateContext&) -> void;

  [[nodiscard]] auto background() const -> const Color&;
  auto set_background(const Color&) -> void;

  [[nodiscard]] auto ambient_light() const -> const Color&;
  auto set_ambient_light(const Color&) -> void;

  [[nodiscard]] auto directional_lights() const
    -> const std::vector<gfx::DirectionalLight>&;
  auto add_directional_light(const Vector3f32& direction, const Color&) -> void;
  auto clear_directional_lights() -> void;

private:
  friend class DebugUi;

  EntityRegistry mEntityRegistry;
  HandlePool<SystemPtr, SystemId> mSystems;
  std::vector<gfx::DirectionalLight> mDirectionalLights;
  Color mBackgroundColor {Colors::BLACK};
  Color mAmbientLightColor;
};

} // namespace basalt

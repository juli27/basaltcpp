#pragma once

#include <basalt/api/scene/ecs.h>
#include <basalt/api/scene/system.h>
#include <basalt/api/scene/types.h>

#include <basalt/api/shared/handle_pool.h>
#include <basalt/api/shared/types.h>

#include <basalt/api/math/vector3.h>

#include <entt/core/fwd.hpp>
#include <entt/core/type_info.hpp>

#include <memory>
#include <type_traits>
#include <unordered_map>
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
    static_assert(std::is_base_of_v<System, T>);

    // these are void of no restrictions
    using UpdateBefore = typename SystemTraits<T>::UpdateBefore;
    using UpdateAfter = typename SystemTraits<T>::UpdateAfter;

    const entt::id_type typeId {entt::type_hash<T>::value()};
    const entt::id_type updateBefore {entt::type_hash<UpdateBefore>::value()};
    const entt::id_type updateAfter {entt::type_hash<UpdateAfter>::value()};

    auto system {std::make_unique<T>(std::forward<Args>(args)...)};

    return add_system(std::move(system), {typeId, updateBefore, updateAfter});
  }

  auto destroy_system(SystemId) -> void;

  struct UpdateContext final {
    SecondsF32 deltaTime;
  };

  auto on_update(const UpdateContext&) -> void;

private:
  friend class DebugUi;

  using SystemTypeId = entt::id_type;
  static constexpr SystemTypeId sVoidId = entt::type_hash<void>::value();

  struct SystemInfo final {
    SystemTypeId typeId;
    SystemTypeId updateBeforeTypeId;
    SystemTypeId updateAfterTypeId;
  };

  struct SystemTypeInfo final {
    // systems which are updated after this one
    std::vector<SystemTypeId> updatedAfter;
    // might be SystemId::null() if there is no System of this type
    SystemId id;
  };

  EntityRegistry mEntityRegistry;
  HandlePool<SystemPtr, SystemId> mSystems;
  std::vector<SystemId> mUpdateOrder;
  std::unordered_map<SystemId, SystemTypeId> mSystemIdToSystemType;
  std::unordered_map<SystemTypeId, SystemTypeInfo> mSystemTypes;

  SecondsF32 mTime {};

  [[nodiscard]] auto add_system(SystemPtr, const SystemInfo&) -> SystemId;
  [[nodiscard]] auto compute_update_order() const -> std::vector<SystemId>;
};

} // namespace basalt

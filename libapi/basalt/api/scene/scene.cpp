#include <basalt/api/scene/scene.h>

#include <basalt/api/scene/parent_system.h>
#include <basalt/api/scene/transform.h>
#include <basalt/api/scene/transform_system.h>

#include <basalt/api/shared/asserts.h>

#include <basalt/api/base/types.h>

#include <entt/graph/adjacency_matrix.hpp>

#include <algorithm>
#include <cmath>
#include <forward_list>
#include <iterator>
#include <memory>

using entt::adjacency_matrix;
using entt::directed_tag;

using std::forward_list;
using std::vector;

namespace basalt {

using AdjacencyMatrix = adjacency_matrix<directed_tag>;

enum class Visited : u8 {
  No,
  Visiting,
  Yes,
};

// helper for Scene::compute_update_order
auto depth_first_search(const uSize vertex, const AdjacencyMatrix& adj,
                        vector<Visited>& visited, forward_list<uSize>& order)
  -> void {
  visited[vertex] = Visited::Visiting;

  for (const auto [lhs, rhs] : adj.out_edges(vertex)) {
    if (visited[rhs] == Visited::Visiting) {
      // TODO: give more info about which systems are causing the cycle?
      // TODO: maybe make this less fatal
      //       once the behaviour of the algorithm in this case is better
      //       understood
      BASALT_CRASH("system dependency cycle detected");
    }

    if (visited[rhs] == Visited::No) {
      depth_first_search(rhs, adj, visited, order);
    }
  }

  visited[vertex] = Visited::Yes;
  order.push_front(vertex);
}

auto Scene::create() -> ScenePtr {
  auto scene {std::make_shared<Scene>()};
  scene->create_system<ParentSystem>();
  scene->create_system<TransformSystem>();

  return scene;
}

auto Scene::entity_registry() const -> const EntityRegistry& {
  return mEntityRegistry;
}

auto Scene::entity_registry() -> EntityRegistry& {
  return mEntityRegistry;
}

auto Scene::create_entity(const Vector3f32& position,
                          const Vector3f32& rotation, const Vector3f32& scale)
  -> Entity {
  const EntityId id {mEntityRegistry.create()};
  mEntityRegistry.emplace<LocalToWorld>(id);
  mEntityRegistry.emplace<Transform>(id, position, rotation, scale);

  return Entity {mEntityRegistry, id};
}

auto Scene::get_handle(const EntityId entity) -> Entity {
  return Entity {mEntityRegistry, entity};
}

auto Scene::destroy_system(const SystemId id) -> void {
  const SystemTypeId typeId {mSystemIdToSystemType[id]};
  mSystemTypes.at(typeId).id = SystemId::null();
  mSystemIdToSystemType.erase(id);
  mSystems.deallocate(id);

  mUpdateOrder.erase(std::remove(mUpdateOrder.begin(), mUpdateOrder.end(), id),
                     mUpdateOrder.end());
}

auto Scene::on_update(const UpdateContext& ctx) -> void {
  for (const SystemId systemId : mUpdateOrder) {
    const System::UpdateContext systemCtx {ctx.deltaTime, *this};

    mSystems[systemId]->on_update(systemCtx);
  }
}

auto Scene::add_system(SystemPtr system, const SystemInfo& info) -> SystemId {
  if (info.updateAfterTypeId != sVoidId) {
    mSystemTypes[info.updateAfterTypeId].updatedAfter.push_back(info.typeId);
  }
  if (info.updateBeforeTypeId != sVoidId) {
    // system before other <=> other after system
    mSystemTypes[info.typeId].updatedAfter.push_back(info.updateBeforeTypeId);
  }

  const SystemId id {mSystems.allocate(std::move(system))};
  mSystemIdToSystemType[id] = info.typeId;
  mSystemTypes[info.typeId].id = id;

  mUpdateOrder = compute_update_order();

  return id;
}

// this topologically sorts the systems + their dependencies as a DAG
auto Scene::compute_update_order() const -> std::vector<SystemId> {
  const vector allSystems(mSystems.begin(), mSystems.end());

  const std::unordered_map typeToVertex {[&] {
    std::unordered_map<SystemTypeId, uSize> map;
    uSize nextIndex {0};

    for (const SystemId systemId : allSystems) {
      const SystemTypeId systemTypeId {mSystemIdToSystemType.at(systemId)};

      map[systemTypeId] = nextIndex++;
    }

    return map;
  }()};

  const auto numSystems {static_cast<uSize>(
    std::abs(std::distance(mSystems.begin(), mSystems.end())))};
  // lhs -> rhs means lhs updates before rhs
  adjacency_matrix<directed_tag> systemUpdateDag {numSystems};

  for (const auto& [systemType, typeInfo] : mSystemTypes) {
    // if we haven't assigned a SystemId to a system type, that means that a
    // system with this type is not part of this scene. So we don't have to
    // include it into our DAG
    if (typeInfo.id.is_null()) {
      continue;
    }
    const uSize systemIndex {typeToVertex.at(systemType)};

    for (const SystemTypeId updatedAfter : typeInfo.updatedAfter) {
      // see comment above
      if (!typeToVertex.count(updatedAfter)) {
        continue;
      }
      const uSize updatedAfterIndex {typeToVertex.at(updatedAfter)};

      systemUpdateDag.insert(systemIndex, updatedAfterIndex);
    }
  }

  vector visited(systemUpdateDag.size(), Visited::No);
  forward_list<uSize> order;

  for (const uSize vertex : systemUpdateDag.vertices()) {
    if (visited[vertex] == Visited::No) {
      depth_first_search(vertex, systemUpdateDag, visited, order);
    }
  }

  vector<SystemId> updateOrder;
  updateOrder.reserve(numSystems);

  std::transform(
    order.begin(), order.end(), std::back_inserter(updateOrder),
    [&](const uSize systemIndex) { return allSystems[systemIndex]; });

  return updateOrder;
}

} // namespace basalt

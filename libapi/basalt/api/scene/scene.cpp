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
#include <utility>

using entt::adjacency_matrix;
using entt::directed_tag;

using std::forward_list;
using std::unordered_map;
using std::vector;

namespace basalt {

using AdjacencyMatrix = adjacency_matrix<directed_tag>;

enum class Visited : u8 {
  No,
  Visiting,
  Yes,
};

// helper for Scene::compute_update_order
auto depth_first_search(uSize const vertex, AdjacencyMatrix const& adj,
                        vector<Visited>& visited, forward_list<uSize>& order)
  -> void {
  visited[vertex] = Visited::Visiting;

  for (auto const [lhs, rhs] : adj.out_edges(vertex)) {
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
  auto scene = std::make_shared<Scene>();
  scene->create_system<ParentSystem>();
  scene->create_system<TransformSystem>();

  return scene;
}

auto Scene::entity_registry() const -> EntityRegistry const& {
  return mEntityRegistry;
}

auto Scene::entity_registry() -> EntityRegistry& {
  return mEntityRegistry;
}

auto Scene::create_entity(std::string name, Vector3f32 const& position,
                          Vector3f32 const& rotation, Vector3f32 const& scale)
  -> Entity {
  auto entity = create_entity(position, rotation, scale);
  entity.emplace<EntityName>(std::move(name));

  return entity;
}

auto Scene::create_entity(Vector3f32 const& position,
                          Vector3f32 const& rotation, Vector3f32 const& scale)
  -> Entity {
  auto const id = mEntityRegistry.create();
  mEntityRegistry.emplace<LocalToWorld>(id);
  mEntityRegistry.emplace<Transform>(id, position, rotation, scale);

  return Entity{mEntityRegistry, id};
}

auto Scene::get_handle(EntityId const entity) -> Entity {
  return Entity{mEntityRegistry, entity};
}

auto Scene::destroy_system(SystemId const id) -> void {
  auto const typeId = mSystemIdToSystemType[id];
  mSystemTypes.at(typeId).id = SystemId::null();
  mSystemIdToSystemType.erase(id);
  mSystems.deallocate(id);

  mUpdateOrder.erase(std::remove(mUpdateOrder.begin(), mUpdateOrder.end(), id),
                     mUpdateOrder.end());
}

auto Scene::on_update(UpdateContext const& ctx) -> void {
  mTime += ctx.deltaTime;

  for (auto const systemId : mUpdateOrder) {
    auto const systemCtx = System::UpdateContext{ctx.deltaTime, mTime, *this};

    mSystems[systemId]->on_update(systemCtx);
  }
}

auto Scene::add_system(SystemPtr system, SystemInfo const& info) -> SystemId {
  if (info.updateAfterTypeId != sVoidId) {
    mSystemTypes[info.updateAfterTypeId].updatedAfter.push_back(info.typeId);
  }
  if (info.updateBeforeTypeId != sVoidId) {
    // system before other <=> other after system
    mSystemTypes[info.typeId].updatedAfter.push_back(info.updateBeforeTypeId);
  }

  auto const id = mSystems.allocate(std::move(system));
  mSystemIdToSystemType[id] = info.typeId;
  mSystemTypes[info.typeId].id = id;

  mUpdateOrder = compute_update_order();

  return id;
}

// this topologically sorts the systems + their dependencies as a DAG
auto Scene::compute_update_order() const -> vector<SystemId> {
  auto const allSystems = vector(mSystems.begin(), mSystems.end());

  auto const typeToVertex = [&] {
    auto map = unordered_map<SystemTypeId, uSize>{};
    auto nextIndex = uSize{0};

    for (auto const systemId : allSystems) {
      auto const systemTypeId = mSystemIdToSystemType.at(systemId);

      map[systemTypeId] = nextIndex++;
    }

    return map;
  }();

  auto const numSystems = static_cast<uSize>(
    std::abs(std::distance(mSystems.begin(), mSystems.end())));
  // lhs -> rhs means lhs updates before rhs
  auto systemUpdateDag = adjacency_matrix<directed_tag>{numSystems};

  for (auto const& [systemType, typeInfo] : mSystemTypes) {
    // if we haven't assigned a SystemId to a system type, that means that a
    // system with this type is not part of this scene. So we don't have to
    // include it into our DAG
    if (typeInfo.id.is_null()) {
      continue;
    }
    auto const systemIndex = typeToVertex.at(systemType);

    for (auto const updatedAfterType : typeInfo.updatedAfter) {
      // see comment above
      if (!typeToVertex.count(updatedAfterType)) {
        continue;
      }
      auto const updatedAfterIndex = typeToVertex.at(updatedAfterType);

      systemUpdateDag.insert(systemIndex, updatedAfterIndex);
    }
  }

  auto visited = vector(systemUpdateDag.size(), Visited::No);
  auto order = forward_list<uSize>{};

  for (auto const vertex : systemUpdateDag.vertices()) {
    if (visited[vertex] == Visited::No) {
      depth_first_search(vertex, systemUpdateDag, visited, order);
    }
  }

  auto updateOrder = vector<SystemId>{};
  updateOrder.reserve(numSystems);

  std::transform(
    order.begin(), order.end(), std::back_inserter(updateOrder),
    [&](uSize const systemIndex) { return allSystems[systemIndex]; });

  return updateOrder;
}

} // namespace basalt

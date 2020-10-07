#pragma once

#include "types.h"

#include <entt/entity/registry.hpp>

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace basalt {

struct ResourceRegistry final {
  template <typename Resource>
  auto add(const std::string_view filePath) -> Resource {
    auto& registry = get_registry<Resource>();

    const auto model = registry.create();
    registry.template emplace<FileLocation>(model, std::string {filePath});

    return model;
  }

  template <typename Resource>
  auto get() -> entt::basic_registry<Resource>& {
    return get_registry<Resource>();
  }

private:
  struct Data {
    Data() noexcept = default;
    Data(const Data&) = delete;
    Data(Data&&) = delete;

    virtual ~Data() noexcept = default;

    Data& operator=(const Data&) = delete;
    Data& operator=(Data&&) = delete;
  };

  template <typename Resource>
  struct RegistryData final : Data {
    entt::basic_registry<Resource> registry;
  };

  std::vector<std::unique_ptr<Data>> mRegistries;

  template <typename Resource>
  auto get_registry() -> entt::basic_registry<Resource>& {
    const auto index {entt::type_index<Resource>::value()};

    if (index >= mRegistries.size()) {
      mRegistries.resize(index + 1);
    }

    auto& data = mRegistries[index];
    if (!data) {
      data.reset(new RegistryData<Resource>);
    }

    return static_cast<RegistryData<Resource>&>(*data).registry;
  }
};

} // namespace basalt

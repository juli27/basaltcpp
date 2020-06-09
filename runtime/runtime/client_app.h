#pragma once

#include "Engine.h"

#include "runtime/shared/Config.h"

#include <memory>

namespace basalt {

struct ClientApp {
  ClientApp() = default;

  ClientApp(const ClientApp&) = delete;
  ClientApp(ClientApp&&) = delete;

  virtual ~ClientApp() = default;

  auto operator=(const ClientApp&) -> ClientApp& = delete;
  auto operator=(ClientApp&&) -> ClientApp& = delete;

  virtual void on_update(const UpdateContext&) = 0;

  static auto configure() -> Config;
  static auto create(Engine&) -> std::unique_ptr<ClientApp>;
};

} // namespace basalt

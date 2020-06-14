#pragma once

#include <memory>

namespace basalt {

struct Config;
struct Engine;
struct UpdateContext;

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

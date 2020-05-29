#pragma once
#ifndef BASALT_CLIENT_APP_H
#define BASALT_CLIENT_APP_H

#include "Engine.h"

#include "runtime/shared/Config.h"
#include "runtime/shared/Size2D.h"

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
  static auto create(
    Engine&, Size2Du16 windowSize) -> std::unique_ptr<ClientApp>;
};

} // namespace basalt

#endif // !BASALT_CLIENT_APP_H

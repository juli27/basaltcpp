#pragma once
#ifndef BASALT_CLIENT_APP_H
#define BASALT_CLIENT_APP_H

#include "runtime/shared/Config.h"
#include "runtime/shared/Types.h"

#include <memory>

namespace basalt {

namespace gfx::backend {
struct IRenderer;
} // gfx::backend

struct ClientApp {
  ClientApp() = default;

  ClientApp(const ClientApp&) = delete;
  ClientApp(ClientApp&&) = delete;

  virtual ~ClientApp() = default;

  auto operator=(const ClientApp&) -> ClientApp& = delete;
  auto operator=(ClientApp&&) -> ClientApp& = delete;

  virtual void on_update(f64 deltaTime) = 0;

  static auto configure() -> Config;
  static auto create(gfx::backend::IRenderer*) -> std::unique_ptr<ClientApp>;
};

} // namespace basalt

#endif // !BASALT_CLIENT_APP_H

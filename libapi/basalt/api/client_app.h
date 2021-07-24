#pragma once

#include "types.h"

#include <memory>

namespace basalt {

struct ClientApp {
  ClientApp(const ClientApp&) = delete;
  ClientApp(ClientApp&&) = delete;

  virtual ~ClientApp() = default;

  auto operator=(const ClientApp&) -> ClientApp& = delete;
  auto operator=(ClientApp&&) -> ClientApp& = delete;

  virtual void on_update(Engine&) = 0;

  static auto create(Engine&) -> std::unique_ptr<ClientApp>;

protected:
  ClientApp() noexcept = default;
};

} // namespace basalt

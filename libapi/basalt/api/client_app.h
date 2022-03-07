#pragma once

#include "types.h"

#include <memory>

namespace basalt {

struct ClientApp {
  ClientApp(const ClientApp&) = delete;
  ClientApp(ClientApp&&) = delete;

  virtual ~ClientApp() noexcept = default;

  auto operator=(const ClientApp&) -> ClientApp& = delete;
  auto operator=(ClientApp&&) -> ClientApp& = delete;

  static auto create(Engine&) -> std::unique_ptr<ClientApp>;

protected:
  ClientApp() noexcept = default;
};

} // namespace basalt

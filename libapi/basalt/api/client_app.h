#pragma once

#include <basalt/api/types.h>

namespace basalt {

struct ClientApp final {
  ClientApp() = delete;
  ClientApp(const ClientApp&) = delete;
  ClientApp(ClientApp&&) = delete;

  ~ClientApp() = delete;

  auto operator=(const ClientApp&) -> ClientApp& = delete;
  auto operator=(ClientApp&&) -> ClientApp& = delete;

  static void bootstrap(Engine&);
};

} // namespace basalt

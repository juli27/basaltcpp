#pragma once

#include <basalt/api/types.h>

namespace basalt {

struct ClientApp final {
  ClientApp() = delete;
  ClientApp(ClientApp const&) = delete;
  ClientApp(ClientApp&&) = delete;

  ~ClientApp() = delete;

  auto operator=(ClientApp const&) -> ClientApp& = delete;
  auto operator=(ClientApp&&) -> ClientApp& = delete;

  static auto bootstrap(Engine&) -> void;
};

} // namespace basalt

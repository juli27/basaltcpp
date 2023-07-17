#pragma once

#include <basalt/api/scene/types.h>

#include <basalt/api/shared/types.h>

namespace basalt {

class System {
public:
  System(const System&) = delete;
  System(System&&) = delete;

  virtual ~System() noexcept = default;

  auto operator=(const System&) -> System& = delete;
  auto operator=(System&&) -> System& = delete;

  struct UpdateContext final {
    SecondsF32 deltaTime;
    Scene& scene;
  };

  virtual auto on_update(const UpdateContext&) -> void = 0;

protected:
  System() noexcept = default;
};

namespace detail {

template <typename S, typename = void>
struct GetUpdateBefore final {
  using Type = void;
};

template <typename S>
struct GetUpdateBefore<S, std::void_t<typename S::UpdateBefore>> final {
  using Type = typename S::UpdateBefore;
};

template <typename S, typename = void>
struct GetUpdateAfter final {
  using Type = void;
};

template <typename S>
struct GetUpdateAfter<S, std::void_t<typename S::UpdateAfter>> final {
  using Type = typename S::UpdateAfter;
};

} // namespace detail

template <typename S>
struct SystemTraits final {
  using SystemType = S;

  using UpdateBefore = typename detail::GetUpdateBefore<S>::Type;
  using UpdateAfter = typename detail::GetUpdateAfter<S>::Type;
};

} // namespace basalt

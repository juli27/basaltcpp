#pragma once

#include <basalt/api/shared/asserts.h>

#include <basalt/api/base/types.h>
#include <basalt/api/base/utils.h>

#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>

namespace basalt {

// SERIALIZED
enum class GfxBackendApi : u8 {
  Default = 0,
  Direct3D9 = 1,
};

constexpr i32 GFX_BACKEND_API_COUNT = 2u;

constexpr auto to_gfx_backend_api(const i32 num) -> GfxBackendApi {
  BASALT_ASSERT(num < GFX_BACKEND_API_COUNT);

  if (num >= GFX_BACKEND_API_COUNT || num < 0) {
    return GfxBackendApi::Default;
  }

  return GfxBackendApi {static_cast<u8>(num)};
}

struct Config final {
  using Value = std::variant<bool, i32, std::string>;
  using VarMap = std::unordered_map<std::string, Value>;

  explicit Config(std::initializer_list<VarMap::value_type> defaults);

  [[nodiscard]] auto get_bool(const std::string& key, bool def = false) const
    -> bool;
  [[nodiscard]] auto get_i32(const std::string& key, i32 def = 0) const -> i32;
  [[nodiscard]] auto get_string(const std::string& key,
                                std::string def = {}) const -> std::string;

  template <typename Fun>
  auto get_enum(const std::string& key, Fun f) const
    -> std::invoke_result_t<Fun, i32> {
    return f(get_i32(key));
  }

  [[nodiscard]] auto try_get_bool(const std::string& key) const
    -> std::optional<bool>;
  [[nodiscard]] auto try_get_i32(const std::string& key) const
    -> std::optional<i32>;
  [[nodiscard]] auto try_get_string(const std::string& key) const
    -> std::optional<std::string>;

  auto set_bool(const std::string& key, bool value) -> void;
  auto set_i32(const std::string& key, i32 value) -> void;
  auto set_string(const std::string& key, std::string value) -> void;

  template <typename E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
  auto set_enum(const std::string& key, const E e) -> void {
    set_i32(key, enum_cast(e));
  }

private:
  VarMap mVars {};

  template <typename T,
            typename V = std::remove_cv_t<std::remove_reference_t<T>>>
  [[nodiscard]] auto get(const std::string& key, T&& def) const -> V;

  template <typename V>
  [[nodiscard]] auto try_get(const std::string& key) const -> std::optional<V>;

  template <typename V>
  auto set(const std::string& key, V&& value) -> void;
};

} // namespace basalt

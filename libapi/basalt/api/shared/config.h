#pragma once

#include <basalt/api/base/types.h>
#include <basalt/api/base/utils.h>

#include <optional>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <variant>

namespace basalt {

class Config final {
public:
  using Value = std::variant<bool, i32, std::string>;
  using VarMap = std::unordered_map<std::string, Value>;

  explicit Config(std::initializer_list<VarMap::value_type> defaults);

  [[nodiscard]]
  auto get_bool(std::string const& key, bool def = false) const -> bool;

  [[nodiscard]]
  auto get_i32(std::string const& key, i32 def = 0) const -> i32;

  [[nodiscard]]
  auto get_string(std::string const& key, std::string def = {}) const
    -> std::string;

  template <typename Fun>
  auto get_enum(std::string const& key, Fun f) const
    -> std::invoke_result_t<Fun, i32> {
    return f(get_i32(key));
  }

  [[nodiscard]]
  auto try_get_bool(std::string const& key) const -> std::optional<bool>;
  [[nodiscard]]
  auto try_get_i32(std::string const& key) const -> std::optional<i32>;
  [[nodiscard]]
  auto try_get_string(std::string const& key) const
    -> std::optional<std::string>;

  auto set_bool(std::string const& key, bool value) -> void;
  auto set_i32(std::string const& key, i32 value) -> void;
  auto set_string(std::string const& key, std::string value) -> void;

  template <typename E, std::enable_if_t<std::is_enum_v<E>, int> = 0>
  auto set_enum(std::string const& key, E const e) -> void {
    set_i32(key, enum_cast(e));
  }

private:
  VarMap mVars{};

  template <typename T,
            typename V = std::remove_cv_t<std::remove_reference_t<T>>>
  [[nodiscard]] auto get(std::string const& key, T&& def) const -> V;

  template <typename V>
  [[nodiscard]] auto try_get(std::string const& key) const -> std::optional<V>;

  template <typename V>
  auto set(std::string const& key, V&& value) -> void;
};

} // namespace basalt

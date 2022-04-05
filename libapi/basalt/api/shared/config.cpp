#include <basalt/api/shared/config.h>

#include <utility>

namespace basalt {

Config::Config(const std::initializer_list<VarMap::value_type> defaults)
  : mVars {defaults} {
}

auto Config::get_bool(const std::string& key, const bool def) const -> bool {
  return get(key, def);
}

auto Config::get_i32(const std::string& key, const i32 def) const -> i32 {
  return get(key, def);
}

auto Config::get_string(const std::string& key, std::string def) const
  -> std::string {
  return get(key, std::move(def));
}

auto Config::try_get_bool(const std::string& key) const -> std::optional<bool> {
  return try_get<bool>(key);
}

auto Config::try_get_i32(const std::string& key) const -> std::optional<i32> {
  return try_get<i32>(key);
}

auto Config::try_get_string(const std::string& key) const
  -> std::optional<std::string> {
  return try_get<std::string>(key);
}

auto Config::set_bool(const std::string& key, bool value) -> void {
  set(key, value);
}

auto Config::set_i32(const std::string& key, i32 value) -> void {
  set(key, value);
}

auto Config::set_string(const std::string& key, std::string value) -> void {
  set(key, std::move(value));
}

template <typename T, typename V>
auto Config::get(const std::string& key, T&& def) const -> V {
  return try_get<V>(key).value_or(std::forward<T>(def));
}

template <typename V>
auto Config::try_get(const std::string& key) const -> std::optional<V> {
  if (const auto it = mVars.find(key); it != mVars.end()) {
    return std::get<V>(it->second);
  }

  return std::nullopt;
}

template <typename V>
auto Config::set(const std::string& key, V&& value) -> void {
  mVars.insert_or_assign(key, std::forward<V>(value));
}

} // namespace basalt

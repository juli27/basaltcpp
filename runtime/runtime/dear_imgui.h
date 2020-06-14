#pragma once

namespace basalt {

struct UpdateContext;

namespace gfx {

struct Device;

} // namespace gfx

struct DearImGui final {
  explicit DearImGui(gfx::Device*);

  DearImGui(const DearImGui&) = delete;
  DearImGui(DearImGui&&) = delete;

  ~DearImGui();

  auto operator=(const DearImGui&) -> DearImGui& = delete;
  auto operator=(DearImGui&&) -> DearImGui& = delete;

  void new_frame(const UpdateContext&) const;

private:
  gfx::Device* mRenderer {};
};

} // namespace basalt

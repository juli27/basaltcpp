#pragma once

#include <basalt/api/gfx/backend/ext/extension.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/gfx/backend/types.h>

#include <basalt/api/shared/color.h>

#include <gsl/span>

#include <filesystem>
#include <vector>

namespace basalt::gfx::ext {

class XModelData final {
public:
  struct Material final {
    Color diffuse {};
    Color ambient {};
    std::filesystem::path textureFile {};
  };

  explicit XModelData(XMesh, std::vector<Material>);

  [[nodiscard]] auto mesh() const noexcept -> XMesh;

  [[nodiscard]] auto materials() const noexcept -> gsl::span<const Material>;

private:
  std::vector<Material> mMaterials;
  XMesh mMesh;
};

struct XModelSupport : ExtensionT<ExtensionId::XModelSupport> {
  [[nodiscard]] virtual auto load(const std::filesystem::path&)
    -> XModelData = 0;

  virtual auto destroy(XMesh) noexcept -> void = 0;
};

struct CommandDrawXMesh final : CommandT<CommandType::ExtDrawXMesh> {
  XMesh handle;
  u32 subset;

  constexpr CommandDrawXMesh(const XMesh meshHandle,
                             const u32 subsetIndex) noexcept
    : handle {meshHandle}, subset {subsetIndex} {
  }
};

static_assert(sizeof(CommandDrawXMesh) == 12);

} // namespace basalt::gfx::ext

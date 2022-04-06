#pragma once

#include <basalt/api/types.h>
#include <basalt/api/view.h>

#include <basalt/api/gfx/types.h>
#include <basalt/api/gfx/backend/types.h>
#include <basalt/api/gfx/backend/ext/types.h>

#include <basalt/api/shared/types.h>

namespace tribase {

class StencilBuffer final : public basalt::View {
public:
  explicit StencilBuffer(basalt::Engine const&);

private:
  basalt::gfx::ResourceCachePtr mGfxCache;
  basalt::gfx::PipelineHandle mPrePassPipeline;
  basalt::gfx::PipelineHandle mOverdrawPipeline1;
  basalt::gfx::PipelineHandle mOverdrawPipeline2;
  basalt::gfx::ext::XMeshHandle mThingMesh;
  basalt::gfx::VertexBufferHandle mRectanglesVb;
  basalt::SecondsF32 mTime{};

  auto on_update(UpdateContext&) -> void override;
};

} // namespace tribase

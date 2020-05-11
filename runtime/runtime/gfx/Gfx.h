#pragma once
#ifndef BASALT_GFX_H
#define BASALT_GFX_H

namespace basalt {

struct Scene;

namespace gfx {
namespace backend {

struct IRenderer;

} // namespace backend

void render(backend::IRenderer* renderer, Scene* scene);

} // namespace gfx
} // namespace basalt

#endif // !BASALT_GFX_H

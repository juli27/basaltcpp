#pragma once
#ifndef BASALT_GFX_H
#define BASALT_GFX_H

namespace basalt {
namespace gfx {

struct View;

namespace backend {

struct IRenderer;

} // namespace backend

void render(backend::IRenderer* renderer, const View&);

} // namespace gfx
} // namespace basalt

#endif // !BASALT_GFX_H

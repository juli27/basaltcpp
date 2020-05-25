#pragma once
#ifndef BASALT_ENGINE_H
#define BASALT_ENGINE_H

namespace basalt {

namespace gfx {

struct View;

} // namespace gfx

void quit();

void set_view(const gfx::View&);

} // namespace basalt

#endif // !BASALT_ENGINE_H

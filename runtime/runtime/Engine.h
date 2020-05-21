#pragma once
#ifndef BASALT_ENGINE_H
#define BASALT_ENGINE_H

#include <memory>

namespace basalt {

struct Scene;

void quit();
void set_current_scene(std::shared_ptr<Scene> scene);
auto get_current_scene() -> Scene*;

void draw_scene_debug_ui(bool* open);

} // namespace basalt

#endif // !BASALT_ENGINE_H

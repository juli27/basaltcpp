#pragma once

#include <memory>

namespace basalt {

struct DearImGui;
using DearImGuiPtr = std::shared_ptr<DearImGui>;

struct InputManager;

class Runtime;

} // namespace basalt

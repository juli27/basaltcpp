#pragma once

#include <memory>

namespace basalt {

class Win32AppWindow;
using Win32AppWindowPtr = std::unique_ptr<Win32AppWindow>;

class Win32Window;
using Win32WindowPtr = std::unique_ptr<Win32Window>;

class Win32MessageQueue;

} // namespace basalt

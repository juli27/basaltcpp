#pragma once
#ifndef BASALT_H
#define BASALT_H

#include "Config.h"
#include "Engine.h"
#include "IApplication.h"
#include "Input.h"
#include "Scene.h"

#include "gfx/Camera.h"
#include "gfx/Gfx.h"
#include "gfx/RenderComponent.h"

#include "gfx/backend/IRenderer.h"
#include "gfx/backend/RenderCommand.h"
#include "gfx/backend/Types.h"

#include "platform/Platform.h"
#include "platform/Types.h"

#include "platform/events/Event.h"
#include "platform/events/KeyEvents.h"
#include "platform/events/MouseEvents.h"
#include "platform/events/WindowEvents.h"

#include "math/Constants.h"
#include "math/Mat4.h"
#include "math/Vec2.h"
#include "math/Vec3.h"

#include "shared/Color.h"
#include "shared/Exceptions.h"
#include "shared/Handle.h"
#include "shared/HandlePool.h"
#include "shared/Types.h"

// at the bottom because of the stupid windows.h dependency
#include "Log.h"
#include "shared/Asserts.h"

#include <entt/entt.hpp>
#include <fmt/format.h>
#include <imgui/imgui.h>

#endif // !BASALT_H

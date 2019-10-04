#pragma once
#ifndef BS_BASALT_H
#define BS_BASALT_H

#include "Config.h"
#include "Engine.h"
#include "IApplication.h"
#include "Input.h"
#include "Scene.h"

#include "common/Color.h"
#include "common/Exceptions.h"
#include "common/Handle.h"
#include "common/HandlePool.h"
#include "common/Types.h"

#include "gfx/Camera.h"
#include "gfx/Gfx.h"
#include "gfx/RenderComponent.h"

#include "gfx/backend/IRenderer.h"
#include "gfx/backend/RenderCommand.h"
#include "gfx/backend/Types.h"

#include "math/Constants.h"
#include "math/Mat4.h"
#include "math/Vec2.h"
#include "math/Vec3.h"

#include "platform/Platform.h"
#include "platform/Types.h"

#include "platform/events/Event.h"
#include "platform/events/KeyEvents.h"
#include "platform/events/MouseEvents.h"
#include "platform/events/WindowEvents.h"

// at the bottom because of the stupid windows.h dependency
#include "Log.h"
#include "common/Asserts.h"

#include <entt/entt.hpp>
#include <imgui/imgui.h>

#ifndef BS_NO_SHORT_NS
namespace bs = basalt;
#endif // !BS_NO_SHORT_NS

#endif // !BS_BASALT_H

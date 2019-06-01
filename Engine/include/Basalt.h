#pragma once
#ifndef BS_BASALT_H
#define BS_BASALT_H

#include "basalt/Config.h"
#include "basalt/Engine.h"
#include "basalt/IApplication.h"
#include "basalt/Input.h"

#include "basalt/common/Asserts.h"
#include "basalt/common/Exceptions.h"
#include "basalt/common/Types.h"

#include "basalt/gfx/backend/Factory.h"
#include "basalt/gfx/backend/IRenderer.h"
#include "basalt/gfx/backend/RenderMesh.h"

#include "basalt/math/Constants.h"
#include "basalt/math/Mat4.h"
#include "basalt/math/Vec2.h"
#include "basalt/math/Vec3.h"

#include "basalt/platform/Platform.h"
#include "basalt/platform/WindowTypes.h"

#include "basalt/platform/events/Event.h"
#include "basalt/platform/events/KeyEvents.h"
#include "basalt/platform/events/MouseEvents.h"
#include "basalt/platform/events/WindowEvents.h"

// at the bottom because of the stupid windows.h dependency
#include "basalt/Log.h"

//#include <imgui/imgui.h>

#ifndef BS_NO_SHORT_NS
namespace bs = basalt;
#endif // !BS_NO_SHORT_NS

#endif // !BS_BASALT_H

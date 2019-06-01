#include "pch.h"

#include <basalt/Input.h>

#include <bitset>

#include <basalt/common/Types.h>
#include <basalt/math/Vec2.h>
#include <basalt/platform/Platform.h>
#include <basalt/platform/events/KeyEvents.h>
#include <basalt/platform/events/MouseEvents.h>

namespace basalt {
namespace input {
namespace {

static constexpr i32 NUM_KEYS = static_cast<i32>(Key::NUMBER_OF_KEYS);
static constexpr i32 NUM_MOUSE_BUTTONS = static_cast<i32>(
  MouseButton::NUMBER_OF_BUTTONS
);

std::bitset<NUM_KEYS> s_keyStates;
std::bitset<NUM_MOUSE_BUTTONS> s_buttonStates;
math::Vec2i32 s_mousePos;


void OnKeyPressed(const platform::KeyPressedEvent& event) {
  const i32 index = static_cast<i32>(event.GetKey());
  s_keyStates[index] = true;
}


void OnKeyReleased(const platform::KeyReleasedEvent& event) {
  const i32 index = static_cast<i32>(event.GetKey());
  s_keyStates[index] = false;
}


void OnMouseMoved(const platform::MouseMovedEvent& event) {
  s_mousePos = event.GetMousePos();
}


void OnButtonPressed(const platform::MouseButtonPressedEvent& event) {
  const i32 index = static_cast<i32>(event.GetButton());
  s_buttonStates[index] = true;
}


void OnButtonReleased(const platform::MouseButtonReleasedEvent& event) {
  const i32 index = static_cast<i32>(event.GetButton());
  s_buttonStates[index] = false;
}


void PlatformEventCallback(const platform::Event& event) {
  platform::EventDispatcher dispatcher(event);
  dispatcher.Dispatch<platform::MouseMovedEvent>(&OnMouseMoved);
  dispatcher.Dispatch<platform::KeyPressedEvent>(&OnKeyPressed);
  dispatcher.Dispatch<platform::KeyReleasedEvent>(&OnKeyReleased);
  dispatcher.Dispatch<platform::MouseButtonPressedEvent>(&OnButtonPressed);
  dispatcher.Dispatch<platform::MouseButtonReleasedEvent>(&OnButtonReleased);
}

} // namespace


void Init() {
  platform::AddEventListener(&PlatformEventCallback);
}


bool IsKeyPressed(Key key) {
  return s_keyStates[static_cast<i32>(key)];
}


const math::Vec2i32& GetMousePos() {
  return s_mousePos;
}


bool IsMouseButtonPressed(MouseButton button) {
  return s_buttonStates[static_cast<i32>(button)];
}

} // namespace input
} // namespace basalt

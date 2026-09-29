#include "InputSystem.h"

InputSystem::InputSystem()
	: m_gamepads{ GamepadInput(0),GamepadInput(1),GamepadInput(2),GamepadInput(3)}
	, m_keyboard()
	, m_mouse()
{
}

void InputSystem::NewFrame()
{
	m_keyboard.NewFrame();
	m_mouse.NewFrame();
	for (auto& gamepad : m_gamepads)
	{
		gamepad.Update();
	}
}

KeyboardInput& InputSystem::Keyboard()			{ return m_keyboard; }
MouseInput& InputSystem::Mouse()				{ return m_mouse; }
GamepadInput& InputSystem::Gamepad(int index)	{ return m_gamepads[index]; }

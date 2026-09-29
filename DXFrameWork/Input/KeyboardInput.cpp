#include "KeyboardInput.h"

void KeyboardInput::NewFrame()
{
	m_previous = m_current;
}

void KeyboardInput::OnKeyDown(int virtualKey)
{
	if (virtualKey >= 0 && virtualKey < kKeyCount)
	{
		m_current[virtualKey] = true;
	}
}

void KeyboardInput::OnKeyUp(int virtualKey)
{
	if (virtualKey >= 0 && virtualKey < kKeyCount)
	{
		m_current[virtualKey] = false;
	}
}

bool KeyboardInput::IsDown(int virtualKey) const
{
	return (virtualKey >= 0 && virtualKey < kKeyCount) && 
		m_current[virtualKey];
}

bool KeyboardInput::IsPressed(int virtualKey) const
{
	return (virtualKey >= 0 && virtualKey < kKeyCount) && 
		m_current[virtualKey] && !m_previous[virtualKey];
}

bool KeyboardInput::IsReleased(int virtualKey) const
{
	return (virtualKey >= 0 && virtualKey < kKeyCount) && 
		!m_current[virtualKey] && m_previous[virtualKey];
}
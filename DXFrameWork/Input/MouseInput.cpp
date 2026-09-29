#include "MouseInput.h"

void MouseInput::NewFrame()
{
    m_previous = m_current;

    m_deltaX = m_x - m_prevX;
    m_deltaY = m_y - m_prevY;
    m_prevX = m_x;
    m_prevY = m_y;

	// ホイールの回転量はフレームごとにリセットする
    m_wheelDelta = 0;
}

void MouseInput::OnMove(int x, int y)
{
    m_x = x;
	m_y = y;
}

void MouseInput::OnButton(MouseButton button, bool isDown)
{
	const size_t index = static_cast<size_t>(button);
    if (index < kButtonCount)
    {
        m_current[index] = isDown;
	}
}

void MouseInput::OnWheel(int delta)
{
    m_wheelDelta += delta;
}

bool MouseInput::IsDown(MouseButton button) const
{
    const size_t index = static_cast<size_t>(button);
    return (index < kButtonCount) && m_current[index];
}

bool MouseInput::IsPressed(MouseButton button) const
{
    const size_t index = static_cast<size_t>(button);
    return (index < kButtonCount) && m_current[index] && !m_previous[index];
}

bool MouseInput::IsReleased(MouseButton button) const
{
    const size_t index = static_cast<size_t>(button);
    return (index < kButtonCount) && !m_current[index] && m_previous[index];
}

int MouseInput::GetX() const
{
    return m_x;
}

int MouseInput::GetY() const
{
    return m_y;
}

int MouseInput::GetDeltaX() const
{
    return m_deltaX;
}

int MouseInput::GetDeltaY() const
{
    return m_deltaY;
}

int MouseInput::GetWheelDelta() const
{
    return m_wheelDelta;
}

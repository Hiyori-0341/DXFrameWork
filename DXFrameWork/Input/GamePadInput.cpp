#include <algorithm>
#include <iostream>
#include "GamePadInput.h"

#pragma comment(lib, "XInput.lib")

GamepadInput::GamepadInput(DWORD userIndex)
	: m_userIndex(userIndex)
	, m_isConnected(false)
	, m_currentButtons(0)
	, m_previousButtons(0)
	, m_leftStickX(0.0f)
	, m_leftStickY(0.0f)
	, m_rightStickX(0.0f)
	, m_rightStickY(0.0f)
	, m_leftTrigger(0.0f)
	, m_rightTrigger(0.0f)
{}

void GamepadInput::Update()
{
	XINPUT_STATE state{};
	const DWORD result = XInputGetState(m_userIndex, &state);

	m_isConnected = (result == ERROR_SUCCESS);
	m_previousButtons = m_currentButtons;

	if(!m_isConnected)
	{
		m_currentButtons = 0;
		m_leftStickX = 0.0f;
		m_leftStickY = 0.0f;
		m_rightStickX = 0.0f;
		m_rightStickY = 0.0f;
		m_leftTrigger = 0.0f;
		m_rightTrigger = 0.0f;
		return;
	}

	const XINPUT_GAMEPAD& pad = state.Gamepad;
	m_currentButtons = pad.wButtons;

	m_leftStickX = ApplyStickDeadZone(pad.sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
	m_leftStickY = ApplyStickDeadZone(pad.sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
	m_rightStickX = ApplyStickDeadZone(pad.sThumbRX, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
	m_rightStickY = ApplyStickDeadZone(pad.sThumbRY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);

	m_leftTrigger = ApplyTriggerDeadZone(pad.bLeftTrigger, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
	m_rightTrigger = ApplyTriggerDeadZone(pad.bRightTrigger, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
}

bool GamepadInput::IsDown(GamepadButton button) const
{
	return (m_currentButtons & static_cast<WORD>(button)) != 0;
}

bool GamepadInput::IsPressed(GamepadButton button)const
{
	const WORD bit = static_cast<WORD>(button);
	return (m_currentButtons & bit) != 0 && (m_previousButtons & bit) == 0;
}

bool GamepadInput::IsReleased(GamepadButton button) const
{
	const WORD bit = static_cast<WORD>(button);
	return (m_currentButtons & bit) == 0 && (m_previousButtons & bit) != 0;
}

void GamepadInput::SetVibration(float leftMotor, float rightMotor)
{
	XINPUT_VIBRATION vibration{};
	vibration.wLeftMotorSpeed = static_cast<WORD>(std::clamp(leftMotor, 0.0f, 1.0f) * 65535.0f);
	vibration.wRightMotorSpeed = static_cast<WORD>(std::clamp(rightMotor, 0.0f, 1.0f) * 65535.0f);
	XInputSetState(m_userIndex, &vibration);
}

void GamepadInput::SetVibration(float leftMotor, float rightMotor)
{
	XINPUT_VIBRATION vibration{};
	vibration.wLeftMotorSpeed = static_cast<WORD>(std::clamp(leftMotor, 0.0f, 1.0f) * 65535.0f);
	vibration.wRightMotorSpeed = static_cast<WORD>(std::clamp(rightMotor, 0.0f, 1.0f) * 65535.0f);
	XInputSetState(m_userIndex, &vibration);
}

// スティックの値にデッドゾーンを適用する
float GamepadInput::ApplyStickDeadZone(SHORT value, SHORT deadzone)
{
	if (value > -deadzone && value < deadzone)
	{
		return 0.0f;
	}

	//デッドゾーンの外側を0.0~1.0の範囲に正規化する
	const float sign = (value > 0) ? 1.0f : -1.0f;
	const float magnitude = static_cast<float>(std::abs(static_cast<int>(value))) - deadzone;
	const float range = static_cast<float>(SHRT_MAX) - deadzone;
	
	return sign * std::clamp(magnitude / range, 0.0f, 1.0f);
}

float GamepadInput::ApplyTriggerDeadZone(BYTE value, BYTE threshold)
{
	if (value < threshold)
	{
		return 0.0f;
	}
	return static_cast<float>(value - threshold) / static_cast<float>(255 - threshold);
}

bool GamepadInput::IsConnected() const { return m_isConnected; }

float GamepadInput::GetLeftStickX()   const { return m_leftStickX; }
float GamepadInput::GetLeftStickY()   const { return m_leftStickY; }
float GamepadInput::GetRightStickX()  const { return m_rightStickX; }
float GamepadInput::GetRightStickY()  const { return m_rightStickY; }
float GamepadInput::GetLeftTrigger()  const { return m_leftTrigger; }
float GamepadInput::GetRightTrigger() const { return m_rightTrigger; }

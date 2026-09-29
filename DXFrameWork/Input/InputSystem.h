#pragma once

#include <array>
#include "KeyboardInput.h"
#include "MouseInput.h"
#include "GamePadInput.h"

// 入力システムを管理するクラス
class InputSystem
{
public:
	InputSystem();

	void NewFrame(); // フレームの開始。各入力デバイスの状態を更新する

	KeyboardInput& Keyboard();			// キーボード入力を取得する
	MouseInput& Mouse();				// マウス入力を取得する
	GamepadInput& Gamepad(int index);	// ゲームパッド入力を取得する

	static constexpr int kMaxGamepads = 4; // 最大ゲームパッド数

private:
	KeyboardInput m_keyboard; // キーボード入力
	MouseInput m_mouse; // マウス入力
	std::array<GamepadInput, kMaxGamepads> m_gamepads; // ゲームパッド入力
};
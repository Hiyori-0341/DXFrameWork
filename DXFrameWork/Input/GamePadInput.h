#pragma once 
#include <windows.h>
#include <Xinput.h>

//ゲームパッドのボタン
//unsigned shortにすることで、XInputのボタン定数と同じ型にする
enum class GamepadButton : unsigned short
{
	DPadUp			= XINPUT_GAMEPAD_DPAD_UP,
	DPadDown		= XINPUT_GAMEPAD_DPAD_DOWN,
	DPadLeft		= XINPUT_GAMEPAD_DPAD_LEFT,
	DPadRight		= XINPUT_GAMEPAD_DPAD_RIGHT,
	Start			= XINPUT_GAMEPAD_START,
	Back			= XINPUT_GAMEPAD_BACK,
	LeftThumb		= XINPUT_GAMEPAD_LEFT_THUMB,
	RightThumb		= XINPUT_GAMEPAD_RIGHT_THUMB,
	LeftShoulder	= XINPUT_GAMEPAD_LEFT_SHOULDER,
	RightShoulder	= XINPUT_GAMEPAD_RIGHT_SHOULDER,
	A				= XINPUT_GAMEPAD_A,
	B				= XINPUT_GAMEPAD_B,
	X				= XINPUT_GAMEPAD_X,
	Y				= XINPUT_GAMEPAD_Y,
	Count //ボタンの数
};

// XInputによる1台分のコントローラ操作を管理する
// WIndowのメッセージは使わずに、毎フレームUpdate()を呼ぶことで状態を更新する
class GamepadInput
{
	explicit GamepadInput(DWORD userIndex);

	void Update();				//コントローラの状態を更新する

	bool IsConnected() const;   //コントローラが接続されているかを取得する

	bool IsDown(GamepadButton button) const;	 //ボタンが押されているかを取得する
	bool IsPressed(GamepadButton button) const;  //ボタンが押された瞬間かを取得する
	bool IsReleased(GamepadButton button) const; //ボタンが離された瞬間かを取得する

	// スティックの値を取得する
	// -1.0～1.0に正規化して、デッドゾーンを適用した値
	float GetLeftStickX() const; //左スティックのX軸の値を取得する
	float GetLeftStickY() const; //左スティックのY軸の値を取得する
	float GetRightStickX() const; //右スティックのX軸の値を取得する
	float GetRightStickY() const; //右スティックのY軸の値を取得する

	float GetLeftTrigger() const;  //左トリガーの値を取得する
	float GetRightTrigger() const; //右トリガーの値を取得する

	void SetVibration(float leftMotor, float rightMotor); //振動を設定する

private:
	static float ApplyStickDeadZone(SHORT value,SHORT deadzone); //スティックの値にデッドゾーンを適用する
	static float ApplyTriggerDeadZone(BYTE value, BYTE threshold); //トリガーの値にデッドゾーンを適用する

	DWORD m_userIndex; //コントローラのユーザーインデックス(0～3)
	bool m_isConnected; //コントローラが接続されているか

	WORD m_currentButtons;  //現在のフレームでのボタンの状態
	WORD m_previousButtons; //前のフレームでのボタンの状態

	float m_leftStickX;  //左スティックのX軸の値(-1.0～1.0)
	float m_leftStickY;  //左スティックのY軸の値(-1.0～1.0)
	float m_rightStickX; //右スティックのX軸の値(-1.0～1.0)
	float m_rightStickY; //右スティックのY軸の値(-1.0～1.0)
	float m_leftTrigger;  //左トリガーの値(0.0～1.0)
	float m_rightTrigger; //右トリガーの値(0.0～1.0)
};
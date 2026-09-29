#pragma once
#include <array>

//マウスボタン
enum class MouseButton
{
	Left,	//左ボタン
	Right,	//右ボタン
	Middle,	//中ボタン

	Count,	//ボタンの数
};

// マウス入力を管理するクラス
class MouseInput
{
public:
	void NewFrame();

	void OnMove(int x, int y);
	void OnButton(MouseButton button, bool isDown);
	void OnWheel(int delta);

	bool IsDown(MouseButton button) const;
	bool IsPressed(MouseButton button) const;
	bool IsReleased(MouseButton button) const;

	int  GetX() const;
	int  GetY() const;
	int  GetDeltaX() const;
	int  GetDeltaY() const;
	int  GetWheelDelta() const;

private:
	static constexpr size_t kButtonCount = static_cast<size_t>(MouseButton::Count);

	std::array<bool, kButtonCount> m_current{};	   // 現在のフレームでのボタンの押下状態
	std::array<bool, kButtonCount> m_previous{};	   // 前のフレームでのボタンの押下状態

	int m_x = 0;			// 現在のマウス座標X
	int m_y = 0;			// 現在のマウス座標Y
	int m_prevX = 0;		// 前のフレームでのマウス座標X
	int m_prevY = 0;		// 前のフレームでのマウス座標Y
	int m_deltaX = 0;		// 前のフレームからのマウス座標Xの変化量
	int m_deltaY = 0;		// 前のフレームからのマウス座標Yの変化量
	int m_wheelDelta = 0;	// ホイールの回転量

};
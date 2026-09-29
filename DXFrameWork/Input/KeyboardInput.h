#pragma once
#include <array>

// キーボード入力を管理するクラス
// Windowからメッセージを受け取って、キーの押下状態を管理する
class KeyboardInput
{
public:
	// フレームの開始。キーの押下状態を更新する
	void NewFrame();

	// キーが押されているかを取得する
	void OnKeyDown(int virtualKey);
	void OnKeyUp(int virtualKey);

	bool IsDown(int virtualKey) const;		 //押している間ずっとtrue
	bool IsPressed(int virtualKey) const;	 //押した瞬間だけtrue
	bool IsReleased(int virtualKey) const;	 //離した瞬間だけtrue

private:
	static constexpr int kKeyCount = 256; // 仮想キーコードの最大値

	std::array<bool, kKeyCount> m_current{};	   // 現在のフレームでのキーの押下状態
	std::array<bool, kKeyCount> m_previous{};	   // 前のフレームでのキーの押下状態
};
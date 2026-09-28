#pragma once

#include <windows.h>
#include "Window.h"

// アプリケーション全体の流れ(初期化 → ループ → 終了)を管理するクラス
class Application
{
public:
	bool Initialize(HINSTANCE hInstance, int nCmdShow);
	int  Run();

private:
	void Update();
	void Render();

	Window m_window;
};
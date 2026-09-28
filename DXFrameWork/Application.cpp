#include "Application.h"

namespace
{
	constexpr int kClientWidth = 1280;
	constexpr int kClientHeight = 720;
}

bool Application::Initialize(HINSTANCE hInstance, int nCmdShow)
{
	if (!m_window.Create(hInstance, L"DXFrameWork", kClientWidth, kClientHeight))
	{
		return false;
	}

	m_window.Show(nCmdShow);
	return true;
}

int Application::Run()
{
	// ウィンドウが閉じられる(WM_QUIT)まで回す
	while (m_window.PumpMessages())
	{
		// 最小化中は更新も描画もしない(CPUを使い切らないよう少し待つ)
		if (m_window.IsMinimized())
		{
			Sleep(10);
			continue;
		}

		Update();
		Render();
	}
	return 0;
}

void Application::Update()
{
	// ステップ2以降: Time / Input / ゲームの更新
}

void Application::Render()
{
	// ステップ3以降: D3D11の描画
}
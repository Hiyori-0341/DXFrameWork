#include "Application.h"
#include <cwchar>
 
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
	m_time.Initialize();
	return true;
}
 
int Application::Run()
{
	// ウィンドウが閉じられる(WM_QUIT)まで回す
	while (m_window.PumpMessages())
	{
		// 最小化中も Tick は呼ぶ(復帰時に巨大な deltaTime が出ないようにする)
		m_time.Tick();
 
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
	// ステップ4以降: Input / ゲームの更新
	UpdateWindowTitle();
}
 
void Application::Render()
{
	// ステップ3以降: D3D11の描画
}
 
// FPSとdeltaTimeをタイトルバーに表示する(値が更新されたときだけ)
void Application::UpdateWindowTitle()
{
	if (!m_time.IsFpsUpdated())
	{
		return;
	}
 
	wchar_t title[128]{};
	swprintf_s(title, L"DXFrameWork  FPS: %.1f  dt: %.4f ms",
		m_time.GetFPS(), m_time.GetDeltaTime() * 1000.0f);
 
	m_window.SetTitle(title);
}
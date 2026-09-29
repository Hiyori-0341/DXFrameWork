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

	// D3D11の初期化(ウィンドウ作成後)
	if (!m_graphics.Initialize(m_window.GetHWND(), m_window.GetWidth(), m_window.GetHeight()))
	{
		return false;
	}

	// ウィンドウのサイズが変わったら、描画先を作り直す
	m_window.SetResizeCallBack([this](int width, int height)
		{
			m_graphics.Resize(width, height);
		});

	// キーボード/マウスの入力コールバックを登録する
	m_window.SetKeyCallback([this](int virtualKey, bool isDown)
		{
			if (isDown)m_input.Keyboard().OnKeyDown(virtualKey);
			else       m_input.Keyboard().OnKeyUp(virtualKey);
		});

	m_window.SetMouseMoveCallback([this](int x, int y)
		{
			m_input.Mouse().OnMove(x, y);
		});

	m_window.SetMouseButtonCallback([this](int button, bool isDown)
		{
			m_input.Mouse().OnButton(static_cast<MouseButton>(button), isDown);
		});

	m_window.SetMouseWheelCallback([this](int delta)
		{
			m_input.Mouse().OnWheel(delta);
		});

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

		// 入力の更新
		m_input.NewFrame();
		
		// 最小化中は更新も描画もしない(CPUを使い切らないよう少し待つ)
		if (m_window.GetIsMinimized())
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

	UpdateWindowTitle();
}

void Application::Render()
{
	const float clearColor[4] = { 0.1f, 0.2f, 0.4f, 1.0f };

	m_graphics.BeginFrame(clearColor);
	// ステップ6以降: ここに3Dオブジェクトの描画が入る
	m_graphics.EndFrame();
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
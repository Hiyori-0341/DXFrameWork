#include "Window.h"

namespace
{
	// ウィンドウクラス名
	constexpr wchar_t kClassName[] = L"DXFrameWork";
}

Window::~Window()
{
	// ウィンドウが作成されている場合は破棄する
	if(m_hWnd != nullptr)
	{
		DestroyWindow(m_hWnd);
	}
	// ウィンドウクラスが登録されている場合は登録解除する
	if(m_hInstance != nullptr)
	{
		UnregisterClass(kClassName, m_hInstance);
	}
}

bool Window::Create(HINSTANCE hInstance, const std::wstring& title, int clientWidth, int clientHeight)
{
	m_hInstance = hInstance;

	// ウィンドウクラスの登録
	WNDCLASSEX wc{};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.lpszClassName = kClassName;

	if(!RegisterClassEx(&wc))
	{
		return false;
	}

	//クライアント領域が指定サイズになるように枠やタイトルバーを加えたサイズを求める
	const DWORD style = WS_OVERLAPPEDWINDOW;
	RECT rect = { 0, 0, clientWidth, clientHeight };
	AdjustWindowRect(&rect, style, FALSE);

	//引数にthisを渡して、StaticWndProcで取り出す
	m_hWnd = CreateWindowEx(
		0,
		kClassName,
		title.c_str(),
		style,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, 
		rect.bottom - rect.top,
		nullptr,
		nullptr,
		hInstance,
		this
	);

	if(m_hWnd == nullptr)
	{
		return false;
	}

	m_width = clientWidth;
	m_height = clientHeight;

	return true;
}

/// @brief ウィンドウを表示する
/// @param nCmdShow 
void Window::Show(int nCmdShow)
{
	ShowWindow(m_hWnd, nCmdShow);
	UpdateWindow(m_hWnd);
}

/// @brief メッセージループを処理する
bool Window::PumpMessages()
{
	MSG msg{};
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if(msg.message == WM_QUIT)
		{
			return false;
		}
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return true;
}

//ウィンドウプロシージャ、thisポインタを取得してHandleMessageに処理を委譲する
LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* self = nullptr;

	if (msg == WM_NCCREATE)
	{
		//CreateWindowExのlpParamに渡したthisポインタを取得する
		auto* create = reinterpret_cast<CREATESTRUCT*>(lParam);
		// thisポインタを取得
		self = reinterpret_cast<Window*>(create->lpCreateParams);
		// ウィンドウハンドルにthisポインタを関連付ける
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(self));
	}
	else
	{
		//ウィンドウハンドルからthisポインタを取得する
		self = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	}

	if(self == nullptr)
	{
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return self->HandleMessage(hwnd, msg, wParam, lParam);
}



LRESULT Window::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	// ウィンドウのサイズが変更されたときの処理
	case WM_SIZE:
		m_isMinimized = (wParam == SIZE_MINIMIZED);
		if (!m_isMinimized)
		{
			m_width = LOWORD(lParam);
			m_height = HIWORD(lParam);
			if (m_resizeCallback)
			{
				m_resizeCallback(m_width, m_height);
			}
		}
		return 0;

	// ウィンドウが破棄されるときの処理
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	// ウィンドウが破棄されるときの処理
	case WM_NCDESTROY:
		// ウィンドウが破棄されるときに、ウィンドウハンドルからthisポインタを解除する
		m_hWnd = nullptr;
		SetWindowLongPtr(hWnd, GWLP_USERDATA, 0);
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}


void Window::SetResizeCallBack(ResizeCallBack callback)
{
	m_resizeCallback = std::move(callback);
}

HWND Window::GetHWND() const
{
	return m_hWnd;
}

int Window::GetWidth() const
{
	return m_width;
}

int Window::GetHeight() const
{
	return m_height;
}

bool Window::IsMinimized() const
{
	return m_isMinimized;
}
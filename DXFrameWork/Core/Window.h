#pragma once

#include <windows.h>
#include <functional>
#include <string>

// ウィンドウクラス
// Win64を管理するためのクラス
class Window
{
public:
	// ウィンドウのリサイズ時に呼ばれるコールバック関数
	using ResizeCallBack = std::function<void(int width, int height)>;
	
	Window() = default;
	~Window();

	// コピーの禁止
	Window(const Window&) = delete;				// コピーコンストラクタを削除
	Window& operator=(const Window&) = delete;	// コピー代入演算子を削除

	//クライアント領域が clientWidth x clientHeight のウィンドウを作成する
	bool Create(HINSTANCE hInstance, const std::wstring& title,
				int clientWidth, int clientHeight);

	void Show(int nCmdShow);

	//たまったメッセージを処理する。WM_QUIT が来たら false を返す
	bool PumpMessages();

	//リサイズ時に呼ばれる関数を登録する(SwapChainの再生成に使用)
	void SetResizeCallBack(ResizeCallBack callback);

	HWND GetHWND() const;
	int GetWidth() const;
	int GetHeight() const;
	bool IsMinimized() const;

	void SetTitle(const std::wstring& title);

private:
	// ウィンドウハンドル
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	HINSTANCE		m_hInstance = nullptr;			//アプリケーションインスタンスハンドル
	HWND			m_hWnd = nullptr;				//ウィンドウハンドル
	int				m_width = 0;					//ウィンドウの幅
	int				m_height = 0;					//ウィンドウの高さ
	bool			m_isMinimized = false;			//ウィンドウが最小化されているかどうか
	ResizeCallBack	m_resizeCallback = nullptr;		//リサイズ時に呼ばれるコールバック関数

};
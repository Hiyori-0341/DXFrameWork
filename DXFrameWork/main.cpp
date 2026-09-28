#include <windows.h>

//ウィンドウプロシージャ
LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	// デフォルトのウィンドウプロシージャを呼び出す
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//エントリーポイント
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow)
{
	//ウィンドウクラス登録
	WNDCLASSEX wc{};

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.lpszClassName = L"DXGameFrameWorkWindow";

	RegisterClassEx(&wc);

	//ウィンドウ作成
	HWND hWnd = CreateWindowEx(
		0,							//拡張スタイル
		wc.lpszClassName,			//クラス名
		L"DXGameFrameWork",			//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,		//ウィンドウスタイル
		CW_USEDEFAULT,				//表示X座標
		CW_USEDEFAULT,				//表示Y座標
		800,						//表示幅
		600,						//表示高さ
		nullptr,					//親ウィンドウハンドル
		nullptr,					//メニューハンドル
		hInstance,					//インスタンスハンドル
		nullptr						//追加パラメータ
	);

	if(hWnd == nullptr)
	{
		return -1;
	}

	//ウィンドウ表示
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	//ゲームループ
	MSG msg{};

	bool isRunning = true;

	while (isRunning)
	{
		//メッセージ処理
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				isRunning = false;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (!isRunning)		break;
		//ここにゲームの更新処理や描画処理を追加する
		//Update
		//Render
	}

	//終了処理
	return 0;
}

#include "Application.h"

// エントリポイント
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Application app;

	if (!app.Initialize(hInstance, nCmdShow))
	{
		return -1;
	}

	return app.Run();
}
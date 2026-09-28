#pragma once

#include <windows.h>
#include <cstdio>

// HRESULTを検査し、失敗したら出力ウィンドウにメッセージを出す
// 使い方: if (!CheckHR(hr, "CreateRenderTargetView")) return false;
inline bool CheckHR(HRESULT hr, const char* what)
{
	if (SUCCEEDED(hr))
	{
		return true;
	}

	char buffer[256]{};
	sprintf_s(buffer, "[D3D] %s failed (HRESULT: 0x%08X)\n", what, static_cast<unsigned int>(hr));
	OutputDebugStringA(buffer);
	return false;
}
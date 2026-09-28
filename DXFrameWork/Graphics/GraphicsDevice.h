#pragma once

#include <windows.h>
#include <d3d11.h>
#include <wrl/client.h>

// D3D11のデバイス、スワップチェーン、描画先(RTV/DSV)を管理するクラス
class GraphicsDevice
{
public:
	// デバイスとスワップチェーンを作成する
	bool Initialize(HWND hWnd, int width, int height);

	// ウィンドウのサイズが変わったときに、描画先を作り直す
	void Resize(int width, int height);

	// フレームの開始。描画先を設定し、画面と深度をクリアする
	void BeginFrame(const float clearColor[4]);

	// フレームの終了。画面に表示する
	void EndFrame(bool vsync = true);

	ID3D11Device* GetDevice() const { return m_device.Get(); }
	ID3D11DeviceContext* GetContext() const { return m_context.Get(); }

private:
	bool CreateRenderTargets();
	void ReleaseRenderTargets();

	Microsoft::WRL::ComPtr<ID3D11Device>           m_device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext>    m_context;
	Microsoft::WRL::ComPtr<IDXGISwapChain>         m_swapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>        m_depthTexture;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;

	int m_width = 0;
	int m_height = 0;
};
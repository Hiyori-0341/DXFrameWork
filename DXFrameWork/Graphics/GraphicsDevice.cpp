#include "GraphicsDevice.h"
#include "D3DHelper.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;

bool GraphicsDevice::Initialize(HWND hWnd, int width, int height)
{
	m_width = width;
	m_height = height;

	// スワップチェーンの設定
	DXGI_SWAP_CHAIN_DESC scd{};
	scd.BufferCount = 2;
	scd.BufferDesc.Width = width;
	scd.BufferDesc.Height = height;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = 1;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	const D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };

	UINT flags = 0;
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;	// デバッグレイヤー(警告やエラーが出力ウィンドウに出る)
#endif

	auto createDevice = [&](UINT createFlags)
		{
			return D3D11CreateDeviceAndSwapChain(
				nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createFlags,
				featureLevels, _countof(featureLevels), D3D11_SDK_VERSION,
				&scd, &m_swapChain, &m_device, nullptr, &m_context);
		};

	HRESULT hr = createDevice(flags);

#ifdef _DEBUG
	// デバッグレイヤーが入っていない環境では、フラグなしで作り直す
	if (FAILED(hr))
	{
		flags &= ~D3D11_CREATE_DEVICE_DEBUG;
		hr = createDevice(flags);
	}
#endif

	if (!CheckHR(hr, "D3D11CreateDeviceAndSwapChain"))
	{
		return false;
	}

	return CreateRenderTargets();
}

void GraphicsDevice::Resize(int width, int height)
{
	// 初期化前や、最小化(サイズ0)のときは何もしない
	if (!m_swapChain || width <= 0 || height <= 0)
	{
		return;
	}

	m_width = width;
	m_height = height;

	// バックバッファへの参照をすべて手放してから ResizeBuffers を呼ぶ
	m_context->OMSetRenderTargets(0, nullptr, nullptr);
	ReleaseRenderTargets();

	if (!CheckHR(m_swapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0), "ResizeBuffers"))
	{
		return;
	}

	CreateRenderTargets();
}

void GraphicsDevice::BeginFrame(const float clearColor[4])
{
	// 描画先とビューポートの設定
	ID3D11RenderTargetView* rtv = m_renderTargetView.Get();
	m_context->OMSetRenderTargets(1, &rtv, m_depthStencilView.Get());

	D3D11_VIEWPORT viewport{};
	viewport.Width = static_cast<float>(m_width);
	viewport.Height = static_cast<float>(m_height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	m_context->RSSetViewports(1, &viewport);

	// 画面と深度バッファのクリア
	m_context->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
	m_context->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void GraphicsDevice::EndFrame(bool vsync)
{
	// 第1引数が1なら垂直同期を待つ
	CheckHR(m_swapChain->Present(vsync ? 1 : 0, 0), "Present");
}

bool GraphicsDevice::CreateRenderTargets()
{
	// バックバッファからRTVを作成
	ComPtr<ID3D11Texture2D> backBuffer;
	if (!CheckHR(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)), "GetBuffer"))
	{
		return false;
	}
	if (!CheckHR(m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_renderTargetView),
		"CreateRenderTargetView"))
	{
		return false;
	}

	// 深度ステンシル用のテクスチャとDSVを作成
	D3D11_TEXTURE2D_DESC depthDesc{};
	depthDesc.Width = m_width;
	depthDesc.Height = m_height;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

	if (!CheckHR(m_device->CreateTexture2D(&depthDesc, nullptr, &m_depthTexture), "CreateTexture2D(depth)"))
	{
		return false;
	}
	if (!CheckHR(m_device->CreateDepthStencilView(m_depthTexture.Get(), nullptr, &m_depthStencilView),
		"CreateDepthStencilView"))
	{
		return false;
	}

	return true;
}

void GraphicsDevice::ReleaseRenderTargets()
{
	m_depthStencilView.Reset();
	m_depthTexture.Reset();
	m_renderTargetView.Reset();
}
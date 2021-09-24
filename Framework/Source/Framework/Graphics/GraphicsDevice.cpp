﻿#include "Pch.h"
#include "GraphicsDevice.h"
#include "../../Application/main.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
GraphicsDevice::GraphicsDevice()
	: g_cpDevice(nullptr)
	, g_cpContext(nullptr)
	, g_cpDeviceN(nullptr)
	, g_cpContextN(nullptr)
	, g_cpContextDeferred(nullptr)
{
}

//-----------------------------------------------------------------------------
// デバイス作成
//-----------------------------------------------------------------------------
bool GraphicsDevice::Initialize(MY_DIRECT3D_DESC desc)
{
	//--------------------------------------------------
	// デバイス/デバイスコンテキスト作成
	//--------------------------------------------------

	D3D_FEATURE_LEVEL featureLevel;
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	UINT flags = 0;

#ifdef _DEBUG
	// 詳細なデバッグ情報を取得する
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// DXGIファクトリー作成
	ComPtr<IDXGIFactory1> factory = nullptr;
	if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(factory.GetAddressOf())))) {
		DebugLog("ファクトリー作成失敗.\n");
		return false;
	}

	// アダプター設定
	ComPtr<IDXGIAdapter1> adapter;
	for (UINT adapterIndex = 0; S_OK == factory->EnumAdapters1(adapterIndex, adapter.GetAddressOf()); ++adapterIndex)
	{
		DXGI_ADAPTER_DESC1 desc;
		adapter->GetDesc1(&desc);
		// GPUアダプタ？
		if (desc.Flags ^= DXGI_ADAPTER_FLAG_SOFTWARE) {
			m_adapterName = wide_to_sjis(desc.Description);
			break;
		}
	}

	// デバイスとデバイスコンテキスト作成
	if (FAILED(D3D11CreateDevice(adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, flags, featureLevels,
		_countof(featureLevels), D3D11_SDK_VERSION, &g_cpDevice, &featureLevel, &g_cpContext))) {
		DebugLog("Direct3D11デバイス作成失敗.\n");
		return false;
	}

	//--------------------------------------------------
	// 現環境で使用できるMSAAをチェック
	//--------------------------------------------------

	DXGI_SAMPLE_DESC sampleDesc = {};
	for (int i = 1; i <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; i <<= 1)
	{
		UINT Quality;
		if (SUCCEEDED(g_cpDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_D24_UNORM_S8_UINT, i, &Quality)))
		{
			if (0 < Quality)
			{
				sampleDesc.Count = i;
				sampleDesc.Quality = Quality - 1;
			}
		}
	}

	sampleDesc.Count = desc.m_useMSAA ? sampleDesc.Count : 1;

	// MSAA非対応の場合
	if (sampleDesc.Count <= 1) {
		// MSAAをOFFにします
		sampleDesc.Quality = 0;
		desc.m_useMSAA = false;
		APP.g_imGuiSystem->AddLog("MSAA is not supported.");
	}

	//--------------------------------------------------
	// スワップチェイン作成
	//--------------------------------------------------

	// スワップチェーンの設定データ
	DXGI_SWAP_CHAIN_DESC DXGISwapChainDesc = {};
	DXGISwapChainDesc.BufferDesc.Width					= desc.m_width;
	DXGISwapChainDesc.BufferDesc.Height					= desc.m_height;
	DXGISwapChainDesc.BufferDesc.RefreshRate.Numerator	= 0;
	DXGISwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	DXGISwapChainDesc.BufferDesc.Format					= desc.m_useHDR ? DXGI_FORMAT_R16G16B16A16_FLOAT : DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGISwapChainDesc.BufferDesc.ScanlineOrdering		= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	DXGISwapChainDesc.BufferDesc.Scaling				= DXGI_MODE_SCALING_UNSPECIFIED;

	DXGISwapChainDesc.SampleDesc						= sampleDesc;

	DXGISwapChainDesc.BufferUsage						= DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	DXGISwapChainDesc.BufferCount						= desc.m_bufferCount;
	DXGISwapChainDesc.OutputWindow						= desc.m_hwnd;
	DXGISwapChainDesc.Windowed							= desc.m_windowed;
	DXGISwapChainDesc.SwapEffect						= DXGISwapChainDesc.BufferCount >= 2 ? DXGI_SWAP_EFFECT_DISCARD : DXGI_SWAP_EFFECT_SEQUENTIAL;
	DXGISwapChainDesc.Flags								= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// スワップチェイン作成
	if (FAILED(factory->CreateSwapChain(g_cpDevice.Get(), &DXGISwapChainDesc, m_cpGISwapChain.ReleaseAndGetAddressOf()))) {
		DebugLog("スワップチェイン作成失敗.\n");
		return false;
	}

	// スワップチェインからバックバッファ取得
	ComPtr<ID3D11Texture2D> pBackBuffer;
	if (FAILED(m_cpGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)pBackBuffer.GetAddressOf()))) {
		DebugLog("バックバッファ取得失敗.\n");
		return false;
	}

	// ALT+Enterでフルスクリーン不可にする
	{
		IDXGIDevice* pDXGIDevice;
		g_cpDevice->QueryInterface<IDXGIDevice>(&pDXGIDevice);

		IDXGIAdapter* pDXGIAdapter;
		pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pDXGIAdapter);

		IDXGIFactory* pIDXGIFactory;
		pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pIDXGIFactory);

		pIDXGIFactory->MakeWindowAssociation(desc.m_hwnd, DXGI_MWA_NO_ALT_ENTER);

		pDXGIDevice->Release();
		pDXGIAdapter->Release();
		pIDXGIFactory->Release();
	}

	//--------------------------------------------------
	// レンダーターゲット/ビューポート作成
	//--------------------------------------------------

	// バックバッファ用テクスチャ管理クラス作成
	m_spBackbuffer = std::make_shared<Texture>();
	if (!m_spBackbuffer->Create(pBackBuffer.Get(), desc.m_useMSAA)) {
		DebugLog("バックバッファ作成失敗.\n");
		return false;
	}
	// Zバッファ用テクスチャ管理クラス作成
	m_spDefaultZbuffer = std::make_shared<Texture>();
	if (!m_spDefaultZbuffer->CreateDepthStencil(desc.m_height, desc.m_width, desc.m_useMSAA)) {
		DebugLog("Zバッファ作成失敗.\n");
		return false;
	}

	// ビューポート変換行列の登録
	D3D11_VIEWPORT vp = {};
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width	= static_cast<float>(desc.m_width);
	vp.Height	= static_cast<float>(desc.m_height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	g_cpContext->RSSetViewports(1, &vp);

	// レンダーターゲット設定
	g_cpContext->OMSetRenderTargets(1, m_spBackbuffer->RTVAddress(), m_spDefaultZbuffer->DSV());

	//--------------------------------------------------
	// 遅延デバイスコンテキスト作成
	//--------------------------------------------------

	// 読み込み用遅延コンテキスト作成
	g_cpDevice->CreateDeferredContext(0, &g_cpContextDeferred);

	// RT/VP設定
	g_cpContextDeferred->RSSetViewports(1, &vp);
	g_cpContextDeferred->OMSetRenderTargets(1, m_spBackbuffer->RTVAddress(), m_spDefaultZbuffer->DSV());

	//--------------------------------------------------
	// 1x1の白テクスチャ作成
	//--------------------------------------------------
	{
		// 0xAABBGGRR
		auto col = cfloat4x4(1, 1, 1, 1).RGBA();
		D3D11_SUBRESOURCE_DATA srdata;
		srdata.pSysMem			= &col;
		srdata.SysMemPitch		= 4;
		srdata.SysMemSlicePitch = 0;

		m_texWhite = std::make_shared<Texture>();
		m_texWhite->Create("Resource/Texture/White1x1.bmp");
	}

	//--------------------------------------------------
	// 1x1のZ向き法線マップ作成
	//--------------------------------------------------
	{
		auto col = cfloat4x4(0.5f, 0.5f, 1.0f, 1).RGBA();
		D3D11_SUBRESOURCE_DATA srdata;
		srdata.pSysMem			= &col;
		srdata.SysMemPitch		= 4;
		srdata.SysMemSlicePitch = 0;

		m_texNormal = std::make_shared<Texture>();
		m_texNormal->Create("Resource/Texture/Blue1x1.bmp");// todo: fix
	}

	APP.g_imGuiSystem->AddLog("INFO: GraphicsDevice Initialized.");

	return true;
}

//-----------------------------------------------------------------------------
// 解放
//-----------------------------------------------------------------------------
void GraphicsDevice::Finalize()
{
	m_spBackbuffer = nullptr;
	m_spDefaultZbuffer = nullptr;
}

//-----------------------------------------------------------------------------
// 開始
//-----------------------------------------------------------------------------
void GraphicsDevice::Begin(const float* clearColor)
{
	g_cpContext->OMSetRenderTargets(1, m_spBackbuffer->RTVAddress(), m_spDefaultZbuffer->DSV());

	constexpr float zeroClear[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	g_cpContext->ClearRenderTargetView(m_spBackbuffer->RTV(), clearColor ? clearColor : zeroClear);
	g_cpContext->ClearDepthStencilView(m_spDefaultZbuffer->DSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void GraphicsDevice::End(UINT syncInterval, UINT flags)
{
	// TODO: なぜか全画面だと垂直同期が切れる
	HRESULT hr = m_cpGISwapChain->Present(syncInterval, flags);
	if (FAILED(hr))
		assert(0 && "エラー：画面更新の失敗.");
	if (hr == DXGI_ERROR_DEVICE_REMOVED)
		assert(0 && "エラー：ビデオカードがシステムから物理的に取り外されたか アップデートが行われました");
}

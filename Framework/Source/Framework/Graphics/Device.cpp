#include "Device.h"
#include "../../Application/ImGuiSystem.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Device::Device()
	: m_cpDevice(nullptr)
	, m_cpImmediateContext(nullptr)
	, m_cpGISwapChain(nullptr)
	, m_cpDefferedContext(nullptr)
	, m_cpCommandList(nullptr)
	, m_spRenderer(nullptr)
	, m_spBackbuffer(nullptr)
	, m_spDefaultZbuffer(nullptr)
	, m_adapterName(L"")
	, m_useVerticalSync(false)
	, m_useMSAA(true)
{
}

//-----------------------------------------------------------------------------
// デバイス.スワップチェインの作成
//-----------------------------------------------------------------------------
bool Device::Create( const HWND& hWnd, bool isUseHDR, int width, int height, bool isFullScreen, bool isDebug, std::string& resultErrMsg )
{
	//--------------------------------------------------
	// デバイス/デバイスコンテキスト作成
	//--------------------------------------------------

	// 使用するDirectXバージョン(上から順に使っていく)
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,	// Direct3D 11.1  ShaderModel 5
		D3D_FEATURE_LEVEL_11_0,	// Direct3D 11.0  ShaderModel 5
		D3D_FEATURE_LEVEL_10_1,	// Direct3D 10.1  ShaderModel 4
		D3D_FEATURE_LEVEL_10_0,	// Direct3D 10.0  ShaderModel 4
		D3D_FEATURE_LEVEL_9_3,	// Direct3D  9.3  ShaderModel 3
		D3D_FEATURE_LEVEL_9_2,	// Direct3D  9.2  ShaderModel 3
		D3D_FEATURE_LEVEL_9_1,	// Direct3D  9.1  ShaderModel 3
	};
	D3D_FEATURE_LEVEL futureLevel; // 実際に使用されたバージョン

	// Debug情報取得
	UINT creationFlags = 0;

	// 詳細なデバッグ情報を取得するためのフラグ
	if (isDebug)
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;

	// DXGIファクトリー作成
	ComPtr<IDXGIFactory1> factory = nullptr;
	if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(factory.GetAddressOf())))) {
		resultErrMsg = "ファクトリー作成失敗";
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
			m_adapterName = desc.Description;
			break;// done
		}
	}

	// デバイスとデバイスコンテキスト作成
	if (FAILED(D3D11CreateDevice(adapter.Get(), D3D_DRIVER_TYPE_UNKNOWN, nullptr, creationFlags, featureLevels,
		_countof(featureLevels), D3D11_SDK_VERSION, &m_cpDevice, &futureLevel, &m_cpImmediateContext))) {
		resultErrMsg = "エラー：Direct3D11デバイス作成失敗.";
		return false;
	}

	//--------------------------------------------------
	// 現環境で使用できるMSAAをチェック
	//--------------------------------------------------

	DXGI_SAMPLE_DESC sampleDesc = {};
	for (int i = 1; i <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; i <<= 1)
	{
		UINT Quality;
		if (SUCCEEDED(m_cpDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_D24_UNORM_S8_UINT, i, &Quality)))
		{
			if (0 < Quality)
			{
				sampleDesc.Count = i;
				sampleDesc.Quality = Quality - 1;
			}
		}
	}

	// MSAA OFFの場合
	sampleDesc.Count = m_useMSAA ? sampleDesc.Count : 1;

	//--------------------------------------------------
	// スワップチェイン作成
	//--------------------------------------------------

	// スワップチェーンの設定データ
	DXGI_SWAP_CHAIN_DESC DXGISwapChainDesc = {};
	DXGISwapChainDesc.BufferDesc.Width						= width;
	DXGISwapChainDesc.BufferDesc.Height						= height;
	DXGISwapChainDesc.BufferDesc.RefreshRate.Numerator		= 0;
	DXGISwapChainDesc.BufferDesc.RefreshRate.Denominator	= 1;
	DXGISwapChainDesc.BufferDesc.Format						= isUseHDR ? DXGI_FORMAT_R16G16B16A16_FLOAT : DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGISwapChainDesc.BufferDesc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	DXGISwapChainDesc.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;

	DXGISwapChainDesc.SampleDesc							= sampleDesc;

	DXGISwapChainDesc.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
	DXGISwapChainDesc.BufferCount							= 2;
	DXGISwapChainDesc.OutputWindow							= hWnd;
	DXGISwapChainDesc.Windowed								= TRUE;
	DXGISwapChainDesc.SwapEffect							= DXGISwapChainDesc.BufferCount >= 2 ? DXGI_SWAP_EFFECT_DISCARD : DXGI_SWAP_EFFECT_SEQUENTIAL;
	DXGISwapChainDesc.Flags									= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// スワップチェイン作成
	if (FAILED(factory->CreateSwapChain(m_cpDevice.Get(), &DXGISwapChainDesc, m_cpGISwapChain.ReleaseAndGetAddressOf()))) {
		resultErrMsg = "スワップチェイン作成失敗";
		return false;
	}

	// スワップチェインからバックバッファ取得
	ComPtr<ID3D11Texture2D> pBackBuffer;
	if (FAILED(m_cpGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)pBackBuffer.GetAddressOf()))) {
		resultErrMsg = "バックバッファ取得失敗";
		return false;
	}

	// ALT+Enterでフルスクリーン不可にする
	{
		IDXGIDevice* pDXGIDevice;
		m_cpDevice->QueryInterface<IDXGIDevice>(&pDXGIDevice);

		IDXGIAdapter* pDXGIAdapter;
		pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);

		IDXGIFactory* pIDXGIFactory;
		pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void **)&pIDXGIFactory);

		pIDXGIFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);

		pDXGIDevice->Release();
		pDXGIAdapter->Release();
		pIDXGIFactory->Release();
	}

	//--------------------------------------------------
	// レンダーターゲット/ビューポート作成
	//--------------------------------------------------

	// バックバッファ用テクスチャ管理クラス作成
	m_spBackbuffer = std::make_shared<Texture>();
	if (!m_spBackbuffer->Create(pBackBuffer.Get(), m_useMSAA)) {
		resultErrMsg = "バックバッファ作成失敗";
		return false;
	}
	// Zバッファ用テクスチャ管理クラス作成
	m_spDefaultZbuffer = std::make_shared<Texture>();
	if (!m_spDefaultZbuffer->CreateDepthStencil(height, width, m_useMSAA)) {
		resultErrMsg = "Zバッファ作成失敗";
		return false;
	}

	// ビューポート変換行列の登録
	D3D11_VIEWPORT vp = {};
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width	= static_cast<float>(width);
	vp.Height	= static_cast<float>(height);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	m_cpImmediateContext->RSSetViewports(1, &vp);

	// レンダーターゲット設定
	m_cpImmediateContext->OMSetRenderTargets(1, m_spBackbuffer->RTVAddress(), m_spDefaultZbuffer->DSV());

	//--------------------------------------------------
	// 読み込み用 デバイスコンテキスト作成
	//--------------------------------------------------

	// 読み込み用遅延コンテキスト作成
	m_cpDevice->CreateDeferredContext(0, &m_cpDefferedContext);

	// RT/VP設定
	m_cpDefferedContext->RSSetViewports(1, &vp);
	m_cpDefferedContext->OMSetRenderTargets(1, m_spBackbuffer->RTVAddress(), m_spDefaultZbuffer->DSV());

	//--------------------------------------------------
	// 描画管理
	m_spRenderer = std::make_shared<Renderer>();
	m_spRenderer->Initialize();

	//--------------------------------------------------
	// 1x1の白テクスチャ作成
	//--------------------------------------------------
	{
		// 0xAABBGGRR
		auto col = cfloat4x4(1, 1, 1, 1).RGBA();
		D3D11_SUBRESOURCE_DATA srdata;
		srdata.pSysMem = &col;
		srdata.SysMemPitch = 4;
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
		srdata.pSysMem = &col;
		srdata.SysMemPitch = 4;
		srdata.SysMemSlicePitch = 0;

		m_texNormal = std::make_shared<Texture>();
		m_texNormal->Create("Resource/Texture/Blue1x1.bmp");
	}

	IMGUISYSTEM.AddLog("INFO: Direct3D initialized.");

	return true;
}

//-----------------------------------------------------------------------------
// 解放
//-----------------------------------------------------------------------------
void Device::Release()
{
	m_spBackbuffer = nullptr;
	m_spDefaultZbuffer = nullptr;
}

//-----------------------------------------------------------------------------
// 描画開始とバッファクリア
//-----------------------------------------------------------------------------
void Device::Begin( const float* clearColor )
{
	// レンダーターゲット設定(デフォルトに戻す)
	m_cpImmediateContext->OMSetRenderTargets(1, m_spBackbuffer->RTVAddress(), m_spDefaultZbuffer->DSV());

	constexpr float zeroClear[4] = { 0.0f, 0.0f, 1.0f, 1.0f };

	// バックバッファクリア
	m_cpImmediateContext->ClearRenderTargetView(m_spBackbuffer->RTV(), clearColor ? clearColor : zeroClear);
	// デプスステンシルビュークリア
	m_cpImmediateContext->ClearDepthStencilView(m_spDefaultZbuffer->DSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

	// ライト情報 更新
	m_spRenderer->LightCommit();
}

//-----------------------------------------------------------------------------
// 描画終了と画面更新
//-----------------------------------------------------------------------------
void Device::End()
{
	// TODO: なぜか全画面だと垂直同期が切れる
	HRESULT hr = m_cpGISwapChain->Present(m_useVerticalSync ? 1 : 0, 0);
	if (FAILED(hr))
		assert(0 && "エラー：画面更新の失敗.");
	if (hr == DXGI_ERROR_DEVICE_REMOVED)
		assert(0 && "エラー：ビデオカードがシステムから物理的に取り外されたか、アップデートが行われました。ドライバー.");
}

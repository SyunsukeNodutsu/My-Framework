#include "Pch.h"
#include "GraphicsDevice.h"
#include "../../Application/main.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
GraphicsDevice::GraphicsDevice()
	: g_cpDevice(nullptr)
	, g_cpContext(nullptr)
	, g_viewport()
	, m_cpGISwapChain(nullptr)
	, m_spBackbuffer(nullptr)
	, m_spDefaultZbuffer(nullptr)
	, m_adapterName("")
	, m_sampleDesc()
	, m_tempFixedVertexBuffer()
	, m_tempVertexBuffer(nullptr)
	, m_texWhite(nullptr)
	, m_texNormal(nullptr)
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

	// 詳細なデバッグ情報を取得する
	if (desc.m_debugMode)
		flags |= D3D11_CREATE_DEVICE_DEBUG;

	// DXGIファクトリー作成
	ComPtr<IDXGIFactory1> factory = nullptr;
	if (FAILED(CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(factory.GetAddressOf())))) {
		DebugLog("ファクトリー作成失敗.\n");
		return false;
	}

	// アダプター設定
	std::vector <IDXGIAdapter*> adapters;
	IDXGIAdapter* tmpAdapter = nullptr;
	for (int i = 0; factory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
		adapters.push_back(tmpAdapter);

	// TODO: 調査.11/02
	// 直近のwin10のアップデートでGPUアダプタの確認がバグったので
	// "NVIDIA"の文字列からGPUアダプタを取得
	for (auto adpt : adapters)
	{
		DXGI_ADAPTER_DESC adesc = {};
		adpt->GetDesc(&adesc);
		std::wstring strDesc = adesc.Description;
		if (strDesc.find(L"NVIDIA") != std::string::npos)
		{
			DebugLog(std::string("ビデオメモリ：" + std::to_string(adesc.DedicatedVideoMemory) + "\n").c_str());

			tmpAdapter = adpt;
			m_adapterName = wide_to_sjis(strDesc);
			break;
		}
	}

	// デバイスとデバイスコンテキスト作成
	if (FAILED(D3D11CreateDevice(tmpAdapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, flags, featureLevels,
		_countof(featureLevels), D3D11_SDK_VERSION, &g_cpDevice, &featureLevel, &g_cpContext))) {
		DebugLog("Direct3D11デバイス作成失敗.\n");
		return false;
	}

	//--------------------------------------------------
	// 現環境で使用できるMSAAをチェック
	//--------------------------------------------------

	for (int i = 1; i <= D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT; i <<= 1) {
		UINT Quality;
		if (SUCCEEDED(g_cpDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_D24_UNORM_S8_UINT, i, &Quality))) {
			if (0 < Quality) {
				m_sampleDesc.Count = i;
				m_sampleDesc.Quality = Quality - 1;
			}
		}
	}

	m_sampleDesc.Count = desc.m_useMSAA ? m_sampleDesc.Count : 1;

	// MSAA非対応の場合
	if (m_sampleDesc.Count <= 1) {
		// MSAAをOFFにします
		m_sampleDesc.Quality = 0;
		desc.m_useMSAA = false;
		ApplicationChilled::GetApplication()->g_imGuiSystem->AddLog("MSAA is not supported.");
	}

	ApplicationChilled::GetApplication()->g_imGuiSystem->AddLog(
		std::string("INFO: SampleDesc Count: " + std::to_string(m_sampleDesc.Count) + " Quality: " + std::to_string(m_sampleDesc.Quality)).c_str()
	);

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

	DXGISwapChainDesc.SampleDesc						= m_sampleDesc;

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
	/*{
		IDXGIDevice* pDXGIDevice;
		g_cpDevice->QueryInterface<IDXGIDevice>(&pDXGIDevice);

		IDXGIAdapter* pDXGIAdapter;
		pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pDXGIAdapter);

		IDXGIFactory* pIDXGIFactory;
		pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pIDXGIFactory);

		pIDXGIFactory->MakeWindowAssociation(desc.m_hwnd, DXGI_MWA_NO_ALT_ENTER);

		SafeRelease(pDXGIDevice);
		SafeRelease(pDXGIAdapter);
		SafeRelease(pIDXGIFactory);
	}*/

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
	g_viewport.TopLeftX = 0;
	g_viewport.TopLeftY = 0;
	g_viewport.Width	= static_cast<float>(desc.m_width);
	g_viewport.Height	= static_cast<float>(desc.m_height);
	g_viewport.MinDepth = D3D11_MIN_DEPTH;
	g_viewport.MaxDepth = D3D11_MAX_DEPTH;
	g_cpContext->RSSetViewports(1, &g_viewport);

	// レンダーターゲット設定
	g_cpContext->OMSetRenderTargets(1, m_spBackbuffer->RTVAddress(), m_spDefaultZbuffer->DSV());

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
		m_texWhite->Create(1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, 1, &srdata);
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
		m_texWhite->Create(1, 1, DXGI_FORMAT_R8G8B8A8_UNORM, 1, &srdata);
	}

	// DrawVertices用頂点バッファを作成
	UINT bufferSize = 80;
	for (int i = 0; i < 10; i++)
	{
		m_tempFixedVertexBuffer[i] = std::make_shared<Buffer>();
		m_tempFixedVertexBuffer[i]->Create(D3D11_BIND_VERTEX_BUFFER, bufferSize, D3D11_USAGE_DYNAMIC, nullptr);
		bufferSize *= 2;
	}

	m_tempVertexBuffer = std::make_shared<Buffer>();

	ApplicationChilled::GetApplication()->g_imGuiSystem->AddLog("INFO: GraphicsDevice Initialized.");

	auto renderer = ::EffekseerRendererDX11::Renderer::Create(g_cpDevice.Get(), g_cpContext.Get(), 8000);
	renderer.Reset();

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
void GraphicsDevice::Begin(const cfloat4x4 clearColor)
{
	g_cpContext->ClearRenderTargetView(m_spBackbuffer->RTV(), clearColor);
	g_cpContext->ClearDepthStencilView(m_spDefaultZbuffer->DSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

	g_cpContext->RSSetViewports(1, &g_viewport);

	g_cpContext->OMSetRenderTargets(1, m_spBackbuffer->RTVAddress(), m_spDefaultZbuffer->DSV());
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

//-----------------------------------------------------------------------------
// 頂点を描画する簡易的な関数
//-----------------------------------------------------------------------------
void GraphicsDevice::DrawVertices(D3D_PRIMITIVE_TOPOLOGY topology, int vertexCount, const void* pVertexStream, UINT stride)
{
	// 全頂点の総バイトサイズ
	UINT totalSize = vertexCount * stride;

	// 最適な固定長バッファを検索
	std::shared_ptr<Buffer> buffer = nullptr;
	for (auto&& n : m_tempFixedVertexBuffer)
	{
		if (totalSize < n->GetSize()) {
			buffer = n; break;
		}
	}

	// 可変長のバッファを使用
	if (buffer == nullptr)
	{
		buffer = m_tempVertexBuffer;

		// 頂点バッファのサイズが小さいときは、リサイズのため再作成する
		if (m_tempVertexBuffer->GetSize() < totalSize)
			m_tempVertexBuffer->Create(D3D11_BIND_VERTEX_BUFFER, totalSize, D3D11_USAGE_DYNAMIC, nullptr);
	}

	// 単純なDISCARDでの書き込み TODO: 修正
	buffer->WriteData(pVertexStream, totalSize);

	// バインド
	{
		g_cpContext.Get()->IASetPrimitiveTopology(topology);

		UINT offset = 0;
		g_cpContext.Get()->IASetVertexBuffers(0, 1, buffer->GetAddress(), &stride, &offset);
	}

	g_cpContext.Get()->Draw(vertexCount, 0);
}

//-----------------------------------------------------------------------------
// 構造化バッファーをもとにSRVを作成して返す
//-----------------------------------------------------------------------------
const HRESULT GraphicsDevice::CreateBufferSRV(ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut)
{
	D3D11_BUFFER_DESC descBuf = {};
	pBuffer->GetDesc(&descBuf);

	D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.BufferEx.FirstElement = 0;
	desc.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;

	return g_cpDevice.Get()->CreateShaderResourceView(pBuffer, &desc, ppSRVOut);
}

//-----------------------------------------------------------------------------
// 構造化バッファーをもとにUAVを作成して返す
//-----------------------------------------------------------------------------
const HRESULT GraphicsDevice::CreateBufferUAV(ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAVOut)
{
	D3D11_BUFFER_DESC descBuf = {};
	pBuffer->GetDesc(&descBuf);

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;

	return g_cpDevice.Get()->CreateUnorderedAccessView(pBuffer, &desc, ppUAVOut);
}

/*
* t ... シェーダー リソース ビュー(SRV)用
* s ... サンプラー用
* u ... 順序指定されていないアクセス ビュー(UAV)用
* b ... 定数バッファー ビュー(CBV)用
*/

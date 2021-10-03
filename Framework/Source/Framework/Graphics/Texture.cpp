#include "Texture.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Texture::Texture()
	: m_cpBuffer(nullptr)
	, m_desc()
	, m_cpRTV(nullptr)
	, m_cpSRV(nullptr)
	, m_cpDSV(nullptr)
{
}

//-----------------------------------------------------------------------------
// テクスチャリソースから作成
//-----------------------------------------------------------------------------
bool Texture::Create( ID3D11Texture2D* pTexBuffer, bool useMSAA )
{
	if (pTexBuffer == nullptr)
		return false;

	m_cpBuffer = pTexBuffer;
	// 指定されたバッファから情報を抜き出す
	pTexBuffer->GetDesc(&m_desc);

	return CreateRTV(useMSAA);
}

//-----------------------------------------------------------------------------
// ファイル名から作成
//-----------------------------------------------------------------------------
bool Texture::Create( const std::string& filepath )
{
	if (filepath.empty())
		return false;

	// マルチバイト -> ワイドバイト
	std::wstring wfilepath = sjis_to_wide(filepath.c_str());

	// 読み込み ※MipMapを作成するにはContextが必要
	HRESULT hr = DirectX::CreateWICTextureFromFile(
		g_graphicsDevice->g_cpDevice.Get(), g_graphicsDevice->g_cpContext.Get(), wfilepath.c_str(),
		(ID3D11Resource**)m_cpBuffer.GetAddressOf(), m_cpSRV.GetAddressOf()
	);

	if (FAILED(hr)) {
		DebugLog("WARNING: テクスチャ読み込み失敗.\n");
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// DESC情報から作成
//-----------------------------------------------------------------------------
bool Texture::Create(const D3D11_TEXTURE2D_DESC& desc)
{
	HRESULT hr = g_graphicsDevice->g_cpDevice.Get()->CreateTexture2D(&desc, nullptr, m_cpBuffer.GetAddressOf());
	if (FAILED(hr)) {
		assert(0 && "エラー：テクスチャバッファ作成失敗.");
		return false;
	}
	
	// DESC情報を取得
	m_cpBuffer->GetDesc(&m_desc);

	return true;
}

//-----------------------------------------------------------------------------
// 作成
//-----------------------------------------------------------------------------
bool Texture::CreateRenderTarget(int height, int width, bool useMSAA, DXGI_FORMAT format, UINT arrayCnt)
{
	auto& smpleDesc = g_graphicsDevice->GetSampleDesc();

	D3D11_TEXTURE2D_DESC desc = {};

	desc.Usage				= D3D11_USAGE_DEFAULT;
	desc.Format				= format;

	// ↓読み書き兼用で作成
	desc.BindFlags			= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.Width				= static_cast<UINT>(width);
	desc.Height				= static_cast<UINT>(height);
	desc.CPUAccessFlags		= 0;
	desc.MipLevels			= 1;
	desc.ArraySize			= arrayCnt;
	desc.MiscFlags			= 0;
	desc.SampleDesc.Count	= useMSAA ? smpleDesc.Count : 1;
	desc.SampleDesc.Quality = useMSAA ? smpleDesc.Quality : 0;
	
	if (!Create(desc))
		return false;

	// Viewの作成
	if (!CreateRTV(useMSAA))
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// 深度テクスチャとして作成
//-----------------------------------------------------------------------------
bool Texture::CreateDepthStencil(int height, int width, bool useMSAA, DXGI_FORMAT format)
{
	auto& smpleDesc = g_graphicsDevice->GetSampleDesc();

	D3D11_TEXTURE2D_DESC desc = {};

	// 種類
	desc.Format				= format;
	desc.BindFlags			= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;

	desc.Height				= static_cast<UINT>(height);
	desc.Width				= static_cast<UINT>(width);

	// アクセスの種類
	desc.Usage				= D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags		= 0;

	desc.MipLevels			= 1;
	desc.ArraySize			= 1;

	// その他の設定
	desc.MiscFlags			= 0;
	desc.SampleDesc.Count	= useMSAA ? smpleDesc.Count : 1;
	desc.SampleDesc.Quality = useMSAA ? smpleDesc.Quality : 0;

	if (!Create(desc))
		return false;

	// Viewの作成
	if (!CreateDSV(useMSAA))
		return false;

	return true;
}

//-----------------------------------------------------------------------------
// リソースを取得
//-----------------------------------------------------------------------------
ID3D11Texture2D* Texture::GetResource() const
{
	if (m_cpSRV == nullptr)
		return nullptr;

	ID3D11Resource* res;
	m_cpSRV->GetResource(&res);

	ID3D11Texture2D* result;
	if (FAILED(res->QueryInterface<ID3D11Texture2D>(&result))) {
		res->Release();
		return nullptr;
	}

	res->Release();
	result->Release();

	return result;
}

//-----------------------------------------------------------------------------
// RTV作成
//-----------------------------------------------------------------------------
bool Texture::CreateRTV(bool useMSAA)
{
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format			= m_desc.Format;
	rtvDesc.ViewDimension	= useMSAA ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;

	// RTV作成
	HRESULT hr = g_graphicsDevice->g_cpDevice.Get()->CreateRenderTargetView(m_cpBuffer.Get(), &rtvDesc, &m_cpRTV);
	if (FAILED(hr)) {
		assert(0 && "エラー：RenderTargetView作成失敗.");
		return false;
	}

	if (m_desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		// 作成するビューの設定
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

		// テクスチャがZバッファの場合は、最適なフォーマットにする
		if (m_desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
		{
			switch (m_desc.Format)
			{
			case DXGI_FORMAT_R16_TYPELESS: srvDesc.Format = DXGI_FORMAT_R16_UNORM; break;// 16ビット
			case DXGI_FORMAT_R32_TYPELESS: srvDesc.Format = DXGI_FORMAT_R32_FLOAT; break;// 32ビット
			case DXGI_FORMAT_R24G8_TYPELESS: srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; break;// 24ビット(Zバッファ) + 8ビット(ステンシルバッファ) 
			default: assert(0 && "[ShaderResource] 対応していないフォーマットです"); break;
			}
		}
		// Zバッファでない場合は、そのまま同じフォーマットを使用
		else srvDesc.Format = m_desc.Format;

		// 単品のテクスチャ(通常テクスチャ)の場合
		if (m_desc.ArraySize == 1)
		{
			srvDesc.ViewDimension = useMSAA ? D3D11_SRV_DIMENSION_TEXTURE2DMS : D3D11_SRV_DIMENSION_TEXTURE2D;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = m_desc.MipLevels;
			if (srvDesc.Texture2D.MipLevels <= 0) srvDesc.Texture2D.MipLevels = -1;
		}
		// テクスチャ配列の場合
		else
		{
			// さらにキューブマップの場合
			if (m_desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE) srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
			// 通常テクスチャ配列
			else srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;

			srvDesc.Texture2DArray.MostDetailedMip = 0;
			srvDesc.Texture2DArray.MipLevels = m_desc.MipLevels;
			srvDesc.Texture2DArray.ArraySize = m_desc.ArraySize;	// 要素数
			srvDesc.Texture2DArray.FirstArraySlice = 0;
		}

		// シェーダリソースビュー作成
		HRESULT hr = g_graphicsDevice->g_cpDevice.Get()->CreateShaderResourceView(m_cpBuffer.Get(), &srvDesc, &m_cpSRV);
		if (FAILED(hr))
		{
			assert(0 && "ShaderResourceViewの作成に失敗");
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// DSV作成
//-----------------------------------------------------------------------------
bool Texture::CreateDSV(bool useMSAA)
{
	D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};

	// 互換性を確認し最適なフォーマットを指定
	switch (m_desc.Format)
	{
		// 16bit
	case DXGI_FORMAT_R16_TYPELESS:
		desc.Format = DXGI_FORMAT_R16_UNORM;
		break;

		// 32bit
	case DXGI_FORMAT_R32_TYPELESS:
		desc.Format = DXGI_FORMAT_D32_FLOAT;
		break;

		// 24bit(Zバッファ) + 8bit(ステンシルバッファ)
	case DXGI_FORMAT_R24G8_TYPELESS:
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		break;
	}

	// 単品の場合
	desc.ViewDimension = useMSAA ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice = 0;

	// 作成
	HRESULT hr = g_graphicsDevice->g_cpDevice.Get()->CreateDepthStencilView(m_cpBuffer.Get(), &desc, m_cpDSV.GetAddressOf());
	if (FAILED(hr)) {
		assert(0 && "エラー：DepthStencilView作成失敗.");
		return false;
	}

	return true;
}

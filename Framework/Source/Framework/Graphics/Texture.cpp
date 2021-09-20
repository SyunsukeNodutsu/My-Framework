#include "Texture.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Texture::Texture()
	: m_cpBuffer(nullptr)
	, m_info()
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
	pTexBuffer->GetDesc(&m_info);

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
		m_graphicsDevice->g_cpDevice.Get(), m_graphicsDevice->g_cpContext.Get(), wfilepath.c_str(),
		(ID3D11Resource**)m_cpBuffer.GetAddressOf(), m_cpSRV.GetAddressOf()
	);

	if (FAILED(hr)) {
		DebugLog("[WARNING]テクスチャ読み込み失敗.\n");
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// DESC情報から作成
//-----------------------------------------------------------------------------
bool Texture::Create(D3D11_TEXTURE2D_DESC& desc)
{
	HRESULT hr = m_graphicsDevice->g_cpDevice.Get()->CreateTexture2D(&desc, nullptr, m_cpBuffer.GetAddressOf());
	if (FAILED(hr)) {
		assert(0 && "エラー：テクスチャバッファ作成失敗.");
		return false;
	}

	m_cpBuffer->GetDesc(&m_info);

	return true;
}

//-----------------------------------------------------------------------------
// 深度テクスチャとして作成
//-----------------------------------------------------------------------------
bool Texture::CreateDepthStencil(int height, int width, bool useMSAA, DXGI_FORMAT format)
{
	D3D11_TEXTURE2D_DESC desc = {};

	// 種類
	desc.Format = format;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;

	desc.Height = static_cast<UINT>(height);
	desc.Width = static_cast<UINT>(width);

	// アクセスの種類
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;

	desc.MipLevels = 1;
	desc.ArraySize = 1;

	// その他の設定
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = useMSAA ? 8 : 1;
	desc.SampleDesc.Quality = 0;

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
	rtvDesc.Format			= m_info.Format;
	rtvDesc.ViewDimension	= useMSAA ? D3D11_RTV_DIMENSION_TEXTURE2DMS : D3D11_RTV_DIMENSION_TEXTURE2D;

	// RTV作成
	HRESULT hr = m_graphicsDevice->g_cpDevice.Get()->CreateRenderTargetView(m_cpBuffer.Get(), &rtvDesc, &m_cpRTV);
	if (FAILED(hr)) {
		assert(0 && "エラー：RenderTargetView作成失敗.");
		return false;
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
	switch (m_info.Format)
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
	HRESULT hr = m_graphicsDevice->g_cpDevice.Get()->CreateDepthStencilView(m_cpBuffer.Get(), &desc, m_cpDSV.GetAddressOf());
	if (FAILED(hr)) {
		assert(0 && "エラー：DepthStencilView作成失敗.");
		return false;
	}

	return true;
}

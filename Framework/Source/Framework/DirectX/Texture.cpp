#include "Texture.h"

bool CreateRenderTargetViewZ(ID3D11Texture2D* resource, D3D11_TEXTURE2D_DESC desc, ID3D11RenderTargetView** ppRTV);
bool CreateShaderResourceViewZ(ID3D11Texture2D* resource, D3D11_TEXTURE2D_DESC desc, ID3D11ShaderResourceView** ppSRV);
bool CreateDepthStencilViewZ(ID3D11Texture2D* resource, D3D11_TEXTURE2D_DESC desc, ID3D11DepthStencilView** ppDSV);

// @brief 2D画像(resource)リソースから 最適なビューを作成する
// @param resource 2D画像リソース
// @param 作成されたShaderResourceViewを受け取るための変数のアドレス
// @param 作成されたRenderTargetViewを受け取るための変数のアドレス
// @param ppDSV 作成されたDepthStencilViewを受け取るための変数のアドレス
static bool CreateViewsFromTexture2D(ID3D11Texture2D* resource, ID3D11ShaderResourceView** ppSRV, ID3D11RenderTargetView** ppRTV, ID3D11DepthStencilView** ppDSV)
{
	if (resource == nullptr)
		return false;

	// テクスチャ情報の取得
	D3D11_TEXTURE2D_DESC desc = {};
	resource->GetDesc(&desc);

	// ShaderResourceViewを作成する
	if (ppSRV && desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		if (CreateShaderResourceViewZ(resource, desc, ppSRV) == false)
			return false;
	}

	// RenderTargetViewを作成する
	if (ppRTV && desc.BindFlags & D3D11_BIND_RENDER_TARGET)
	{
		if (CreateRenderTargetViewZ(resource, desc, ppRTV) == false)
			return false;
	}

	// DepthStencilViewを作成する
	if (ppDSV && desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
	{
		if (CreateDepthStencilViewZ(resource, desc, ppDSV) == false)
			return false;
	}

	return true;
}

// @brief リソースから RenderTargetViewを作成する
bool CreateRenderTargetViewZ(ID3D11Texture2D* resource, D3D11_TEXTURE2D_DESC desc, ID3D11RenderTargetView** ppRTV)
{
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = desc.Format;

	// 単品のテクスチャ(通常テクスチャ)の場合
	if (desc.ArraySize == 1)
	{
		// 単品テクスチャ
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	}
	// テクスチャ配列の場合
	else
	{
		rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;	// テクスチャ配列
		rtvDesc.Texture2DArray.ArraySize = desc.ArraySize;	// 要素数
		rtvDesc.Texture2DArray.FirstArraySlice = 0;
		rtvDesc.Texture2DArray.MipSlice = 0;
	}

	// レンダーターゲットビュー作成
	HRESULT hr = D3D.GetDevice()->CreateRenderTargetView(resource, &rtvDesc, ppRTV);
	if (FAILED(hr)) {
		assert(0 && "エラー：RenderTargetViewの作成に失敗.");
		return false;
	}
	return true;
}

// @brief リソースから ShaderResourceViewを作成する
bool CreateShaderResourceViewZ(ID3D11Texture2D* resource, D3D11_TEXTURE2D_DESC desc, ID3D11ShaderResourceView** ppSRV)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	// テクスチャがZバッファの場合は、最適なフォーマットにする
	if (desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
	{
		switch (desc.Format)
		{
			// 16ビット
		case DXGI_FORMAT_R16_TYPELESS: srvDesc.Format = DXGI_FORMAT_R16_UNORM; break;
			// 32ビット
		case DXGI_FORMAT_R32_TYPELESS: srvDesc.Format = DXGI_FORMAT_R32_FLOAT; break;
			// 24ビット(Zバッファ) + 8ビット(ステンシルバッファ)
		case DXGI_FORMAT_R24G8_TYPELESS: srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; break;
			// 非対応
		default: assert(0 && "エラー：対応していないフォーマットです."); break;
		}
	}
	// Zバッファでない場合は、そのまま同じフォーマットを使用
	else
	{
		srvDesc.Format = desc.Format;
	}

	// 単品のテクスチャ(通常テクスチャ)の場合
	if (desc.ArraySize == 1)
	{
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;
		if (srvDesc.Texture2D.MipLevels <= 0)
			srvDesc.Texture2D.MipLevels = -1;
	}
	// テクスチャ配列の場合
	else
	{
		// さらにキューブマップの場合
		if (desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE)
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		}
		// 通常テクスチャ配列
		else
		{
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		}
		srvDesc.Texture2DArray.MostDetailedMip = 0;
		srvDesc.Texture2DArray.MipLevels = desc.MipLevels;
		srvDesc.Texture2DArray.ArraySize = desc.ArraySize; // 要素数
		srvDesc.Texture2DArray.FirstArraySlice = 0;
	}

	// 作成
	HRESULT hr = D3D.GetDevice()->CreateShaderResourceView(resource, &srvDesc, ppSRV);
	if (FAILED(hr)) {
		assert(0 && "エラー：ShaderResourceViewの作成に失敗.");
		return false;
	}
	return true;
}

// @brief リソースから DepthStencilViewを作成する
bool CreateDepthStencilViewZ(ID3D11Texture2D* resource, D3D11_TEXTURE2D_DESC desc, ID3D11DepthStencilView** ppDSV)
{
	// 作成するビューの設定
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};

	// テクスチャー作成時に指定したフォーマットと互換性があり、深度ステンシルビューとして指定できるフォーマットを指定する
	switch (desc.Format)
	{
		// 16ビット
	case DXGI_FORMAT_R16_TYPELESS: dsvDesc.Format = DXGI_FORMAT_D16_UNORM; break;
		// 32ビット
	case DXGI_FORMAT_R32_TYPELESS: dsvDesc.Format = DXGI_FORMAT_D32_FLOAT; break;
		// 24ビット(Zバッファ) + 8ビット(ステンシルバッファ)
	case DXGI_FORMAT_R24G8_TYPELESS: dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; break;
		// 非対応
	default: assert(0 && "エラー：対応していないフォーマットです."); break;
	}

	// 単品のテクスチャ(通常テクスチャ)の場合
	if (desc.ArraySize == 1)
	{
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Texture2D.MipSlice = 0;
	}
	// テクスチャ配列の場合
	else
	{
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		dsvDesc.Texture2DArray.ArraySize = desc.ArraySize;
		dsvDesc.Texture2DArray.MipSlice = 0;
		dsvDesc.Texture2DArray.FirstArraySlice = 0;
	}

	// 作成
	HRESULT hr = D3D.GetDevice()->CreateDepthStencilView(resource, &dsvDesc, ppDSV);
	if (FAILED(hr)) {
		assert(0 && "エラー：DepthStencilViewの作成に失敗.");
		return false;
	}
	return true;
}

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
		D3D.GetDevice(), D3D.GetDeviceContext(), wfilepath.c_str(),
		(ID3D11Resource**)m_cpBuffer.GetAddressOf(), m_cpSRV.GetAddressOf()
	);

	if (FAILED(hr)) {
		assert(0 && "エラー：テクスチャ読み込み失敗.");
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// DESC情報から作成
//-----------------------------------------------------------------------------
bool Texture::Create(D3D11_TEXTURE2D_DESC& desc)
{
	HRESULT hr = D3D.GetDevice()->CreateTexture2D(&desc, nullptr, m_cpBuffer.GetAddressOf());
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
	HRESULT hr = D3D.GetDevice()->CreateRenderTargetView(m_cpBuffer.Get(), &rtvDesc, &m_cpRTV);
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
	HRESULT hr = D3D.GetDevice()->CreateDepthStencilView(m_cpBuffer.Get(), &desc, m_cpDSV.GetAddressOf());
	if (FAILED(hr)) {
		assert(0 && "エラー：DepthStencilView作成失敗.");
		return false;
	}

	return true;
}

#include "RenderTarget.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
RenderTarget::RenderTarget()
{
}

//-----------------------------------------------------------------------------
// デストラクタ
//-----------------------------------------------------------------------------
RenderTarget::~RenderTarget()
{
}

//-----------------------------------------------------------------------------
// 作成
//-----------------------------------------------------------------------------
bool RenderTarget::Create(int w, int h, int mipLevel, int arraySize, DXGI_FORMAT colorF, DXGI_FORMAT dsF, float clearColor[4])
{
	auto& d3dDevice = g_graphicsDevice->g_cpDevice;
	m_width = w;
	m_height = h;

	//レンダリングターゲットとなるテクスチャを作成する。
	if (!CreateRenderTargetTexture(w, h, mipLevel, arraySize, colorF, clearColor)) {
		MessageBoxA(nullptr, "レンダリングターゲットとなるテクスチャの作成に失敗しました。", "エラー", MB_OK);
		return false;
	}

	//深度ステンシルバッファとなるテクスチャを作成する。
	if (dsF != DXGI_FORMAT_UNKNOWN) {
		if (!CreateDepthStencilTexture(w, h, dsF)) {
			MessageBoxA(nullptr, "レンダリングターゲットとなるテクスチャの作成に失敗しました。", "エラー", MB_OK);
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
bool RenderTarget::CreateRenderTargetTexture(int w, int h, int mipLevel, int arraySize, DXGI_FORMAT format, float clearColor[4])
{
	return false;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
bool RenderTarget::CreateDepthStencilTexture(int w, int h, DXGI_FORMAT format)
{
	D3D11_TEXTURE2D_DESC desc = {};

	// 種類
	desc.Format = format;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;

	desc.Height = w;
	desc.Width = h;

	// アクセスの種類
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;

	desc.MipLevels = 1;
	desc.ArraySize = 1;

	// その他の設定
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;//useMSAA ? 8 : 1;
	desc.SampleDesc.Quality = 0;

	return true;
}

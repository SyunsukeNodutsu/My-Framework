#include "SpriteShader.h"

//コンストラクタ ==============================================================
SpriteShader::SpriteShader()
	: m_cb4Sprite()
	, m_prevProjMat(mfloat4x4::Identity)
	, m_isBegin(false)
{
}

//初期化 ======================================================================
bool SpriteShader::Initialize()
{
	HRESULT hr = S_FALSE;

	//頂点シェーダ
	{
		//コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "SpriteShader_VS.shaderinc"

		//頂点シェーダー作成
		hr = g_graphicsDevice->g_cpDevice.Get()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_cpVS.GetAddressOf());
		if (FAILED(hr)) {
			assert(0 && "頂点シェーダー作成失敗");
			return false;
		}

		//1頂点の詳細な情報
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		//頂点インプットレイアウト作成
		hr = g_graphicsDevice->g_cpDevice.Get()->CreateInputLayout(&layout[0], (UINT)layout.size(), compiledBuffer, sizeof(compiledBuffer), m_cpInputLayout.GetAddressOf());
		if (FAILED(hr)) {
			assert(0 && "CreateInputLayout失敗");
			return false;
		}
	}

	//ピクセルシェーダ
	{
		//コンパイル済みのシェーダーヘッダーファイルをインクルード
		#include "SpriteShader_PS.shaderinc"

		hr = g_graphicsDevice->g_cpDevice.Get()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_cpPS.GetAddressOf());
		if (FAILED(hr)) {
			assert(0 && "ピクセルシェーダー作成失敗");
			return false;
		}
	}

	//定数バッファ作成
	if (!m_cb4Sprite.Create()) {
		assert(0 && "エラー：コンスタントバッファ作成失敗.");
		return false;
	}

	m_cb4Sprite.SetToDevice(4);

	return true;
}

//開始 ========================================================================
void SpriteShader::Begin(ID3D11DeviceContext* pd3dContext, bool linear, bool disableZBuffer)
{
	if (!pd3dContext) return;

	if (m_isBegin) return;
	m_isBegin = true;

	//2D用正射影行列作成

	//射影行列を保存しておく
	m_prevProjMat = RENDERER.Getcb9().Get().m_proj_matrix;

	//正射影行列を設定
	UINT pNumVierports = 1;
	D3D11_VIEWPORT vp;
	pd3dContext->RSGetViewports(&pNumVierports, &vp);

	RENDERER.Getcb9().Work().m_proj_matrix = DirectX::XMMatrixOrthographicLH(vp.Width, vp.Height, 0, 1);
	RENDERER.Getcb9().Write();

	//使用するステートをセット
	
	//Z判定、Z書き込み無効のステートをセット
	if (disableZBuffer)
		RENDERER.SetDepthStencil(false, false);

	//Samplerステートをセット
	RENDERER.SetSampler(
		linear ? SS_FilterMode::eLinear : SS_FilterMode::ePoint,
		SS_AddressMode::eWrap
	);

	//Rasterizerステートをセット
	RENDERER.SetRasterize(RS_CullMode::eCullNone, RS_FillMode::eSolid);

	//各シェーダ設定
	pd3dContext->VSSetShader(m_cpVS.Get(), 0, 0);
	pd3dContext->IASetInputLayout(m_cpInputLayout.Get());

	pd3dContext->PSSetShader(m_cpPS.Get(), 0, 0);
}

//終了 ========================================================================
void SpriteShader::End(ID3D11DeviceContext* pd3dContext)
{
	if (!m_isBegin) return;
	m_isBegin = false;

	RENDERER.Getcb9().Work().m_proj_matrix = m_prevProjMat;
	RENDERER.Getcb9().Write();

	RENDERER.SetDepthStencil(true, true);
	RENDERER.SetSampler(SS_FilterMode::eAniso, SS_AddressMode::eWrap);
	RENDERER.SetRasterize(RS_CullMode::eBack, RS_FillMode::eSolid);
}

//2D画像描画 ==================================================================
void SpriteShader::DrawTexture(const Texture* texture, float2 position, const cfloat4x4 color)
{
	if (!g_graphicsDevice) return;
	if (!g_graphicsDevice->g_cpContext) return;

	if (!m_isBegin) return;
	if (texture == nullptr) return;

	RENDERER.Getcb8().Work().m_world_matrix = mfloat4x4::Identity;
	RENDERER.Getcb8().Write();

	//テクスチャセット
	g_graphicsDevice->g_cpContext.Get()->PSSetShaderResources(0, 1, texture->SRVAddress());

	//色
	if (color) m_cb4Sprite.Work().m_color = color;
	m_cb4Sprite.Write();

	D3D11_TEXTURE2D_DESC desc = {};
	texture->GetResource()->GetDesc(&desc);
	float width = static_cast<float>(desc.Width);
	float height = static_cast<float>(desc.Height);

	//頂点作成
	float x_01 = position.x;
	float y_01 = position.y;
	float x_02 = position.x + width;
	float y_02 = position.y + height;

	//基準点(Pivot)ぶんずらす
	float2 pivot = float2(0.5f);
	x_01 -= pivot.x * width;
	x_02 -= pivot.x * width;
	y_01 -= pivot.y * height;
	y_02 -= pivot.y * height;

	//左上 -> 右上 -> 左下 -> 右下
	Vertex vertex[] = {
		{ float3(x_01, y_01, 0), float2(0, 1) },
		{ float3(x_01, y_02, 0), float2(0, 0) },
		{ float3(x_02, y_01, 0), float2(1, 1) },
		{ float3(x_02, y_02, 0), float2(1, 0) },
	};

	//描画
	g_graphicsDevice->DrawVertices(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, vertex, sizeof(Vertex));

	//セットしたテクスチャを解除しておく
	ID3D11ShaderResourceView* null_srv = nullptr;
	g_graphicsDevice->g_cpContext.Get()->PSSetShaderResources(0, 1, &null_srv);
}
void SpriteShader::DrawTexture(const Texture* texture, ID3D11DeviceContext* pd3dContext, mfloat4x4 worldMatrix, const cfloat4x4 color)
{
	if (!g_graphicsDevice) return;
	if (!g_graphicsDevice->g_cpContext) return;

	if (!m_isBegin) return;
	if (texture == nullptr) return;

	float2 position = float2::Zero;

	RENDERER.Getcb8().Work().m_world_matrix = worldMatrix;
	RENDERER.Getcb8().Write();

	//テクスチャセット
	pd3dContext->PSSetShaderResources(0, 1, texture->SRVAddress());

	//色
	if (color) m_cb4Sprite.Work().m_color = color;
	m_cb4Sprite.Write();

	D3D11_TEXTURE2D_DESC desc = {};
	texture->GetResource()->GetDesc(&desc);
	float width = static_cast<float>(desc.Width);
	float height = static_cast<float>(desc.Height);

	//頂点作成
	float x_01 = position.x;
	float y_01 = position.y;
	float x_02 = position.x + width;
	float y_02 = position.y + height;

	//基準点(Pivot)ぶんずらす
	float2 pivot = float2(0.5f);
	x_01 -= pivot.x * width;
	x_02 -= pivot.x * width;
	y_01 -= pivot.y * height;
	y_02 -= pivot.y * height;

	//左上 -> 右上 -> 左下 -> 右下
	Vertex vertex[] = {
		{ float3(x_01, y_01, 0), float2(0, 1) },
		{ float3(x_01, y_02, 0), float2(0, 0) },
		{ float3(x_02, y_01, 0), float2(1, 1) },
		{ float3(x_02, y_02, 0), float2(1, 0) },
	};

	//描画
	g_graphicsDevice->DrawVertices(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, vertex, sizeof(Vertex));

	//セットしたテクスチャを解除しておく
	ID3D11ShaderResourceView* null_srv = nullptr;
	pd3dContext->PSSetShaderResources(0, 1, &null_srv);
}

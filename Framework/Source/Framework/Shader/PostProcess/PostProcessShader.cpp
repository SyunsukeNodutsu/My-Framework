#include "PostProcessShader.h"

//-----------------------------------------------------------------------------
//コンストラクタ
//-----------------------------------------------------------------------------
PostProcessShader::PostProcessShader()
	: m_colorPS(nullptr)
	, m_blurPS(nullptr)
	, m_HBrightPS(nullptr)
	, m_cb0Blur()
{
}

//-----------------------------------------------------------------------------
//デストラクタ
//-----------------------------------------------------------------------------
PostProcessShader::~PostProcessShader()
{
}

//-----------------------------------------------------------------------------
//初期化
//-----------------------------------------------------------------------------
bool PostProcessShader::Initialize()
{
	HRESULT hr = S_FALSE;

	//頂点シェーダ/入力レイアウト
	{
		#include "PostProcessShader_VS.shaderinc"

		hr = g_graphicsDevice->g_cpDevice.Get()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_cpVS.GetAddressOf());
		if (FAILED(hr)) {
			assert(0 && "頂点シェーダー作成失敗.");
			return false;
		}

		std::vector<D3D11_INPUT_ELEMENT_DESC> layout = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		hr = g_graphicsDevice->g_cpDevice.Get()->CreateInputLayout(&layout[0], (UINT)layout.size(), compiledBuffer, sizeof(compiledBuffer), m_cpInputLayout.GetAddressOf());
		if (FAILED(hr)) {
			assert(0 && "CreateInputLayout失敗.");
			return false;
		}
	}

	//ピクセルシェーダ/Color_PS
	{
		#include "PostProcess_Color_PS.shaderinc"

		hr = g_graphicsDevice->g_cpDevice.Get()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_colorPS.GetAddressOf());
		if (FAILED(hr)) {
			assert(0 && "ピクセルシェーダー作成失敗.");
			return false;
		}
	}

	//ピクセルシェーダ/Blur_PS
	{
		#include "PostProcess_Blur_PS.shaderinc"

		hr = g_graphicsDevice->g_cpDevice.Get()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_blurPS.GetAddressOf());
		if (FAILED(hr)) {
			assert(0 && "ピクセルシェーダー作成失敗.");
			return false;
		}
	}

	//ピクセルシェーダ/HBright_PS
	{
		#include "PostProcess_HBright_PS.shaderinc"

		hr = g_graphicsDevice->g_cpDevice.Get()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_HBrightPS.GetAddressOf());
		if (FAILED(hr)) {
			assert(0 && "ピクセルシェーダー作成失敗.");
			return false;
		}
	}

	//定数バッファ作成
	if (!m_cb0Blur.Create()) {
		assert(0 && "定数バッファ作成失敗.");
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
//加工段階描画
//-----------------------------------------------------------------------------
void PostProcessShader::DrawColor(Texture* texture)
{
	if (texture == nullptr) return;

	g_graphicsDevice->g_cpContext.Get()->IASetInputLayout(m_cpInputLayout.Get());

	g_graphicsDevice->g_cpContext.Get()->VSSetShader(m_cpVS.Get(), 0, 0);
	g_graphicsDevice->g_cpContext.Get()->PSSetShader(m_colorPS.Get(), 0, 0);

	g_graphicsDevice->g_cpContext.Get()->PSSetShaderResources(0, 1, texture->SRVAddress());

	//Z判定、Z書き込み無効のステートをセット
	RENDERER.SetDepthStencil(false, false);
	RENDERER.SetSampler(SS_FilterMode::eLinear, SS_AddressMode::eWrap);
	RENDERER.SetRasterize(RS_CullMode::eCullNone, RS_FillMode::eSolid);

	g_graphicsDevice->DrawVertices(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, &vOrthographic[0], sizeof(Vertex));

	RENDERER.SetDepthStencil(true, true);
	RENDERER.SetSampler(SS_FilterMode::eAniso, SS_AddressMode::eWrap);
	RENDERER.SetRasterize(RS_CullMode::eBack, RS_FillMode::eSolid);
}

//-----------------------------------------------------------------------------
//ブラー描画
//-----------------------------------------------------------------------------
void PostProcessShader::BlurDraw(Texture* texture, float2 dir)
{
	//ステート記憶
	ComPtr<ID3D11DepthStencilState> saveDS; UINT saveStencilRef = 0;
	g_graphicsDevice->g_cpContext.Get()->OMGetDepthStencilState(&saveDS, &saveStencilRef);

	RENDERER.SetDepthStencil(false, false);
	RENDERER.SetSampler(SS_FilterMode::eLinear, SS_AddressMode::eWrap);

	//サンプリング
	{
		//テクスチャのサイズから1ピクセルのサイズを求める(テクセルサイズ)
		float2 ts;
		ts.x = 1.0f / texture->GetDesc().Width;
		ts.y = 1.0f / texture->GetDesc().Height;

		//ずれ分の座標を計算
		constexpr int MAX_SAMPLING = 31;
		constexpr float DISPERSION = 3;
		float totalWeight = 0;
		for (int i = 0; i < MAX_SAMPLING; i++)
		{
			int pt = i - MAX_SAMPLING / 2;
			m_cb0Blur.Work().offset[i].x = dir.x * (pt * ts.x);
			m_cb0Blur.Work().offset[i].y = dir.y * (pt * ts.y);

			//ガウス係数の計算
			float w = exp(-(pt * pt) / (2 * DISPERSION * DISPERSION));
			m_cb0Blur.Work().offset[i].z = w;

			//合計を計算
			totalWeight += w;
		}

		//係数を正規化
		for (int i = 0; i < 31; i++) {
			m_cb0Blur.Work().offset[i].z /= totalWeight;
		}

		m_cb0Blur.Write();
		m_cb0Blur.SetToDevice(0);
	}

	g_graphicsDevice->g_cpContext.Get()->PSSetShaderResources(0, 1, texture->SRVAddress());

	g_graphicsDevice->g_cpContext.Get()->IASetInputLayout(m_cpInputLayout.Get());

	g_graphicsDevice->g_cpContext.Get()->VSSetShader(m_cpVS.Get(), 0, 0);
	g_graphicsDevice->g_cpContext.Get()->PSSetShader(m_blurPS.Get(), 0, 0);

	g_graphicsDevice->DrawVertices(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, &vOrthographic[0], sizeof(Vertex));

	//NULLリソースの設定
	ID3D11ShaderResourceView* nullSRV = nullptr;
	g_graphicsDevice->g_cpContext.Get()->PSSetShaderResources(0, 1, &nullSRV);

	//ステート復元
	g_graphicsDevice->g_cpContext.Get()->OMSetDepthStencilState(saveDS.Get(), saveStencilRef);
	RENDERER.SetSampler(SS_FilterMode::eAniso, SS_AddressMode::eWrap);
}

//-----------------------------------------------------------------------------
//高輝度抽出ぼかし描画
//-----------------------------------------------------------------------------
void PostProcessShader::GenerateBlur(BlurTexture& blurTex, Texture* srcTex)
{
	RestoreRenderTarget rrt = {};

	//ビューポート記憶
	D3D11_VIEWPORT saveVP = {};
	UINT numVP = 1;
	g_graphicsDevice->g_cpContext.Get()->RSGetViewports(&numVP, &saveVP);

	//サンプリングを繰り返す/輝度の抽出
	for (int i = 0; i < 5; i++)
	{
		g_graphicsDevice->g_cpContext.Get()->OMSetRenderTargets(1, blurTex.m_rt[i][0]->RTVAddress(), nullptr);
		g_graphicsDevice->g_cpContext.Get()->RSSetViewports(1, &g_graphicsDevice->g_viewport);

		if (i == 0) {
			DrawColor(srcTex);
		}
		else {
			DrawColor(blurTex.m_rt[i - 1][0].get());
		}

		g_graphicsDevice->g_cpContext.Get()->OMSetRenderTargets(1, blurTex.m_rt[i][1]->RTVAddress(), nullptr);

		BlurDraw(blurTex.m_rt[i][0].get(), float2(1, 0));

		g_graphicsDevice->g_cpContext.Get()->OMSetRenderTargets(1, blurTex.m_rt[i][0]->RTVAddress(), nullptr);

		BlurDraw(blurTex.m_rt[i][1].get(), float2(0, 1));
	}

	//記憶したビューポートをもとに復元
	g_graphicsDevice->g_cpContext.Get()->RSSetViewports(1, &saveVP);
}

//-----------------------------------------------------------------------------
//フィルタリング
//-----------------------------------------------------------------------------
void PostProcessShader::BrightFiltering(Texture* destRT, Texture* srcTex)
{
	RestoreRenderTarget rrt = {};

	D3D11_VIEWPORT saveVP = {};
	UINT numVP = 1;
	g_graphicsDevice->g_cpContext.Get()->RSGetViewports(&numVP, &saveVP);

	g_graphicsDevice->g_cpContext.Get()->OMSetRenderTargets(1, destRT->RTVAddress(), nullptr);

	g_graphicsDevice->g_cpContext.Get()->RSSetViewports(1, &g_graphicsDevice->g_viewport);

	ID3D11DepthStencilState* saveDS = {};
	UINT saveStencilRef = 0;
	g_graphicsDevice->g_cpContext.Get()->OMGetDepthStencilState(&saveDS, &saveStencilRef);

	RENDERER.SetDepthStencil(false, false);

	g_graphicsDevice->g_cpContext.Get()->PSSetShaderResources(0, 1, srcTex->SRVAddress());
	RENDERER.SetSampler(SS_FilterMode::eLinear, SS_AddressMode::eWrap);

	g_graphicsDevice->g_cpContext.Get()->VSSetShader(m_cpVS.Get(), 0, 0);
	g_graphicsDevice->g_cpContext.Get()->IASetInputLayout(m_cpInputLayout.Get());
	g_graphicsDevice->g_cpContext.Get()->PSSetShader(m_HBrightPS.Get(), 0, 0);

	g_graphicsDevice->DrawVertices(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP, 4, &vOrthographic[0], sizeof(Vertex));

	ID3D11ShaderResourceView* nullSRV = nullptr;
	g_graphicsDevice->g_cpContext.Get()->PSSetShaderResources(0, 1, &nullSRV);

	RENDERER.SetSampler(SS_FilterMode::eAniso, SS_AddressMode::eWrap);
	g_graphicsDevice->g_cpContext.Get()->OMSetDepthStencilState(saveDS, saveStencilRef);
	saveDS->Release();

	g_graphicsDevice->g_cpContext.Get()->RSSetViewports(1, &saveVP);
}

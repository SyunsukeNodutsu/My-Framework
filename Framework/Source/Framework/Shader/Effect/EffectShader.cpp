#include "EffectShader.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
EffectShader::EffectShader()
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
bool EffectShader::Initialize()
{
	if (!g_graphicsDevice) return false;
	if (!g_graphicsDevice->g_cpContext) return false;

	HRESULT hr = S_FALSE;

	//--------------------------------------------------
	// 頂点シェーダ
	//--------------------------------------------------
	{
		#include "EffectShader_VS.shaderinc"

		hr = g_graphicsDevice->g_cpDevice.Get()->CreateVertexShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_cpVS.GetAddressOf());
		if (FAILED(hr)) {
			assert(0 && "エラー：頂点シェーダ作成失敗.");
			return false;
		}

		// 1頂点の詳細な情報
		std::vector<D3D11_INPUT_ELEMENT_DESC> layout =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",	  0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		hr = g_graphicsDevice->g_cpDevice.Get()->CreateInputLayout(&layout[0], (UINT)layout.size(), compiledBuffer, sizeof(compiledBuffer), m_cpInputLayout.GetAddressOf());
		if (FAILED(hr)) {
			assert(0 && "エラー：頂点入力レイアウト作成失敗.");
			return false;
		}
	}

	//--------------------------------------------------
	// ピクセルシェーダ
	//--------------------------------------------------
	{
		#include "EffectShader_PS.shaderinc"

		hr = g_graphicsDevice->g_cpDevice.Get()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_cpPS.GetAddressOf());
		if (FAILED(hr)) {
			assert(0 && "エラー：ピクセルシェーダ作成失敗.");
			return false;
		}
	}

	return true;
}

//-----------------------------------------------------------------------------
// 開始
//-----------------------------------------------------------------------------
void EffectShader::Begin()
{
	if (!g_graphicsDevice) return;
	if (!g_graphicsDevice->g_cpContext) return;

	g_graphicsDevice->g_cpContext.Get()->VSSetShader(m_cpVS.Get(), 0, 0);
	g_graphicsDevice->g_cpContext.Get()->IASetInputLayout(m_cpInputLayout.Get());

	g_graphicsDevice->g_cpContext.Get()->PSSetShader(m_cpPS.Get(), 0, 0);
}

//-----------------------------------------------------------------------------
// 3D線描画
//-----------------------------------------------------------------------------
void EffectShader::DrawLine(const float3& p1, const float3& p2, const cfloat4x4& color)
{
	if (!g_graphicsDevice) return;

	Vertex vertex[2] = {
		{ p1, float2(0, 0), color},
		{ p2, float2(1, 0), color},
	};

	// 頂点を描画
	g_graphicsDevice->DrawVertices(D3D_PRIMITIVE_TOPOLOGY_LINESTRIP, 2, &vertex[0], sizeof(Vertex));
}

#include "GPUParticleShader.h"

//-----------------------------------------------------------------------------
//コンストラクタ
//-----------------------------------------------------------------------------
GPUParticleShader::GPUParticleShader()
	: m_cpCS(nullptr)
	, m_spVertexBuffer(nullptr)
{
}

//-----------------------------------------------------------------------------
//初期化
//-----------------------------------------------------------------------------
bool GPUParticleShader::Initialize()
{
	HRESULT hr = S_FALSE;

	//頂点シェーダ/入力レイアウト
	{
		#include "GPUParticleShader_VS.shaderinc"

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

	//ピクセルシェーダ
	{
		#include "GPUParticleShader_PS.shaderinc"

		hr = g_graphicsDevice->g_cpDevice.Get()->CreatePixelShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_cpPS.GetAddressOf());
		if (FAILED(hr)) {
			assert(0 && "ピクセルシェーダー作成失敗.");
			return false;
		}
	}

	//計算シェーダ
	{
		#include "GPUParticleShader_CS.shaderinc"

		hr = g_graphicsDevice->g_cpDevice.Get()->CreateComputeShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_cpCS.GetAddressOf());
		if (FAILED(hr)) {
			assert(0 && "計算シェーダー作成失敗.");
			return false;
		}
	}

	//頂点定義
	static constexpr float rectSize = 0.2f;
	Vertex vertices[]{
		{ float3(-rectSize, -rectSize,  0.0f), float2(0, 1) },
		{ float3(-rectSize,  rectSize,  0.0f), float2(0, 0) },
		{ float3( rectSize, -rectSize,  0.0f), float2(1, 1) },
		{ float3( rectSize,  rectSize,  0.0f), float2(1, 0) },
	};

	//頂点バッファーの作成
	m_spVertexBuffer = std::make_shared<Buffer>();
	m_spVertexBuffer->Create(D3D11_BIND_VERTEX_BUFFER, sizeof(Vertex) * 4, D3D11_USAGE_DYNAMIC, nullptr);
	m_spVertexBuffer->WriteData(&vertices[0], sizeof(Vertex) * 4);

	return true;
}

//-----------------------------------------------------------------------------
//シェーダーセットアップ
//-----------------------------------------------------------------------------
void GPUParticleShader::Begin()
{
	constexpr static UINT strides = sizeof(Vertex);
	constexpr static UINT offsets = 0;
	g_graphicsDevice->g_cpContext.Get()->IASetVertexBuffers(0, 1, m_spVertexBuffer->GetAddress(), &strides, &offsets);

	g_graphicsDevice->g_cpContext.Get()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	g_graphicsDevice->g_cpContext.Get()->IASetInputLayout(m_cpInputLayout.Get());

	g_graphicsDevice->g_cpContext.Get()->VSSetShader(m_cpVS.Get(), 0, 0);
	g_graphicsDevice->g_cpContext.Get()->PSSetShader(m_cpPS.Get(), 0, 0);
}

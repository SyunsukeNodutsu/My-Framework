#include "GPUParticleShader.h"

//-----------------------------------------------------------------------------
//コンストラクタ
//-----------------------------------------------------------------------------
GPUParticleShader::GPUParticleShader()
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
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR"	, 0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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

	//定数バッファ
	if (!m_cb7Particle.Create()) {
		assert(0 && "定数バッファ作成失敗.");
		return false;
	}
	m_cb7Particle.SetToDevice(RENDERER.use_slot_particle);

	//

	Vertex g_VertexList[]{
		{ { -0.5f,  0.5f, 0.0f }, cfloat4x4::Red },
		{ {  0.5f, -0.5f, 0.0f }, cfloat4x4::Red },
		{ { -0.5f, -0.5f, 0.0f }, cfloat4x4::Red }
	};

	const int size = 256;
	m_vertexBuffer = std::make_shared<Buffer>();
	m_vertexBuffer->Create(D3D11_BIND_VERTEX_BUFFER, size, D3D11_USAGE_DYNAMIC, nullptr);

	m_vertexBuffer->WriteData(g_graphicsDevice->g_cpContext.Get(), &g_VertexList[0], sizeof(Vertex) * 4);

	return true;
}

//-----------------------------------------------------------------------------
//更新
//-----------------------------------------------------------------------------
void GPUParticleShader::Update()
{

}

//-----------------------------------------------------------------------------
//描画
//-----------------------------------------------------------------------------
void GPUParticleShader::Draw()
{
	//バッファセット
	UINT strides = sizeof(Vertex); UINT offsets = 0;
	g_graphicsDevice->g_cpContext.Get()->IASetVertexBuffers(0, 1, m_vertexBuffer->GetAddress(), &strides, &offsets);

	g_graphicsDevice->g_cpContext.Get()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//シェーダー設定
	g_graphicsDevice->g_cpContext.Get()->IASetInputLayout(m_cpInputLayout.Get());

	g_graphicsDevice->g_cpContext.Get()->VSSetShader(m_cpVS.Get(), 0, 0);
	g_graphicsDevice->g_cpContext.Get()->PSSetShader(m_cpPS.Get(), 0, 0);

	//描画
	RENDERER.SetRasterize(RS_CullMode::eCullNone, RS_FillMode::eSolid);
	g_graphicsDevice->g_cpContext.Get()->Draw(3, 0);
	RENDERER.SetRasterize(RS_CullMode::eBack, RS_FillMode::eSolid);
}

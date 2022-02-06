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

	//ピクセルシェーダ
	{
		#include "GPUParticleShader_CS.shaderinc"

		hr = g_graphicsDevice->g_cpDevice.Get()->CreateComputeShader(compiledBuffer, sizeof(compiledBuffer), nullptr, m_cpCS.GetAddressOf());
		if (FAILED(hr)) {
			assert(0 && "計算シェーダー作成失敗.");
			return false;
		}
	}

	//定数バッファ
	if (!m_cb7Particle.Create()) {
		assert(0 && "定数バッファ作成失敗.");
		return false;
	}
	m_cb7Particle.SetToDevice(RENDERER.use_slot_particle);

	//頂点定義
	float3 center = float3(0, 4, 0);
	Vertex vertices[]{
		{ center + float3( 0.5f, -0.5f,  0.5f), cfloat4x4::Red },
		{ center + float3(-0.5f, -0.5f,  0.5f), cfloat4x4::Red },
		{ center + float3( 0.5f, -0.5f, -0.5f), cfloat4x4::Red },
		{ center + float3(-0.5f, -0.5f, -0.5f), cfloat4x4::Red },
	};

	const int size = sizeof(Vertex) * 4;
	m_vertexBuffer = std::make_shared<Buffer>();
	m_vertexBuffer->Create(D3D11_BIND_VERTEX_BUFFER, size, D3D11_USAGE_DYNAMIC, nullptr);

	m_vertexBuffer->WriteData(g_graphicsDevice->g_cpContext.Get(), &vertices[0], size);

	//particle構造体
	for (int i = 0; i < PARTICLE_COUNT; i++)
	{

	}

	return true;
}

//-----------------------------------------------------------------------------
//更新
//-----------------------------------------------------------------------------
void GPUParticleShader::Update()
{
	// メルセンヌ・ツイスター法による擬似乱数生成器を、
	// ハードウェア乱数をシードにして初期化
	std::random_device seed_gen;
	std::mt19937 engine(seed_gen());

	// 一様実数分布
	// [-5.0, 5.0)の値の範囲で、等確率に実数を生成する
	std::uniform_real_distribution<> dist(-5.0, 5.0);

	float x = static_cast<float>(dist(engine));
	float y = static_cast<float>(dist(engine));
	float z = static_cast<float>(dist(engine));

	m_cb7Particle.Work().position = float3(x, y, z);
	m_cb7Particle.Write();
}

//-----------------------------------------------------------------------------
//描画
//-----------------------------------------------------------------------------
void GPUParticleShader::Draw()
{
	//バッファセット
	UINT strides = sizeof(Vertex); UINT offsets = 0;
	g_graphicsDevice->g_cpContext.Get()->IASetVertexBuffers(0, 1, m_vertexBuffer->GetAddress(), &strides, &offsets);

	g_graphicsDevice->g_cpContext.Get()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//シェーダー設定
	g_graphicsDevice->g_cpContext.Get()->IASetInputLayout(m_cpInputLayout.Get());

	g_graphicsDevice->g_cpContext.Get()->VSSetShader(m_cpVS.Get(), 0, 0);
	g_graphicsDevice->g_cpContext.Get()->PSSetShader(m_cpPS.Get(), 0, 0);

	//描画
	RENDERER.SetRasterize(RS_CullMode::eCullNone, RS_FillMode::eSolid);
	g_graphicsDevice->g_cpContext.Get()->Draw(4, 0);
	RENDERER.SetRasterize(RS_CullMode::eBack, RS_FillMode::eSolid);
}

#include "GPUParticleShader.h"

//-----------------------------------------------------------------------------
//コンストラクタ
//-----------------------------------------------------------------------------
GPUParticleShader::GPUParticleShader()
	: m_cpCS(nullptr)
	, m_cb7Particle()
	, m_spVertexBuffer(nullptr)
	, m_spTexture(nullptr)
	, m_billboard(true)
	, m_cullNone(true)
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

	//計算シェーダ
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
	Vertex vertices[]{
		{ float3(-0.5f, -0.5f,  0.0f), cfloat4x4::Red },
		{ float3(-0.5f,  0.5f,  0.0f), cfloat4x4::Blue },
		{ float3( 0.5f, -0.5f,  0.0f), cfloat4x4::Green },
		{ float3( 0.5f,  0.5f,  0.0f), cfloat4x4::White },
	};

	//頂点バッファーの作成
	const int size = sizeof(Vertex) * 4;
	m_spVertexBuffer = std::make_shared<Buffer>();
	m_spVertexBuffer->Create(D3D11_BIND_VERTEX_BUFFER, size, D3D11_USAGE_DYNAMIC, nullptr);

	m_spVertexBuffer->WriteData(g_graphicsDevice->g_cpContext.Get(), &vertices[0], size);

	//テクスチャ
	m_spTexture = std::make_shared<Texture>();

	return true;
}

//-----------------------------------------------------------------------------
//更新
//-----------------------------------------------------------------------------
void GPUParticleShader::Update()
{
	//擬似乱数生成器の初期化
	std::random_device seed_gen;
	std::mt19937 engine(seed_gen());

	//一様実数分布
	std::uniform_real_distribution<> dist(-0.2, 0.2);

	//座標をランダムにしてみる
	float x = static_cast<float>(dist(engine));
	float y = static_cast<float>(dist(engine));
	float z = static_cast<float>(dist(engine));

	float3 center = float3(6, 4, 6);
	mfloat4x4 trans = mfloat4x4::CreateTranslation(center + float3(x, y, z));

	//ビルボード
	if (m_billboard)
	{
		mfloat4x4 view = RENDERER.Getcb9().Get().m_view_matrix;
		mfloat4x4 invView = view.Invert();
		invView._41 = 0.0f;
		invView._42 = 0.0f;
		invView._43 = 0.0f;

		RENDERER.Getcb8().Work().m_world_matrix = invView * trans;
	}
	else
	{
		RENDERER.Getcb8().Work().m_world_matrix = trans;
	}

	RENDERER.Getcb8().Write();
}

//-----------------------------------------------------------------------------
//描画
//-----------------------------------------------------------------------------
void GPUParticleShader::Draw()
{
	//バッファセット
	constexpr static UINT strides = sizeof(Vertex);
	constexpr static UINT offsets = 0;
	g_graphicsDevice->g_cpContext.Get()->IASetVertexBuffers(0, 1, m_spVertexBuffer->GetAddress(), &strides, &offsets);

	g_graphicsDevice->g_cpContext.Get()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//シェーダー設定
	g_graphicsDevice->g_cpContext.Get()->IASetInputLayout(m_cpInputLayout.Get());

	g_graphicsDevice->g_cpContext.Get()->VSSetShader(m_cpVS.Get(), 0, 0);
	g_graphicsDevice->g_cpContext.Get()->PSSetShader(m_cpPS.Get(), 0, 0);

	//描画
	if (m_cullNone) RENDERER.SetRasterize(RS_CullMode::eCullNone, RS_FillMode::eSolid);
	g_graphicsDevice->g_cpContext.Get()->Draw(4, 0);
	if (m_cullNone) RENDERER.SetRasterize(RS_CullMode::eBack, RS_FillMode::eSolid);
}

//-----------------------------------------------------------------------------
//SRV関連付けのバッファーを作成
//-----------------------------------------------------------------------------
HRESULT GPUParticleShader::CreateBufferSRV(ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut)
{
	D3D11_BUFFER_DESC descBuf = {};
	pBuffer->GetDesc(&descBuf);

	D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	desc.BufferEx.FirstElement = 0;

	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED) {
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
	}
	return g_graphicsDevice->g_cpDevice->CreateShaderResourceView(pBuffer, &desc, ppSRVOut);
}

//-----------------------------------------------------------------------------
//UAV関連付けのバッファーを作成
//-----------------------------------------------------------------------------
HRESULT GPUParticleShader::CreateBufferUAV(ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAVOut)
{
	D3D11_BUFFER_DESC descBuf = {};
	pBuffer->GetDesc(&descBuf);

	D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
	desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	desc.Buffer.FirstElement = 0;

	if (descBuf.MiscFlags & D3D11_RESOURCE_MISC_BUFFER_STRUCTURED) {
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;
	}
	return g_graphicsDevice->g_cpDevice->CreateUnorderedAccessView(pBuffer, &desc, ppUAVOut);
}

//-----------------------------------------------------------------------------
//書き込み専用バッファーをD3D11_CPU_ACCESS_READに変更しコピー
//-----------------------------------------------------------------------------
ID3D11Buffer* GPUParticleShader::CreateAndCopyToDebugBuf(ID3D11Buffer* pBuffer)
{
	ID3D11Buffer* debugbuf = nullptr;

	D3D11_BUFFER_DESC desc = {};
	pBuffer->GetDesc(&desc);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	if (SUCCEEDED(g_graphicsDevice->g_cpDevice->CreateBuffer(&desc, nullptr, &debugbuf)))
		g_graphicsDevice->g_cpContext->CopyResource(debugbuf, pBuffer);
	return debugbuf;
}

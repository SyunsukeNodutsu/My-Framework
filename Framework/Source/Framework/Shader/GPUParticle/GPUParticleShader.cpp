#include "GPUParticleShader.h"

//-----------------------------------------------------------------------------
//コンストラクタ
//-----------------------------------------------------------------------------
GPUParticleShader::GPUParticleShader()
	: mParticleAmount(25600)
	, m_cpCS(nullptr)
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
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR"	, 0, DXGI_FORMAT_R8G8B8A8_UNORM,	0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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
	Vertex vertices[]{
		{ float3(-0.5f, -0.5f,  0.0f), cfloat4x4::White },
		{ float3(-0.5f,  0.5f,  0.0f), cfloat4x4::White },
		{ float3( 0.5f, -0.5f,  0.0f), cfloat4x4::White },
		{ float3( 0.5f,  0.5f,  0.0f), cfloat4x4::White },
	};

	//頂点バッファーの作成
	const int size = sizeof(Vertex) * 4;
	m_spVertexBuffer = std::make_shared<Buffer>();
	m_spVertexBuffer->Create(D3D11_BIND_VERTEX_BUFFER, size, D3D11_USAGE_DYNAMIC, nullptr);

	m_spVertexBuffer->WriteData(g_graphicsDevice->g_cpContext.Get(), &vertices[0], size);

	//テクスチャ
	m_spTexture = std::make_shared<Texture>();
	m_spTexture->Create("Resource/Texture/EnlylozVQAArsPj.jpg");

	//====================================================

	//擬似乱数生成器の初期化
	std::random_device seed_gen;
	std::mt19937 engine(seed_gen());

	//一様実数分布
	std::uniform_real_distribution<> dist(-0.02, 0.02);

	//particle生成
	mpParticle = new ParticleCompute[mParticleAmount];
	for (int i = 0; i < mParticleAmount; i++)
	{
		float x = static_cast<float>(dist(engine));
		float y = static_cast<float>(dist(engine));
		float z = static_cast<float>(dist(engine));

		mpParticle[i].position= float3(0, 0, 0);
		mpParticle[i].velocity = float3(x, y, z);
		mpParticle[i].lifeSpan = 300.0f;
	}

	//Buffer作成
	hr = CreateStructuredBuffer(sizeof(ParticleCompute), (UINT)mParticleAmount, nullptr, &mpParticleBuffer, false);
	hr = CreateStructuredBuffer(sizeof(float3), (UINT)mParticleAmount, nullptr, &mpPositionBuffer, false);
	hr = CreateStructuredBuffer(sizeof(ParticleCompute), (UINT)mParticleAmount, nullptr, &mpResultBuffer, true);

	//SRV生成
	hr = CreateBufferSRV(mpParticleBuffer, &mpParticleSRV);
	hr = CreateBufferSRV(mpPositionBuffer, &mpPositionSRV);

	//UAV生成
	hr = CreateBufferUAV(mpResultBuffer, &mpResultUAV);

	return true;
}

//-----------------------------------------------------------------------------
//更新
//-----------------------------------------------------------------------------
void GPUParticleShader::Update()
{
	// パーティクルの資料をバッファに入れる
	{
		D3D11_MAPPED_SUBRESOURCE pData;
		if (SUCCEEDED(g_graphicsDevice->g_cpContext.Get()->Map(mpParticleBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
		{
			size_t size = sizeof(ParticleCompute) * mParticleAmount;
			memcpy_s(pData.pData, size, mpParticle, size);
			g_graphicsDevice->g_cpContext.Get()->Unmap(mpParticleBuffer, 0);
		}
	}

	//コンピュートシェーダー実行/粒子シュミレーション
	ID3D11ShaderResourceView* pSRVs[1] = { mpParticleSRV };
	g_graphicsDevice->g_cpContext.Get()->CSSetShaderResources(0, 1, pSRVs);
	g_graphicsDevice->g_cpContext.Get()->CSSetShader(m_cpCS.Get(), 0, 0);
	g_graphicsDevice->g_cpContext.Get()->CSSetUnorderedAccessViews(0, 1, &mpResultUAV, 0);
	g_graphicsDevice->g_cpContext.Get()->Dispatch(256, 1, 1);

	// 戻った計算結果をバッファに入れる
	{
		//CPUアクセス変更(読み込み) -> 結果コピー
		ID3D11Buffer* pResultBufCpy = CreateAndCopyToDebugBuf(mpResultBuffer);

		D3D11_MAPPED_SUBRESOURCE pData;
		if (SUCCEEDED(g_graphicsDevice->g_cpContext.Get()->Map(pResultBufCpy, 0, D3D11_MAP_READ, 0, &pData)))
		{
			mpParticle[0].velocity;
			mpParticle[0].position;

			size_t size = sizeof(ParticleCompute) * mParticleAmount;
			//memcpy_s(pData.pData, size, mpParticle, size);
			memcpy_s(mpParticle, size, pData.pData, size);
			g_graphicsDevice->g_cpContext.Get()->Unmap(pResultBufCpy, 0);
			pResultBufCpy->Release();

			mpParticle[0].velocity;
			mpParticle[0].position;
		}
	}

	// 座標を座標バッファに入れる(頂点シェーダーで使う)
	{
		D3D11_MAPPED_SUBRESOURCE pData;
		if (SUCCEEDED(g_graphicsDevice->g_cpContext.Get()->Map(mpPositionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
		{
			/*for (int i = 0; i < mParticleAmount; i++)
				mpParticle->position += mpParticle->velocity;*/

			size_t size = sizeof(float3) * mParticleAmount;

			float3* pBufType = (float3*)pData.pData;
			for (int v = 0; v < mParticleAmount; v++) {
				pBufType[v] = mpParticle[v].position;
			}
			//memcpy_s(pData.pData, size , &mpParticle->position, size);
			g_graphicsDevice->g_cpContext.Get()->Unmap(mpPositionBuffer, 0);
		}
	}

	//nullresourceの設定
	ID3D11ShaderResourceView* nullSRVs[1] = { nullptr };
	ID3D11UnorderedAccessView* nullUAVs[1] = { nullptr };
	g_graphicsDevice->g_cpContext.Get()->CSSetShaderResources(0, 1, nullSRVs);
	g_graphicsDevice->g_cpContext.Get()->CSSetUnorderedAccessViews(0, 1, nullUAVs, 0);
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

	//テクスチャ/座標(SRV)
	if (m_spTexture != nullptr)
		g_graphicsDevice->g_cpContext.Get()->PSSetShaderResources(0, 1, m_spTexture->SRVAddress());
	g_graphicsDevice->g_cpContext.Get()->VSSetShaderResources(2, 1, &mpPositionSRV);

	//シェーダー設定
	g_graphicsDevice->g_cpContext.Get()->IASetInputLayout(m_cpInputLayout.Get());

	g_graphicsDevice->g_cpContext.Get()->VSSetShader(m_cpVS.Get(), 0, 0);
	g_graphicsDevice->g_cpContext.Get()->PSSetShader(m_cpPS.Get(), 0, 0);

	//描画
	if (m_cullNone) RENDERER.SetRasterize(RS_CullMode::eCullNone, RS_FillMode::eSolid);
	//g_graphicsDevice->g_cpContext.Get()->Draw(4, 0);
	g_graphicsDevice->g_cpContext.Get()->DrawInstanced(4, mParticleAmount, 0, 0);
	if (m_cullNone) RENDERER.SetRasterize(RS_CullMode::eBack, RS_FillMode::eSolid);

	//nullresourceの設定
	ID3D11ShaderResourceView* nullSRVs[1] = { nullptr };
	g_graphicsDevice->g_cpContext.Get()->VSSetShaderResources(2, 1, nullSRVs);
}

//-----------------------------------------------------------------------------
//StructuredBuffer想定バッファー作成
//-----------------------------------------------------------------------------
HRESULT GPUParticleShader::CreateStructuredBuffer(UINT uElementSize, UINT uCount, void* pInitData, ID3D11Buffer** ppBufOut, bool isUAV)
{
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = uElementSize * uCount;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = uElementSize;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	if (isUAV) {
		//順不同アクセスビューの場合
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		desc.Usage = D3D11_USAGE_DEFAULT;
	}
	else {
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	}

	if (pInitData) {
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = pInitData;
		return g_graphicsDevice->g_cpDevice.Get()->CreateBuffer(&desc, &InitData, ppBufOut);
	}
	else {
		return g_graphicsDevice->g_cpDevice.Get()->CreateBuffer(&desc, nullptr, ppBufOut);
	}
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
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.BufferEx.FirstElement = 0;
	desc.BufferEx.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;

	return g_graphicsDevice->g_cpDevice.Get()->CreateShaderResourceView(pBuffer, &desc, ppSRVOut);
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
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.Buffer.FirstElement = 0;
	desc.Buffer.NumElements = descBuf.ByteWidth / descBuf.StructureByteStride;

	return g_graphicsDevice->g_cpDevice.Get()->CreateUnorderedAccessView(pBuffer, &desc, ppUAVOut);
}

//-----------------------------------------------------------------------------
//CPUアクセスレベルをD3D11_CPU_ACCESS_READに変更しコピー
//-----------------------------------------------------------------------------
ID3D11Buffer* GPUParticleShader::CreateAndCopyToDebugBuf(ID3D11Buffer* pBuffer)
{
	ID3D11Buffer* debugbuf = nullptr;

	D3D11_BUFFER_DESC desc = {};
	pBuffer->GetDesc(&desc);

	//下記変更点
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.Usage = D3D11_USAGE_STAGING;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	if (SUCCEEDED(g_graphicsDevice->g_cpDevice.Get()->CreateBuffer(&desc, nullptr, &debugbuf)))
		g_graphicsDevice->g_cpContext.Get()->CopyResource(debugbuf, pBuffer);
	return debugbuf;
}

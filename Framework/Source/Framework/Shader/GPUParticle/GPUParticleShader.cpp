﻿#include "GPUParticleShader.h"

//-----------------------------------------------------------------------------
//コンストラクタ
//-----------------------------------------------------------------------------
GPUParticleShader::GPUParticleShader()
	: mParticleAmount(128000 * 1)
	, m_cpCS(nullptr)
	, m_spVertexBuffer(nullptr)
	, m_spTexture(nullptr)
	, m_billboard(true)
	, m_cullNone(true)
{
}

//-----------------------------------------------------------------------------
//デストラクタ
//-----------------------------------------------------------------------------
GPUParticleShader::~GPUParticleShader()
{
	delete[] mpParticle;
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
			{ "COLOR"	, 0, DXGI_FORMAT_R32G32B32A32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
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

	//バッファー/ビューの作成
	{
		//頂点定義
		static constexpr cfloat4x4 color = cfloat4x4(1, 1, 1, 1.0f);
		static constexpr float rectSize = 0.2f;
		Vertex vertices[]{
			{ float3(-rectSize, -rectSize,  0.0f), float2(0, 1), color },
			{ float3(-rectSize,  rectSize,  0.0f), float2(0, 0), color },
			{ float3( rectSize, -rectSize,  0.0f), float2(1, 1), color },
			{ float3( rectSize,  rectSize,  0.0f), float2(1, 0), color },
		};

		//頂点バッファーの作成
		const int size = sizeof(Vertex) * 4;
		m_spVertexBuffer = std::make_shared<Buffer>();
		m_spVertexBuffer->Create(D3D11_BIND_VERTEX_BUFFER, size, D3D11_USAGE_DYNAMIC, nullptr);
		m_spVertexBuffer->WriteData(&vertices[0], size);

		//シミュレーション入力データ用バッファーの作成
		m_spInputBuffer = std::make_shared<Buffer>();
		m_spInputBuffer->CreateStructured(sizeof(ParticleCompute), (UINT)mParticleAmount, false);

		//計算シェーダからの結果受け取り用バッファーの作成
		m_spResultBuffer = std::make_shared<Buffer>();
		m_spResultBuffer->CreateStructured(sizeof(ParticleCompute), (UINT)mParticleAmount, true);

		//計算結果から座標を取得してそれを入力用バッファーの作成
		m_spPositionBuffer = std::make_shared<Buffer>();
		m_spPositionBuffer->CreateStructured(sizeof(float3), (UINT)mParticleAmount, false);

		//SRVの生成
		hr = g_graphicsDevice->CreateBufferSRV(m_spInputBuffer->Get(), &mpParticleSRV);
		hr = g_graphicsDevice->CreateBufferSRV(m_spPositionBuffer->Get(), &mpPositionSRV);

		//UAVの生成
		hr = g_graphicsDevice->CreateBufferUAV(m_spResultBuffer->Get(), &mpResultUAV);
	}

	//パーティクル構造体の初期化
	//TOOD: 発生に関しても計算シェーダーでよさそう
	{
		//擬似乱数生成器の初期化
		std::random_device seed_gen;
		std::mt19937 engine(seed_gen());

		//一様実数分布
		std::uniform_real_distribution<float> dist(-4.0f, 4.0f);

		//particle生成
		mpParticle = new ParticleCompute[mParticleAmount];
		for (int i = 0; i < mParticleAmount; i++)
		{
			float x = dist(engine);
			float y = dist(engine);
			float z = dist(engine);

			mpParticle[i].position = float3(0, 0, 0);
			mpParticle[i].velocity = float3(x, y, z);
			mpParticle[i].lifeSpan = 30.0f;
		}
	}

	//テクスチャ
	m_spTexture = std::make_shared<Texture>();
	m_spTexture->Create("Resource/Texture/sample.jpg");

	return true;
}

//-----------------------------------------------------------------------------
//更新
//-----------------------------------------------------------------------------
void GPUParticleShader::Update()
{
	//粒子シミュレーション用データの書き込み
	m_spInputBuffer->WriteData(mpParticle, sizeof(ParticleCompute) * mParticleAmount);

	//コンピュートシェーダー実行/粒子のシュミレーション
	{
		ID3D11ShaderResourceView* pSRVs[1] = { mpParticleSRV };
		g_graphicsDevice->g_cpContext.Get()->CSSetShaderResources(0, 1, pSRVs);
		g_graphicsDevice->g_cpContext.Get()->CSSetShader(m_cpCS.Get(), 0, 0);
		g_graphicsDevice->g_cpContext.Get()->CSSetUnorderedAccessViews(0, 1, &mpResultUAV, 0);
		g_graphicsDevice->g_cpContext.Get()->Dispatch(256, 1, 1);//X次元256でディスパッチ
	}

	//シミュレーション結果データ バインド
	{
		//CPUアクセス変更(読み込み) -> 結果コピー
		ID3D11Buffer* pResultBufCpy = Buffer::CreateAndCopyToDebugBuf(m_spResultBuffer->Get());

		D3D11_MAPPED_SUBRESOURCE pData;
		if (SUCCEEDED(g_graphicsDevice->g_cpContext.Get()->Map(pResultBufCpy, 0, D3D11_MAP_READ, 0, &pData)))
		{
			size_t size = sizeof(ParticleCompute) * mParticleAmount;
			memcpy_s(mpParticle, size, pData.pData, size);
			g_graphicsDevice->g_cpContext.Get()->Unmap(pResultBufCpy, 0);
			pResultBufCpy->Release();
		}
	}

	//座標データバインド ※頂点シェーダーで使用
	{
		//構造体の特定メンバ(position)のみのアドレス配列作成
		//TODO: 粒子構造体で作成してもいいかも
		float3* posPtr = new float3[mParticleAmount];
		for (int v = 0; v < mParticleAmount; v++)
			posPtr[v] = mpParticle[v].position;

		m_spPositionBuffer->WriteData(posPtr, sizeof(float3) * mParticleAmount);

		delete[] posPtr;
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
	{
		RENDERER.SetResources(m_spTexture.get(), 0);
	}
	g_graphicsDevice->g_cpContext.Get()->VSSetShaderResources(2, 1, &mpPositionSRV);

	//シェーダー設定
	g_graphicsDevice->g_cpContext.Get()->IASetInputLayout(m_cpInputLayout.Get());

	g_graphicsDevice->g_cpContext.Get()->VSSetShader(m_cpVS.Get(), 0, 0);
	g_graphicsDevice->g_cpContext.Get()->PSSetShader(m_cpPS.Get(), 0, 0);

	//インスタンシング描画
	if (m_cullNone) RENDERER.SetRasterize(RS_CullMode::eCullNone, RS_FillMode::eSolid);
	g_graphicsDevice->g_cpContext.Get()->DrawInstanced(4, mParticleAmount, 0, 0);
	if (m_cullNone) RENDERER.SetRasterize(RS_CullMode::eBack, RS_FillMode::eSolid);

	//nullresourceの設定
	ID3D11ShaderResourceView* nullSRVs[1] = { nullptr };
	g_graphicsDevice->g_cpContext.Get()->VSSetShaderResources(0, 1, nullSRVs);
	g_graphicsDevice->g_cpContext.Get()->VSSetShaderResources(2, 1, nullSRVs);
}

void GPUParticleShader::Emit()
{
}

void GPUParticleShader::End()
{
}

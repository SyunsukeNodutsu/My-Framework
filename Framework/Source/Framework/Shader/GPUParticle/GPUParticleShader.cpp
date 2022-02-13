#include "GPUParticleShader.h"

//-----------------------------------------------------------------------------
//コンストラクタ
//-----------------------------------------------------------------------------
GPUParticleShader::GPUParticleShader()
	: m_particleMax(0)
	, m_pParticle(nullptr)
	, m_cpCS(nullptr)
	, m_spVertexBuffer(nullptr)
	, m_spInputBuffer(nullptr)
	, m_spResultBuffer(nullptr)
	, m_spPositionBuffer(nullptr)
	, m_cpInputSRV(nullptr)
	, m_cpPositionSRV(nullptr)
	, m_cpResultUAV(nullptr)
	, m_spTexture(nullptr)
	, m_billboard(false)
	, m_cullNone(true)
	, m_lifeSpan(0)
	, isGenerated(false)
	, isGeneratedMutex()
{
}

//-----------------------------------------------------------------------------
//デストラクタ
//-----------------------------------------------------------------------------
GPUParticleShader::~GPUParticleShader()
{
	End();
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
		{ float3(rectSize, -rectSize,  0.0f), float2(1, 1) },
		{ float3(rectSize,  rectSize,  0.0f), float2(1, 0) },
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
	//TODO: テクスチャとvertexシェーダーはエミッターごとに変える

	constexpr static UINT strides = sizeof(Vertex);
	constexpr static UINT offsets = 0;
	g_graphicsDevice->g_cpContext.Get()->IASetVertexBuffers(0, 1, m_spVertexBuffer->GetAddress(), &strides, &offsets);

	g_graphicsDevice->g_cpContext.Get()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//テクスチャ/座標(SRV)
	if (m_spTexture != nullptr) RENDERER.SetResources(m_spTexture.get(), 0);

	g_graphicsDevice->g_cpContext.Get()->IASetInputLayout(m_cpInputLayout.Get());

	g_graphicsDevice->g_cpContext.Get()->VSSetShader(m_cpVS.Get(), 0, 0);
	g_graphicsDevice->g_cpContext.Get()->PSSetShader(m_cpPS.Get(), 0, 0);
}

//-----------------------------------------------------------------------------
//更新
//-----------------------------------------------------------------------------
void GPUParticleShader::Update()
{
	if (!Done()) return;

	//粒子シミュレーション用データの書き込み
	m_spInputBuffer->WriteData(m_pParticle, sizeof(ParticleCompute) * m_particleMax);

	//コンピュートシェーダー実行/粒子のシュミレーション
	//TODO: エミッターを複数作成した場合に エミッター毎にDispatchするのはよくない
	{
		ID3D11ShaderResourceView* pSRVs[1] = { m_cpInputSRV.Get() };
		g_graphicsDevice->g_cpContext.Get()->CSSetShaderResources(0, 1, pSRVs);
		g_graphicsDevice->g_cpContext.Get()->CSSetShader(m_cpCS.Get(), 0, 0);
		g_graphicsDevice->g_cpContext.Get()->CSSetUnorderedAccessViews(0, 1, m_cpResultUAV.GetAddressOf(), 0);
		g_graphicsDevice->g_cpContext.Get()->Dispatch(256, 1, 1);//X次元256でディスパッチ
	}

	//シミュレーション結果データ バインド
	{
		//CPUアクセス変更(読み込み) -> 結果コピー
		ID3D11Buffer* pResultBufCpy = Buffer::CreateAndCopyToDebugBuf(m_spResultBuffer->Get());

		D3D11_MAPPED_SUBRESOURCE pData;
		if (SUCCEEDED(g_graphicsDevice->g_cpContext.Get()->Map(pResultBufCpy, 0, D3D11_MAP_READ, 0, &pData)))
		{
			size_t size = sizeof(ParticleCompute) * m_particleMax;
			memcpy_s(m_pParticle, size, pData.pData, size);
			g_graphicsDevice->g_cpContext.Get()->Unmap(pResultBufCpy, 0);
			pResultBufCpy->Release();
		}
	}

	//座標データバインド ※頂点シェーダーで使用
	m_spPositionBuffer->WriteData(m_pParticle, sizeof(ParticleCompute) * m_particleMax);

	g_graphicsDevice->g_cpContext.Get()->VSSetShaderResources(2, 1, m_cpPositionSRV.GetAddressOf());

	//nullresourceの設定
	ID3D11ShaderResourceView* nullSRVs[1] = { nullptr };
	ID3D11UnorderedAccessView* nullUAVs[1] = { nullptr };
	g_graphicsDevice->g_cpContext.Get()->CSSetShaderResources(0, 1, nullSRVs);
	g_graphicsDevice->g_cpContext.Get()->CSSetUnorderedAccessViews(0, 1, nullUAVs, 0);

	//生存期間の確認
	m_lifeSpan -= static_cast<float>(ApplicationChilled::GetApplication()->g_fpsTimer->GetDeltaTime());
	if (m_lifeSpan <= 0)
		End();
}

//-----------------------------------------------------------------------------
//描画
//-----------------------------------------------------------------------------
void GPUParticleShader::Draw()
{
	if (!Done()) return;

	Begin();

	//インスタンシング描画
	g_graphicsDevice->g_cpContext.Get()->DrawInstanced(4, m_particleMax, 0, 0);

	//nullresourceの設定
	ID3D11ShaderResourceView* nullSRVs[1] = { nullptr };
	g_graphicsDevice->g_cpContext.Get()->VSSetShaderResources(0, 1, nullSRVs);
	g_graphicsDevice->g_cpContext.Get()->VSSetShaderResources(2, 1, nullSRVs);
}

//-----------------------------------------------------------------------------
//発生
//-----------------------------------------------------------------------------
void GPUParticleShader::Emit(UINT particleMax, EmitData data, std::string_view textureFilepath, bool flip)
{
	if (particleMax == 0) return;

	m_particleMax = particleMax;
	m_lifeSpan = data.maxLifeSpan;

	//ここはスレッドセーフじゃない
	{
		//シミュレーション入力データ用バッファーの作成
		m_spInputBuffer = std::make_shared<Buffer>();
		m_spInputBuffer->CreateStructured(sizeof(ParticleCompute), (UINT)m_particleMax, false);

		//計算シェーダからの結果受け取り用バッファーの作成
		m_spResultBuffer = std::make_shared<Buffer>();
		m_spResultBuffer->CreateStructured(sizeof(ParticleCompute), (UINT)m_particleMax, true);

		//計算結果から座標を取得してそれを入力するバッファーの作成
		m_spPositionBuffer = std::make_shared<Buffer>();
		m_spPositionBuffer->CreateStructured(sizeof(ParticleCompute), (UINT)m_particleMax, false);

		//SRVの生成
		g_graphicsDevice->CreateBufferSRV(m_spInputBuffer->Get(), m_cpInputSRV.GetAddressOf());
		g_graphicsDevice->CreateBufferSRV(m_spPositionBuffer->Get(), m_cpPositionSRV.GetAddressOf());

		//UAVの生成
		g_graphicsDevice->CreateBufferUAV(m_spResultBuffer->Get(), m_cpResultUAV.GetAddressOf());
	}

	//テクスチャ
	m_spTexture = std::make_shared<Texture>();
	if (textureFilepath.empty()) {
		m_spTexture = g_graphicsDevice->GetWhiteTex();
		ApplicationChilled::GetApplication()->g_imGuiSystem->AddLog("INFO: Texture filepath empty. Use white texture.");
	}
	else {
		m_spTexture->Create(textureFilepath.data());
	}

	SetDone(false);

	std::thread([=]
		{
			//擬似乱数生成器の初期化
			std::random_device seed_gen;
			std::mt19937 engine(seed_gen());

			//一様実数分布
			//TOOD: コストがバカ高そう...発生に関しても計算シェーダーでよさそう
			//TOOD: 最小値が最大値を上回ってないかの確認(expression invalid min max arguments for uniform_real)
			std::uniform_real_distribution<float> distr_pos_x(data.minPosition.x, data.maxPosition.x);
			std::uniform_real_distribution<float> distr_pos_y(data.minPosition.y, data.maxPosition.y);
			std::uniform_real_distribution<float> distr_pos_z(data.minPosition.z, data.maxPosition.z);

			std::uniform_real_distribution<float> distr_vel_x(data.minVelocity.x, data.maxVelocity.x);
			std::uniform_real_distribution<float> distr_vel_y(data.minVelocity.y, data.maxVelocity.y);
			std::uniform_real_distribution<float> distr_vel_z(data.minVelocity.z, data.maxVelocity.z);

			std::uniform_real_distribution<float> distr_life(data.minLifeSpan, data.maxLifeSpan);

			std::uniform_real_distribution<float> distr_col(0.0f, 1.0f);

			//粒子生成
			m_pParticle = new ParticleCompute[m_particleMax];
			for (int i = 0; i < m_particleMax; i++)
			{
				m_pParticle[i].position = float3(distr_pos_x(engine), distr_pos_y(engine), distr_pos_z(engine));
				m_pParticle[i].velocity = float3(distr_vel_x(engine), distr_vel_y(engine), distr_vel_z(engine));
				m_pParticle[i].lifeSpan = distr_life(engine);
				m_pParticle[i].color = float4(distr_col(engine), distr_col(engine), distr_col(engine), 1);
				m_pParticle[i].lifeSpanMax = m_pParticle[i].lifeSpan;
			}

			SetDone(true);
		}
	).detach();
}

//-----------------------------------------------------------------------------
//生存期間の計算を待たずに終了させる
//-----------------------------------------------------------------------------
void GPUParticleShader::End()
{
	m_spInputBuffer.reset();
	m_spResultBuffer.reset();
	m_spPositionBuffer.reset();

	for (;;)
	{
		if (!Done()) continue;

		if (m_pParticle != nullptr)
		{
			delete[] m_pParticle;
			m_pParticle = nullptr;
			break;
		}
	}
}

#include "ParticleSystem.h"

const int ParticleSystem::PARTICLE_MAX = 256000 * 4;

//=============================================================================
//
//ParticleWork
//
//=============================================================================

//-----------------------------------------------------------------------------
//コンストラクタ
//-----------------------------------------------------------------------------
ParticleWork::ParticleWork()
	: m_pParticle(nullptr)
	, m_psTexture(nullptr)
	, m_particleMax(0)
	, m_lifeSpan(0.0f)
	, m_isLoop(false)
	, m_data()
	, m_filepath("")
	, m_spInputBuffer(nullptr)
	, m_spResultBuffer(nullptr)
	, m_spPositionBuffer(nullptr)
	, m_cpInputSRV(nullptr)
	, m_cpPositionSRV(nullptr)
	, m_cpResultUAV(nullptr)
	, isGenerated(false)
	, isGeneratedMutex()
{
}

//-----------------------------------------------------------------------------
//更新
//-----------------------------------------------------------------------------
void ParticleWork::Update(ID3D11ComputeShader* simulationShader, float deltaTime)
{
	if (!Done()) return;
	if (m_pParticle == nullptr) return;

	//粒子シミュレーション用データの書き込み
	m_spInputBuffer->WriteData(m_pParticle, sizeof(ParticleCompute) * m_particleMax);

	//コンピュートシェーダー実行/粒子のシュミレーション
	//TODO: エミッターを複数作成した場合に エミッター毎にDispatchするのはよくない
	{
		ID3D11ShaderResourceView* pSRVs[1] = { m_cpInputSRV.Get() };
		g_graphicsDevice->g_cpContext.Get()->CSSetShaderResources(0, 1, pSRVs);
		g_graphicsDevice->g_cpContext.Get()->CSSetShader(simulationShader, 0, 0);
		g_graphicsDevice->g_cpContext.Get()->CSSetUnorderedAccessViews(0, 1, m_cpResultUAV.GetAddressOf(), 0);
		g_graphicsDevice->g_cpContext.Get()->Dispatch(256, 1, 1);//X次元256でディスパッチ

		//nullリソース設定(解除)
		RENDERER.SetNullResources(0);
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

	//生存期間の確認
	m_lifeSpan -= deltaTime;
	if (m_lifeSpan <= 0)
	{
		if (m_isLoop) Emit(m_particleMax, m_data, m_isLoop, m_filepath);
		else End();
	}
}

//-----------------------------------------------------------------------------
//描画
//-----------------------------------------------------------------------------
void ParticleWork::Draw()
{
	if (!Done()) return;
	if (m_pParticle == nullptr) return;

	//リソース設定
	//RENDERER.SetResources(.get(), 0);
	g_graphicsDevice->g_cpContext.Get()->PSSetShaderResources(0, 1, m_psTexture->SRVAddress());//テクスチャ/座標(SRV)
	g_graphicsDevice->g_cpContext.Get()->VSSetShaderResources(2, 1, m_cpPositionSRV.GetAddressOf());//UAVからの出力結果

	//インスタンシング描画
	g_graphicsDevice->g_cpContext.Get()->DrawInstanced(4, m_particleMax, 0, 0);

	//nullリソース設定(解除)
	RENDERER.SetNullResources(0);
	RENDERER.SetNullResources(2);
}

//-----------------------------------------------------------------------------
//発生
//-----------------------------------------------------------------------------
void ParticleWork::Emit(UINT particleMax, EmitData data, bool loop, std::string_view filepath)
{
	m_particleMax = particleMax;
	m_data = data;
	m_filepath = filepath;
	m_isLoop = loop;
	m_lifeSpan = data.maxLifeSpan;

	m_psTexture = std::make_shared<Texture>();
	if (!m_psTexture->Create(filepath.data()) || filepath.empty()) {
		m_psTexture = g_graphicsDevice->GetWhiteTex();
		ApplicationChilled::GetApplication()->g_imGuiSystem->AddLog("Set White texture.");
	}

	SetupViews();

	SetDone(false);

	//非同期で粒子を生成
	std::thread([=] {
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

	}).detach();
}

//-----------------------------------------------------------------------------
//シミュレーションを終了させる
//-----------------------------------------------------------------------------
void ParticleWork::End()
{
	for (;;)
	{
		if (!Done()) continue;

		if (m_pParticle != nullptr)
		{
			delete[] m_pParticle;
			m_pParticle = nullptr;
		}
		break;
	}
}

//-----------------------------------------------------------------------------
//各ビューの設定
//-----------------------------------------------------------------------------
void ParticleWork::SetupViews()
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



//=============================================================================
//
//ParticleSystem
//
//=============================================================================

//-----------------------------------------------------------------------------
//コンストラクタ
//-----------------------------------------------------------------------------
ParticleSystem::ParticleSystem()
	: m_spParticleList()
{
}

//-----------------------------------------------------------------------------
//更新
//-----------------------------------------------------------------------------
void ParticleSystem::Update(float deltaTime)
{
	const auto& cs = SHADER.GetGPUParticleShader().GetSimulationComputeShader();

	for (auto itr = m_spParticleList.begin(); itr != m_spParticleList.end();)
	{
		if ((*itr) == nullptr) continue;

		//更新
		(*itr)->Update(cs, deltaTime);

		//除外
		if ((*itr)->IsEnd() == true) {
			itr = m_spParticleList.erase(itr);
		}
		else ++itr;
	}
}

//-----------------------------------------------------------------------------
//描画
//-----------------------------------------------------------------------------
void ParticleSystem::Draw(float deltaTime)
{
	RENDERER.SetRasterize(RS_CullMode::eCullNone, RS_FillMode::eSolid);

	SHADER.GetGPUParticleShader().Begin();

	for (auto&& particle : m_spParticleList)
	{
		particle->Draw();
	}

	RENDERER.SetRasterize(RS_CullMode::eBack, RS_FillMode::eSolid);
}

//-----------------------------------------------------------------------------
//発生させる
//-----------------------------------------------------------------------------
void ParticleSystem::Emit(UINT particleMax, EmitData data, bool loop, std::string_view filepath)
{
	std::shared_ptr<ParticleWork> particle = std::make_shared<ParticleWork>();
	particle->Emit(particleMax, data, loop, filepath);

	//描画順の関係上 先頭に追加する必要がある
	m_spParticleList.push_front(particle);
}

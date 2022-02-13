//-----------------------------------------------------------------------------
//File: ParticleSystem.h
//
//粒子シミュレーション管理システム
//複数エミッターを操作
//-----------------------------------------------------------------------------
#pragma once

//発生情報
struct EmitData
{
	float3 maxPosition; float3 minPosition;
	float3 maxVelocity; float3 minVelocity;
	float maxLifeSpan; float minLifeSpan;
	float4 color;
};

//エミッター単位 TODO: 頂点バッファだれが持つ
class ParticleWork : public GraphicsDeviceChild
{
	//粒子単位 定数バッファ(SRV)
	struct ParticleCompute
	{
		float3 position;
		float lifeSpanMax;
		float3 velocity;
		float lifeSpan;
		float4 color;
	};

public:

	//@brief 終了処理
	void Finalize()
	{
		if (m_pParticle != nullptr)
		{
			delete[] m_pParticle;
			m_pParticle = nullptr;
		}
	}

	//@brief 更新
	void Update(ID3D11ComputeShader* simulationShader,float deltaTime)
	{
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
	}

	//@brief 描画
	void Draw()
	{
		//TODO: テクスチャ

		//UAVからの出力結果
		g_graphicsDevice->g_cpContext.Get()->VSSetShaderResources(2, 1, m_cpPositionSRV.GetAddressOf());

		//インスタンシング描画
		g_graphicsDevice->g_cpContext.Get()->DrawInstanced(4, m_particleMax, 0, 0);
	}

	//@brief 発生させる
	void Emit(UINT particleMax, EmitData data)
	{
		m_particleMax = particleMax;
		m_lifeSpan = data.maxLifeSpan;

		SetupViews();

#pragma region スレッド分け
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
#pragma endregion
	}

	//@brief 終了しているかどうかを返す
	bool IsEnd() { return (m_pParticle == nullptr); }

private:

	ParticleCompute* m_pParticle;//粒子
	int m_particleMax;//粒子の数
	float m_lifeSpan;//生存期間

	std::shared_ptr<Buffer> m_spInputBuffer;//シミュレーション準備データ(入力SRV)
	std::shared_ptr<Buffer> m_spResultBuffer;//シミュレーション結果データ(出力UAV)
	std::shared_ptr<Buffer> m_spPositionBuffer;

	//各ビュー
	ComPtr<ID3D11ShaderResourceView> m_cpInputSRV;
	ComPtr<ID3D11ShaderResourceView> m_cpPositionSRV;
	ComPtr<ID3D11UnorderedAccessView> m_cpResultUAV;

private:

	//@brief 各ビューの初期化 ※スレッドセーフじゃないのでメインスレッドで行う(まことに遺憾)
	void SetupViews()
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

};

//粒子システム
class ParticleSystem
{
public:

	//@brief コンストラクタ
	ParticleSystem();

	//@brief デストラクタ
	~ParticleSystem();

	//@brief 更新
	void Update(float deltaTime);

	//@brief 描画
	void Draw(float deltaTime);

	//@brief 発生させる
	void Emit(UINT particleMax, EmitData data);

private:

	static const int PARTICLE_MAX = 256000 * 4;
	std::vector<ParticleWork*> m_pParticleVector;

};

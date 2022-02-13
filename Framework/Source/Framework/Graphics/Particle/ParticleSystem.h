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

//エミッター単位
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

	//@brief コンストラクタ
	ParticleWork();

	//@brief デストラクタ
	~ParticleWork() { End(); }

	//@brief 更新
	void Update(ID3D11ComputeShader* simulationShader, float deltaTime);

	//@brief 描画
	void Draw();

	//@brief 発生させる
	void Emit(UINT particleMax, EmitData data, bool loop = false, std::string_view filepath = "");

	//@brief シミュレーションを終了させる
	void End();

	//@brief 終了しているかどうかを返す
	bool IsEnd() { return (m_pParticle == nullptr); }

private:

	ParticleCompute* m_pParticle;//粒子
	std::shared_ptr<Texture> m_psTexture;//テクスチャ
	int m_particleMax;//粒子の数
	float m_lifeSpan;//生存期間
	bool m_isLoop;//ループ再生するかどうか

	EmitData m_data;
	std::string_view m_filepath;

	//バッファー
	std::shared_ptr<Buffer> m_spInputBuffer;	//シミュレーション準備データ(入力SRV)
	std::shared_ptr<Buffer> m_spResultBuffer;	//シミュレーション結果データ(出力UAV)
	std::shared_ptr<Buffer> m_spPositionBuffer;

	//各ビュー
	ComPtr<ID3D11ShaderResourceView> m_cpInputSRV;
	ComPtr<ID3D11ShaderResourceView> m_cpPositionSRV;
	ComPtr<ID3D11UnorderedAccessView> m_cpResultUAV;

	//非同期生成用
	bool isGenerated;//生成完了
	std::mutex isGeneratedMutex;

private:

	//@brief 各ビューの初期化
	//@note スレッドセーフじゃないのでメインスレッドで行う(まことに遺憾)
	void SetupViews();

	//@brief 生成完了かどうかを返す
	bool Done() {
		std::lock_guard<std::mutex> lock(isGeneratedMutex);
		return isGenerated;
	}

	//@brief 生成完了かどうかを設定
	void SetDone(bool done) {
		std::lock_guard<std::mutex> lock(isGeneratedMutex);
		isGenerated = done;
	}

};

//粒子システム
class ParticleSystem
{
public:

	//@brief コンストラクタ
	ParticleSystem();

	//@brief デストラクタ
	~ParticleSystem() {}

	//@brief 更新
	void Update(float deltaTime);

	//@brief 描画
	void Draw(float deltaTime);

	//@brief 発生させる
	void Emit(UINT particleMax, EmitData data, bool loop = false, std::string_view filepath = "");

	//static
	static const int PARTICLE_MAX;//アプリケーションの最大粒子数

private:

	std::vector<std::shared_ptr<ParticleWork>> m_spParticleVector;

};

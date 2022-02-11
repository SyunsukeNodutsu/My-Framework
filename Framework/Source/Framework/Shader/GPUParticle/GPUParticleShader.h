//-----------------------------------------------------------------------------
//File: GPUParticleShader.h
//
//GPUによる粒子計算
//とりあえず一回きりの発生
//-----------------------------------------------------------------------------
#pragma once

//非同期コンピュート GPUによるparticle
class GPUParticleShader : public Shader
{
	//1頂点
	struct Vertex
	{
		float3 position;
		float2 uv;
	};

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

	//発生情報
	struct EmitData
	{
		float3 maxPosition; float3 minPosition;
		float3 maxVelocity; float3 minVelocity;
		float maxLifeSpan; float minLifeSpan;
		float4 color;
	};

public:

	//@brief コンストラクタ
	GPUParticleShader();

	//@brief デストラクタ
	~GPUParticleShader();

	//@brief 初期化
	bool Initialize();

	//@brief 更新
	void Update();

	//@brief 描画
	void Draw();

	//@brief 発生 ※とりあえず最低限の設定 TODO: 設定用に構造体を用意
	//@param particleMax 発生させる粒子の数
	//@param data 初期化データ
	//@param spTexture 設定するテクスチャ(nullptrで白テクスチャ使用)
	void Emit(UINT particleMax, EmitData data, std::string_view textureFilepath = "");

	//@brief 生存期間の計算を待たずに終了させる
	void End(/*終了モード アルファ値を減少させながら とか*/);

private:

	int m_particleMax;//粒子の数

	ParticleCompute* m_pParticle;//粒子
	ComPtr<ID3D11ComputeShader>	m_cpCS;

	//バッファー
	std::shared_ptr<Buffer> m_spVertexBuffer;
	std::shared_ptr<Buffer> m_spInputBuffer;//シミュレーション準備データ(入力SRV)
	std::shared_ptr<Buffer> m_spResultBuffer;//シミュレーション結果データ(出力UAV)
	std::shared_ptr<Buffer> m_spPositionBuffer;

	//各ビュー
	ComPtr<ID3D11ShaderResourceView> m_cpInputSRV;
	ComPtr<ID3D11ShaderResourceView> m_cpPositionSRV;
	ComPtr<ID3D11UnorderedAccessView> m_cpResultUAV;

	std::shared_ptr<Texture> m_spTexture;//テクスチャ
	bool m_billboard;//ビルボード表示
	bool m_cullNone;//背面カリングOFF
	float m_lifeSpan;

};

//-----------------------------------------------------------------------------
//File: GPUParticleShader.h
//
//GPUによる粒子計算
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
		float4 color;
	};

	//粒子単位 定数バッファ(SRV)
	struct ParticleCompute
	{
		float3 position;
		float tmp;
		float3 velocity;
		float lifeSpan;
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

	//@テクスチャ設定
	//@param pTexture 設定するテクスチャ
	void SetTexture(std::shared_ptr<Texture> spTexture) { m_spTexture = spTexture; }

	//TODO: エミッター作成
	void Emit();
	void End();

private:

	ParticleCompute* mpParticle;
	ComPtr<ID3D11ComputeShader>	m_cpCS;

	//バッファー
	std::shared_ptr<Buffer> m_spVertexBuffer;
	std::shared_ptr<Buffer> m_spInputBuffer;
	std::shared_ptr<Buffer> m_spResultBuffer;
	std::shared_ptr<Buffer> m_spPositionBuffer;

	// SRV
	ID3D11ShaderResourceView* mpParticleSRV = nullptr;
	ID3D11ShaderResourceView* mpPositionSRV = nullptr;

	// UAV
	ID3D11UnorderedAccessView* mpResultUAV = nullptr;

	std::shared_ptr<Texture> m_spTexture;
	bool m_billboard;//ビルボード表示
	bool m_cullNone;//背面カリングOFF
	int mParticleAmount;

};

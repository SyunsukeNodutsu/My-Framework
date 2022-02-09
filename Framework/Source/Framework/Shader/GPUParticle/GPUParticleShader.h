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

	static const int PARTICLE_MAX;//粒子の数

private:

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

};

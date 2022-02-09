//-----------------------------------------------------------------------------
//File: GPUParticleShader.h
//
//GPUによる粒子計算目標 とりあえず板ポリ表示まで
//t ... シェーダー リソース ビュー(SRV)用
//s ... サンプラー用
//u ... 順序指定されていないアクセス ビュー(UAV)用
//b ... 定数バッファー ビュー(CBV)用 
//-----------------------------------------------------------------------------
#pragma once

//非同期コンピュート GPUによるparticle
class GPUParticleShader : public Shader
{
	//1頂点
	struct Vertex
	{
		float3 position;
		float4 m_color;
	};

	//粒子単位 定数バッファ
	struct cbParticle
	{
		float3 position; float tmp;
		float3 velocity;
		float lifeSpan;
	};

public:

	//@brief コンストラクタ
	GPUParticleShader();

	//@brief デストラクタ
	~GPUParticleShader() {}

	//@brief 初期化
	bool Initialize();

	//@brief 更新
	void Update();

	//@brief 描画
	void Draw();

private:

	ComPtr<ID3D11ComputeShader>	m_cpCS;
	ConstantBuffer<cbParticle> m_cb7Particle;
	std::shared_ptr<Buffer> m_spVertexBuffer;
	std::shared_ptr<Texture> m_spTexture;
	bool m_billboard;//ビルボード表示
	bool m_cullNone;//背面カリングOFF

	//TODO: fix
	ID3D11Buffer* mpParticleBuffer;
	ID3D11Buffer* mpResultBuffer;
	ID3D11Buffer* mpPositionBuffer;
	// SRV
	ID3D11ShaderResourceView* mpParticleSRV;
	ID3D11ShaderResourceView* mpPositionSRV;
	// UAV
	ID3D11UnorderedAccessView* mpResultUAV;

private:

	HRESULT CreateBufferSRV(ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut);
	HRESULT CreateBufferUAV(ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAVOut);
	ID3D11Buffer* CreateAndCopyToDebugBuf(ID3D11Buffer* pBuffer);
};

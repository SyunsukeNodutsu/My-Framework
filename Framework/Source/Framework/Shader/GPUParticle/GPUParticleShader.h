//-----------------------------------------------------------------------------
//File: GPUParticleShader.h
//
//非同期コンピュートによる粒子のシミュレーション
//-----------------------------------------------------------------------------
#pragma once

//非同期コンピュート GPUによるparticle
class GPUParticleShader : public Shader
{
	//頂点単位
	struct Vertex
	{
		float3 position;
		float2 uv;
	};

public:

	//@brief コンストラクタ
	GPUParticleShader();

	//@brief デストラクタ
	~GPUParticleShader() {}

	//@brief 初期化
	bool Initialize();

	//@brief 使用開始
	void Begin();

	//@brief シミュレーションに使用する計算シェーダーを返す
	ID3D11ComputeShader* GetSimulationComputeShader() const {
		return m_cpCS.Get();
	}

private:

	ComPtr<ID3D11ComputeShader>	m_cpCS;
	std::shared_ptr<Buffer> m_spVertexBuffer;

};

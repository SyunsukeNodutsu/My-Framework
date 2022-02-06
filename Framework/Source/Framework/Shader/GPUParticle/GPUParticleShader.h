//-----------------------------------------------------------------------------
// File: GPUParticleShader.h
//
// GPUによる粒子計算
//-----------------------------------------------------------------------------
#pragma once

//非同期コンピュート GPUによるparticle
class GPUParticleShader : public Shader
{
	struct Vertex
	{
		float3 position;
		float4 m_color;
	};

	//粒子単位 定数バッファ
	struct cbParticle
	{
		float3 position;
		float tmp;
		float3 velocity;
		float lifeSpan;
	};

public:

	GPUParticleShader();
	~GPUParticleShader()
	{
	}

	bool Initialize();
	void Update();
	void Draw();

private:

	static const int PARTICLE_COUNT = 100;
	ConstantBuffer<cbParticle> m_cb7Particle;
	std::shared_ptr<Buffer> m_vertexBuffer;
	ComPtr<ID3D11ComputeShader> m_cpCS;

};

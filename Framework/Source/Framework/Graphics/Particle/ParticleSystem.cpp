#include "ParticleSystem.h"

//-----------------------------------------------------------------------------
//コンストラクタ
//-----------------------------------------------------------------------------
ParticleSystem::ParticleSystem()
	: m_pParticleVector()
{
}

//-----------------------------------------------------------------------------
//デストラクタ
//-----------------------------------------------------------------------------
ParticleSystem::~ParticleSystem()
{
	for (auto&& particle : m_pParticleVector)
	{
		delete particle;
		particle = nullptr;
	}
}

//-----------------------------------------------------------------------------
//更新
//-----------------------------------------------------------------------------
void ParticleSystem::Update(float deltaTime)
{
	const auto& cs = SHADER.GetGPUParticleShader().GetSimulationComputeShader();

	for (auto&& particle : m_pParticleVector)
	{
		particle->Update(cs, deltaTime);
	}
}

//-----------------------------------------------------------------------------
//描画
//-----------------------------------------------------------------------------
void ParticleSystem::Draw(float deltaTime)
{
	RENDERER.SetSampler(SS_FilterMode::eAniso, SS_AddressMode::eWrap);
	//RENDERER.SetRasterize(RS_CullMode::eCullNone, RS_FillMode::eSolid);

	for (auto&& particle : m_pParticleVector)
	{
		particle->Draw();
	}

	RENDERER.SetRasterize(RS_CullMode::eBack, RS_FillMode::eSolid);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void ParticleSystem::Emit(UINT particleMax, EmitData data)
{
	ParticleWork* particle = new ParticleWork();
	m_pParticleVector.push_back(particle);
}

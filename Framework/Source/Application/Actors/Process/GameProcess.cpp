#include "GameProcess.h"
#include "../../../Framework/Audio/SoundDirector.h"

//-----------------------------------------------------------------------------
//コンストラクタ
//-----------------------------------------------------------------------------
GameProcess::GameProcess()
{
}

//-----------------------------------------------------------------------------
//初期化
//-----------------------------------------------------------------------------
void GameProcess::Initialize()
{
	//BGM再生
	//auto sound = SOUND_DIRECTOR.CreateSoundWork("Resource/Audio/BGM/minzoku28_maoudamasii.wav", true, true);
	//auto sound = SOUND_DIRECTOR.CreateSoundWork("Resource/Audio/BGM/New Happy Day by fennec beats.wav", true, true);
	//auto sound = SOUND_DIRECTOR.CreateSoundWork("Resource/Audio/BGM/VK  Wings Of Piano.wav", true, true);
	auto sound = SOUND_DIRECTOR.CreateSoundWork("Resource/Audio/BGM/Colors feat Shion_Kirara Magic.wav", true, true);
	if (sound) {
		sound->Play(1000);
		sound->SetVolume(1.0f);
		//sound->SetFilter(XAUDIO2_FILTER_TYPE::LowPassFilter, 0.2f);
	}

	m_emitData.minPosition = float3(-10.0f, 20.0f, -10.0f);
	m_emitData.maxPosition = float3(10.0f, 40.0f, 10.0f);

	m_emitData.minVelocity = float3(-2.0f, -2.0f, -2.0f);
	m_emitData.maxVelocity = float3(2.0f, 2.0f, 2.0f);

	m_emitData.minLifeSpan = 5.0f;
	m_emitData.maxLifeSpan = 6.0f;
	m_emitData.color = cfloat4x4::White;

	SHADER.GetGPUParticleShader().Emit(256000 * 2, m_emitData, "Resource/Texture/test.png");
}

//-----------------------------------------------------------------------------
//更新
//-----------------------------------------------------------------------------
void GameProcess::Update(float deltaTime)
{
	if (SHADER.GetGPUParticleShader().IsEnd())
		SHADER.GetGPUParticleShader().Emit(256000 * 2, m_emitData, "Resource/Texture/test.png");
}

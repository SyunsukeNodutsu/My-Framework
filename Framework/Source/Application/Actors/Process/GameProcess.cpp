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

	auto& particleSystem = g_application->g_gameSystem->g_particleSystem;
	
	m_deta.minPosition = float3(-10.0f, 20.0f, -10.0f);
	m_deta.maxPosition = float3(10.0f, 40.0f, 10.0f);

	m_deta.minVelocity = float3(-2.0f, -2.0f, -2.0f);
	m_deta.maxVelocity = float3(2.0f, 2.0f, 2.0f);

	m_deta.minLifeSpan = 10.0f;
	m_deta.maxLifeSpan = 11.0f;
	m_deta.color = cfloat4x4::White;

	particleSystem.Emit(2560 * 2, m_deta, false, "Resource/Texture/test.png");
}

//-----------------------------------------------------------------------------
//更新
//-----------------------------------------------------------------------------
void GameProcess::Update(float deltaTime)
{
	static float timer = 0.0f;
	timer += deltaTime;

	if (timer >= 6.0f)
	{
		timer = 0.0f;

		auto& particleSystem = g_application->g_gameSystem->g_particleSystem;

		m_deta.minPosition = float3(0.0f, 10.0f, 10.0f);
		m_deta.maxPosition = float3(0.0f, 10.0f, 10.0f);

		particleSystem.Emit(2560 * 2, m_deta, false, "Resource/Texture/test.png");
	}
}

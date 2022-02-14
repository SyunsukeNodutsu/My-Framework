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
	m_sound = SOUND_DIRECTOR.CreateSoundWork("Resource/Audio/BGM/VK  Wings Of Piano.wav", true, true);
	//auto sound = SOUND_DIRECTOR.CreateSoundWork("Resource/Audio/BGM/Colors feat Shion_Kirara Magic.wav", true, true);
	if (m_sound) {
		m_sound->Play(1000);
		m_sound->SetVolume(1.0f);
		m_sound->SetFilter(XAUDIO2_FILTER_TYPE::LowPassFilter, 0.3f);
	}

	auto& particleSystem = g_application->g_gameSystem->g_particleSystem;

	const float vel = 10.0f;
	m_deta.minVelocity = float3(-vel, -vel, -vel);
	m_deta.maxVelocity = float3(vel, vel, vel);

	m_deta.minLifeSpan = 10.0f;
	m_deta.maxLifeSpan = 11.0f;
	m_deta.color = cfloat4x4::White;
}

//-----------------------------------------------------------------------------
//更新
//-----------------------------------------------------------------------------
void GameProcess::Update(float deltaTime)
{
	static int typetest = 0;

	static float timer = 0.0f;
	timer += deltaTime;

	if (timer >= 2.0f)
	{
		timer = 0.0f;
		typetest++;

		auto& particleSystem = g_application->g_gameSystem->g_particleSystem;

		const float z = 40.0f;

		m_deta.minPosition = float3(30.0f, 20.0f, z);
		m_deta.maxPosition = float3(30.0f, 20.0f, z);

		particleSystem.Emit(2560 * 2, m_deta, false, "Resource/Texture/test.png");

		m_deta.minPosition = float3(0.0f, 40.0f, z + 30);
		m_deta.maxPosition = float3(0.0f, 40.0f, z + 30);

		particleSystem.Emit(2560 * 2, m_deta, false, "Resource/Texture/test.png");

		m_deta.minPosition = float3(-30.0f, 14.0f, z + 10);
		m_deta.maxPosition = float3(-30.0f, 14.0f, z + 10);

		particleSystem.Emit(2560 * 2, m_deta, false, "Resource/Texture/test.png");
	}
}

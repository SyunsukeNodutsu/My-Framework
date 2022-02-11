#include "GameProcess.h"
#include "../../../Framework/Audio/SoundDirector.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
GameProcess::GameProcess()
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void GameProcess::Initialize()
{
	// BGM再生
	//auto sound = SOUND_DIRECTOR.CreateSoundWork("Resource/Audio/BGM/minzoku28_maoudamasii.wav", true, true);
	//auto sound = SOUND_DIRECTOR.CreateSoundWork("Resource/Audio/BGM/New Happy Day by fennec beats.wav", true, true);
	//auto sound = SOUND_DIRECTOR.CreateSoundWork("Resource/Audio/BGM/VK  Wings Of Piano.wav", true, true);
	auto sound = SOUND_DIRECTOR.CreateSoundWork("Resource/Audio/BGM/Colors feat Shion_Kirara Magic.wav", true, true);
	if (sound) {
		sound->Play(1000);
		sound->SetVolume(1.0f);
		sound->SetFilter(XAUDIO2_FILTER_TYPE::LowPassFilter, 0.2f);
	}

	std::uniform_real_distribution<float> distr_vel(-2.0f, -0.1f);
	std::uniform_real_distribution<float> distr_col(0.0f, 1.0f);

	GPUParticleShader::EmitData data;
	data.minPosition = float3(-100.0f, 10.0f, -100.0f);
	data.maxPosition = float3( 100.0f, 50.0f,  100.0f);

	data.minVelocity = float3(0.0f, -2.0f, 0.0f);
	data.maxVelocity = float3(0.0f, -0.1f, 0.0f);

	data.minLifeSpan = 18;
	data.maxLifeSpan = 20;
	data.color = cfloat4x4::White;

	//SHADER.GetGPUParticleShader().Emit(256000 * 1, data);
	SHADER.GetGPUParticleShader().Emit(25600 * 1, data, "Resource/Texture/test.png");
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void GameProcess::Draw(float)
{
	
}

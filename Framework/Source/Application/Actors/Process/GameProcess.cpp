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
	auto sound = SOUND_DIRECTOR.CreateSoundWork("Resource/Audio/BGM/New Happy Day by fennec beats.wav", true, true);
	if (sound) {
		sound->Play(1000);
		sound->SetVolume(1.0f);
	}
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void GameProcess::Draw(float)
{
	
}

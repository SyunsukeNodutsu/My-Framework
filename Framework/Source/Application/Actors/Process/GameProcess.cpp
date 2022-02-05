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
	auto sound = SOUND_DIRECTOR.CreateSoundWork("Resource/Audio/BGM/魔王魂  民族28.wav", true, true);
	if (sound) {
		sound->Play(1000);
		sound->SetVolume(0.36f);
	}
}

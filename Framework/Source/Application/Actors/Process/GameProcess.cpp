#include "GameProcess.h"
#include "../../main.h"

#include "../../../Framework/Audio/SoundDirector.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
GameProcess::GameProcess()
	: m_spPauseBackGround(nullptr)
	, m_pause(false)
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void GameProcess::Initialize()
{
	// BGM再生
	auto sound = SOUND_DIRECTOR.CreateSoundWork("Resource/Audio/BGM/Germany.wav", true, true);
	if (sound) {
		sound->Play(1000);
		sound->SetVolume(0.36f);
	}

	// テクスチャ読み込み1
	m_spPauseBackGround = std::make_shared<Texture>();
	m_spPauseBackGround->Create("Resource/Texture/PauseBackGround.png");
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void GameProcess::Update(float deltaTime)
{
	m_pause = ApplicationChilled::GetApplication()->g_gameSystem->g_cameraSystem.IsEditorMode();
}

//-----------------------------------------------------------------------------
// スプライト描画
//-----------------------------------------------------------------------------
void GameProcess::DrawSprite(float deltaTime)
{
	if (!m_pause) return;

	//SHADER.GetSpriteShader().DrawTexture(m_spPauseBackGround.get(), float2::Zero, cfloat4x4(1.0f, 1.0f, 1.0f, 0.9f));
}

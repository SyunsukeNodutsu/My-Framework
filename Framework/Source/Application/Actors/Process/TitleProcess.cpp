#include "TitleProcess.h"
#include "../../main.h"
#include "../../../Framework/Audio/SoundDirector.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
TitleProcess::TitleProcess()
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void TitleProcess::Initialize()
{
	m_spTextureBackGround = RES_FAC.GetTexture("Resource/Texture/TitleBackGround.png");

	m_spCamera = std::make_shared<Camera>();
	if (m_spCamera)
	{
		m_spCamera->g_name = "Camera2D";
		m_spCamera->g_priority = 1.0f;

		APP.g_gameSystem->g_cameraSystem.AddCameraList(m_spCamera);
	}
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void TitleProcess::Update(float deltaTime)
{
	if (APP.g_rawInputDevice->g_spMouse->IsPressed(MouseButton::Left) ||
		APP.g_rawInputDevice->g_spMouse->IsPressed(MouseButton::Right))
	{
		SOUND_DIRECTOR.Play("Resource/Audio/SE/Select.wav");
		APP.g_gameSystem->RequestChangeScene("Resource/Jsons/GameProcess.json");
	}
}

//-----------------------------------------------------------------------------
// スプライト描画
//-----------------------------------------------------------------------------
void TitleProcess::DrawSprite(float deltaTime)
{
	// 背景
	SHADER.GetSpriteShader().DrawTexture(m_spTextureBackGround.get(), float2(0, 0));
}

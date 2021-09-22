#include "ShaderManager.h"
#include "../../Application/main.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
ShaderManager::ShaderManager()
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void ShaderManager::Initialize()
{
	// EffectShader
	m_spEffectShader = std::make_shared<EffectShader>();
	m_spEffectShader->Initialize();

	// ModelShader
	m_spModelShader = std::make_shared<ModelShader>();
	m_spModelShader->Initialize();

	// SpriteShader
	m_spSpriteShader = std::make_shared<SpriteShader>();
	m_spSpriteShader->Initialize();

	APP.g_imGuiSystem->AddLog("INFO: ShaderManager initialized.");
}

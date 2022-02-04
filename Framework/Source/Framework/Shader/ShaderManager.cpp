#include "ShaderManager.h"

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
	m_spEffectShader = std::make_shared<EffectShader>();
	m_spModelShader = std::make_shared<ModelShader>();
	m_spSpriteShader = std::make_shared<SpriteShader>();

	m_spEffectShader->Initialize();
	m_spModelShader->Initialize();
	m_spSpriteShader->Initialize();

	ApplicationChilled::GetApplication()->g_imGuiSystem->AddLog("INFO: ShaderManager initialized.");
}

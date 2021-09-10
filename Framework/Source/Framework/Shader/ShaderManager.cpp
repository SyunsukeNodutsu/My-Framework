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
	// ModelShader
	m_spModelShader = std::make_shared<ModelShader>();
	m_spModelShader->Initialize();

	// SpriteShader
	m_spSpriteShader = std::make_shared<SpriteShader>();
	m_spSpriteShader->Initialize();
}

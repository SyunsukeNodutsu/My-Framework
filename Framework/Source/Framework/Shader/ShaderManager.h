//-----------------------------------------------------------------------------
// File: ShaderManager.h
//
// シェーダーの管理
// Device classから移動
//-----------------------------------------------------------------------------
#pragma once
#include "Model/ModelShader.h"
#include "Sprite/SpriteShader.h"

// シェーダーの管理クラス
class ShaderManager
{
public:

	// @brief 初期化
	void Initialize();

	//--------------------------------------------------
	// 取得
	//--------------------------------------------------

	// @brief インスタンスを返す
	// @return シングルトン・インスタンス
	static ShaderManager& GetInstance() {
		static ShaderManager instance; return instance;
	}

	// @brief Modelシェーダを返す
	// @return Modelシェーダ
	ModelShader& GetModelShader() const { return *m_spModelShader; }

	// @brief Spriteシェーダを返す
	// @return Spriteシェーダ
	SpriteShader& GetSpriteShader() const { return *m_spSpriteShader; }

private:

	std::shared_ptr<ModelShader>	m_spModelShader;	// 3Dモデル描画シェーダ
	std::shared_ptr<SpriteShader>	m_spSpriteShader;	// 画像描画用シェーダ

private:

	// @brief コンストラクタ
	ShaderManager();

	// @brief デストラクタ
	~ShaderManager() = default;
};

//-----------------------------------------------------------------------------
// define: 簡単にアクセス
//-----------------------------------------------------------------------------
#define SHADER ShaderManager::GetInstance()

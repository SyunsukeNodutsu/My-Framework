//-----------------------------------------------------------------------------
// File: ShaderManager.h
//
// シェーダーの管理
// TODO: Shader基底クラスの作成
//-----------------------------------------------------------------------------
#pragma once
#include "Effect/EffectShader.h"
#include "Model/ModelShader.h"
#include "Sprite/SpriteShader.h"
#include "GPUParticle/GPUParticleShader.h"

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

	// @brief Effectシェーダを返す
	// @return Effectシェーダ
	EffectShader& GetEffectShader() const { return *m_spEffectShader; }

	// @brief Modelシェーダを返す
	// @return Modelシェーダ
	ModelShader& GetModelShader() const { return *m_spModelShader; }

	// @brief Spriteシェーダを返す
	// @return Spriteシェーダ
	SpriteShader& GetSpriteShader() const { return *m_spSpriteShader; }

	//
	GPUParticleShader& GetGPUParticleShader() const { return *m_spGPUParticleShader; }

private:

	std::shared_ptr<EffectShader>	m_spEffectShader;	// Effect描画シェーダ
	std::shared_ptr<ModelShader>	m_spModelShader;	// 3Dモデル描画シェーダ
	std::shared_ptr<SpriteShader>	m_spSpriteShader;	// 画像描画用シェーダ
	std::shared_ptr<GPUParticleShader>m_spGPUParticleShader;

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

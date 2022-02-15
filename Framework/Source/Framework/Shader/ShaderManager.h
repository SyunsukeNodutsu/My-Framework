//-----------------------------------------------------------------------------
//File: ShaderManager.h
//
//シェーダーの管理
//-----------------------------------------------------------------------------
#pragma once
#include "Effect/EffectShader.h"
#include "Model/ModelShader.h"
#include "Sprite/SpriteShader.h"
#include "GPUParticle/GPUParticleShader.h"
#include "PostProcess/PostProcessShader.h"

//シェーダーの管理クラス
class ShaderManager
{
public:

	//@brief 初期化
	void Initialize();

	//@brief インスタンスを返す
	//@return シングルトン・インスタンス
	static ShaderManager& GetInstance() {
		static ShaderManager instance; return instance;
	}

	//シェーダーー取得
	EffectShader& GetEffectShader() const { return *m_spEffectShader; }
	ModelShader& GetModelShader() const { return *m_spModelShader; }
	SpriteShader& GetSpriteShader() const { return *m_spSpriteShader; }
	GPUParticleShader& GetGPUParticleShader() const { return *m_spGPUParticleShader; }
	PostProcessShader& GetPostProcessShader() const { return *m_spPostProcessShader; }

private:

	std::shared_ptr<EffectShader>		m_spEffectShader;		//Effect描画シェーダ
	std::shared_ptr<ModelShader>		m_spModelShader;		//3Dモデル描画シェーダ
	std::shared_ptr<SpriteShader>		m_spSpriteShader;		//画像描画用シェーダ
	std::shared_ptr<GPUParticleShader>	m_spGPUParticleShader;	//非同期コンピュート 粒子シミュレーションシェーダー
	std::shared_ptr<PostProcessShader>	m_spPostProcessShader;	//加工シェーダー

private:

	//@brief コンストラクタ
	ShaderManager();

	//@brief デストラクタ
	~ShaderManager() = default;
};

//-----------------------------------------------------------------------------
// define: 簡単にアクセス
//-----------------------------------------------------------------------------
#define SHADER ShaderManager::GetInstance()

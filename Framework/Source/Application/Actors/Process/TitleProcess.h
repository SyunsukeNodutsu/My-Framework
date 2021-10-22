//-----------------------------------------------------------------------------
// File: TitleProcess.h
//
// タイトルプロセス管理
//-----------------------------------------------------------------------------
#pragma once

// タイトルプロセス管理クラス
class TitleProcess : public Actor
{
public:

	// @brief コンストラクタ
	TitleProcess();

	// @brief 初期化
	void Initialize() override;

	// @brief 更新
	// @param deltaTime 前フレームからの経過時間
	void Update(float deltaTime) override;

	// @brief スプライト描画
	// @param deltaTime 前フレームからの経過時間
	void DrawSprite(float deltaTime) override;

private:

	std::shared_ptr<Camera>	m_spCamera;
	std::shared_ptr<Texture> m_spTextureBackGround;

};

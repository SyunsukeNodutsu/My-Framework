//-----------------------------------------------------------------------------
// File: GameProcess.h
//
// ゲームプロセス管理
//-----------------------------------------------------------------------------
#pragma once

// ゲームプロセスの管理クラス
class GameProcess : public Actor
{
public:

	// @brief コンストラクタ
	GameProcess();

	// @brief 初期化
	void Initialize() override;

	// @brief 更新
	// @param deltaTime 前フレームからの経過時間
	void Update(float deltaTime) override;

	// @brief スプライト描画
	// @param deltaTime 前フレームからの経過時間
	void DrawSprite(float deltaTime) override;

private:

	// 一時停止中の背景画像
	std::shared_ptr<Texture> m_spPauseBackGround;

	// 一時停止？
	bool m_pause;

};

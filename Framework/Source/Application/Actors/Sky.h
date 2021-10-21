//-----------------------------------------------------------------------------
// File: Sky.h
//
// 球体の空
//-----------------------------------------------------------------------------
#pragma once
#include "Actor.h"

// スカイスフィアクラス
class Sky : public Actor
{
public:

	// @brief コンストラクタ
	Sky();

	// @brief 更新
	// @param deltaTime 前フレームからの経過時間
	void Update(float deltaTime) override;

	// @brief 描画
	// @param deltaTime 前フレームからの経過時間
	void Draw(float deltaTime) override;

private:

	float m_rotateSpeed;

};

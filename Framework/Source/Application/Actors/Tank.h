//-----------------------------------------------------------------------------
// File: Tank.h
// 
// 戦車クラス 親子構造
//-----------------------------------------------------------------------------
#pragma once
#include "Actor.h"

// 戦車クラス
class Tank : public Actor
{
public:

	// @brief インスタンス生成直後
	void Awake() override;

	// @brief 描画
	void Draw(float deltaTime) override;

private:

	Actor m_tire;

};

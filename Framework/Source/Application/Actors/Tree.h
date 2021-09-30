//-----------------------------------------------------------------------------
// File: Tree.h
//
// 木クラス
//-----------------------------------------------------------------------------
#pragma once
#include "Actor.h"

// 木クラス
class Tree : public Actor
{
public:

	// @brief コンストラクタ
	Tree();

	// @bief 初期化
	void Initialize() override;

	// @brief 描画
	// @param deltaTime 前フレームからの経過時間
	void Draw(float deltaTime) override;

private:


};

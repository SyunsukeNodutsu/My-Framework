//-----------------------------------------------------------------------------
// File: Tree.h
//
// 木クラス
//-----------------------------------------------------------------------------
#pragma once

// 木クラス
class Tree : public Actor
{
public:

	// @brief コンストラクタ
	Tree();

	// @brief 描画
	// @param deltaTime 前フレームからの経過時間
	void Draw(float deltaTime) override;

};

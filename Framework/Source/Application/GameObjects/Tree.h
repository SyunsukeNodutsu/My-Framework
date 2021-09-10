//-----------------------------------------------------------------------------
// File: Tree.h
//
// 描画の際にテクスチャ表示のために裏面カリングをOFF
// TODO: 中心から離れるほど揺らせばいい感じになるかも
//-----------------------------------------------------------------------------
#pragma once
#include "GameObject.h"

// 木クラス
class Tree : public GameObject
{
public:

	// @brief コンストラクタ
	Tree();

	// @brief 描画
	void Draw(float deltaTime) override;

};

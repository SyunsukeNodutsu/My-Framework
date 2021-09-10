//-----------------------------------------------------------------------------
// File: SailingShip.h
//
// 帆船 動力の大半が空気
// TODO: 将来的には旗を親子構造にして 専用のShaderで揺らす
//-----------------------------------------------------------------------------
#pragma once
#include "GameObject.h"

// 帆船クラス
class SailingShip : public GameObject
{
public:

	// @brief 初期化
	void Initialize()override;

	// @brief インスタンス生成直後
	void Draw(float deltaTime) override;

private:


};

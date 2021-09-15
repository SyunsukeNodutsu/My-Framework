//-----------------------------------------------------------------------------
// File: Tank.h
// 
// 戦車クラス 親子構造
//-----------------------------------------------------------------------------
#pragma once
#include "GameObject.h"

// 戦車クラス
class Tank : public GameObject
{
public:

	// @brief インスタンス生成直後
	void Awake() override;

	// @brief 描画
	void Draw(float deltaTime) override;

private:

	GameObject m_tire;

};

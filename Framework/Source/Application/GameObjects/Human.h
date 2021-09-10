//-----------------------------------------------------------------------------
// File: Human.h
//
// 人型クラス
//-----------------------------------------------------------------------------
#pragma once
#include "GameObject.h"
#include "../Camera/TPSCamera.h"

// 人型クラス
class Human : public GameObject
{
public:

	// @brief 初期化
	void Initialize()override;

	// @brief 終了
	void Finalize() override;

	// @brief 更新
	void Update(float deltaTime) override;

	// @brief 描画
	void Draw(float deltaTime) override;

private:

	std::shared_ptr<TPSCamera> m_spCamera;
	float m_zoom = 5;

};

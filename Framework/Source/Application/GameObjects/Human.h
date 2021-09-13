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

	// @brief コンストラクタ
	Human();

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
	Animator	m_animator;
	float		m_zoom;
	float3 m_rotation;

private:

	// @brief 移動更新
	void UpdateMove(float deltaTime);

	// @brief 体の回転更新
	// @param moveVec カメラを加味した移動ベクトル
	void RotateBody(const float3& moveVec);

};

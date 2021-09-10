//-----------------------------------------------------------------------------
// File: MainCamera.h
//
// デバッグ用にマウス操作のカメラ実装 名前おかしいな...
//
// FPSカメラを実行する正しい方法は、X回転とY角度を別々の変数に保持し
// 必要に応じてフレームごとにマトリックスを再作成すること
//-----------------------------------------------------------------------------
#pragma once
#include "GameObject.h"

class MainCamera : public GameObject
{
public:

	// @インスタンス生成直後
	void Awake() override;

	// @brief 更新
	void Update(float deltaTime) override;

private:

	POINT m_prevMousePos; // 前フレームのマウス座標
	mfloat4x4 m_viewMatrix;

};

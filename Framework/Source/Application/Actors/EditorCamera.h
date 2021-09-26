//-----------------------------------------------------------------------------
// File: EditorCamera.h
//
// 編集用 神視点カメラ
//-----------------------------------------------------------------------------
#pragma once
#include "Actor.h"
#include "../Camera/FPSCamera.h"

// 編集用 神視点カメラ
class EditorCamera : public Actor
{
public:

	// @brief コンストラクタ
	EditorCamera();

	// @brief 初期化
	void Initialize() override;

	// @brief 更新
	// @param deltaTime 前フレームからの経過時間
	void Update(float deltaTime) override;

private:

	std::shared_ptr<FPSCamera> m_spCamera;

};

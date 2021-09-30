//-----------------------------------------------------------------------------
// File: Camera.h
//
// 編集カメラ マウスで視点を操作
//-----------------------------------------------------------------------------
#pragma once
#include "../../Framework/Graphics/Camera/Camera.h"

// 編集カメラ
class EditorCamera : public Camera
{
public:

	// @brief コンストラクタ
	EditorCamera();

	// @brief 更新
	// @param deltaTime 前フレームからの経過時間
	void Update(float deltaTime);

private:

	// 1フレーム前のマウス座標
	float2 m_mousePosOld;

	// 現在のマウス座標
	float2 m_mousePosNow;

	float3 m_position;
	float3 m_rotation;

};

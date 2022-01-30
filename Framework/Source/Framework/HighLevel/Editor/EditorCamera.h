//-----------------------------------------------------------------------------
// File: Camera.h
//
// 編集カメラ マウスで視点を操作
//-----------------------------------------------------------------------------
#pragma once

class Transform;

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

	std::shared_ptr<Transform> m_transform;

	float3 m_position;
	float3 m_rotation;

	float2 m_mousePosOld;// 前フレーム
	float2 m_mousePosNow;

};

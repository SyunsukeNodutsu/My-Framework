#pragma once
#include "FPSCamera.h"

// 3人称視点のカメラ
class TPSCamera : public FPSCamera
{
public:

	// コンストラクタ
	TPSCamera() {}

	// デストラクタ
	~TPSCamera() {}

	// カメラ行列の設定
	void SetCameraMatrix(const mfloat4x4& mWorld) override;

	// 注視点調整用ベクトルを設定
	void SetLocalGazePosition(const float3& pos) {
		m_localGazePos = pos;
	}
	void SetLocalGazePosition(float x, float y, float z) {
		m_localGazePos = float3(x, y, z);
	}

private:

	// 注視点(カメラが見ている地点)調整用ベクトル
	float3 m_localGazePos;

};

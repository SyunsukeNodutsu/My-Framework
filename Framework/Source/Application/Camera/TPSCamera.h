//-----------------------------------------------------------------------------
// File: FPSCamera.h
//
// TPS視点(3人称視点)カメラクラス
//-----------------------------------------------------------------------------
#pragma once
#include "FPSCamera.h"

// TPS視点カメラクラス
class TPSCamera : public FPSCamera
{
public:

	// @brief コンストラクタ
	TPSCamera();

	// @brief カメラ行列の設定
	void SetCameraMatrix(const mfloat4x4& mWorld) override;

	// @brief 注視点調整用ベクトルを設定
	// @param pos 設定するベクトル
	void SetLocalGazePosition(const float3& pos) {
		m_localGazePos = pos;
	}

private:

	// 注視点(カメラが見ている地点)調整用ベクトル
	float3 m_localGazePos;

};

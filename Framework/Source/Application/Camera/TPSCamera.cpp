#include "TPSCamera.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
TPSCamera::TPSCamera()
	: m_localGazePos(float3::Zero)
{
}

//-----------------------------------------------------------------------------
// カメラ行列の設定
//-----------------------------------------------------------------------------
void TPSCamera::SetCameraMatrix(const mfloat4x4& worldMatrix)
{
	// 視線調整用の移動行列
	mfloat4x4 gazeTrans;
	gazeTrans = gazeTrans.CreateTranslation(m_localGazePos);

	mfloat4x4 localPos;
	localPos = mfloat4x4::CreateTranslation(m_localPos);

	mfloat4x4 rotation;
	rotation = GetRotationMatrix();

	Camera::SetCameraMatrix(localPos * rotation * gazeTrans * worldMatrix);
}

#include "FPSCamera.h"

const POINT FPSCamera::s_fixMousePos = { 640, 360 };

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
FPSCamera::FPSCamera()
	: m_localPos(float3::Zero)
	, m_degAngle(float3::Zero)
	, m_minAngleX(-360)
	, m_maxAngleX(360)
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void FPSCamera::Initialize()
{
	// マウスを固定位置に移動
	SetCursorPos(s_fixMousePos.x, s_fixMousePos.y);
	// マウス表示OFF
	RAW_INPUT.GetMouse()->SetCursorShow(false);
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void FPSCamera::Update()
{
	if (!m_enable)
		return;

	{
		// マウスでカメラを回転させる処理
		POINT nowPos;
		GetCursorPos(&nowPos);

		POINT mouseMove;

		mouseMove.x = nowPos.x - s_fixMousePos.x;
		mouseMove.y = nowPos.y - s_fixMousePos.y;

		SetCursorPos(s_fixMousePos.x, s_fixMousePos.y);

		// カメラを回転させる処理
		m_degAngle.x += mouseMove.y * 0.1f;
		m_degAngle.y += mouseMove.x * 0.1f;
	}

	// 回転制御
	m_degAngle.x = std::clamp(m_degAngle.x, m_minAngleX, m_maxAngleX);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
const mfloat4x4 FPSCamera::GetRotationMatrix()
{
	return mfloat4x4::CreateFromYawPitchRoll(
		m_degAngle.y * ToRadians,
		m_degAngle.x * ToRadians,
		m_degAngle.z * ToRadians
	);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
const mfloat4x4 FPSCamera::GetRotationYMatrix()
{
	return mfloat4x4::CreateRotationY(m_degAngle.y * ToRadians);
}

//-----------------------------------------------------------------------------
// FPSカメラのカメラ行列を設定
//-----------------------------------------------------------------------------
void FPSCamera::SetCameraMatrix(const mfloat4x4& cameraMat)
{
	mfloat4x4 trans;
	trans = trans.CreateTranslation(m_localPos);

	mfloat4x4 rotation;
	rotation = GetRotationMatrix();

	Camera::SetCameraMatrix(rotation * trans * cameraMat);
}

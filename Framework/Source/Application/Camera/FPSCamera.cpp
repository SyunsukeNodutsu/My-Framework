#include "FPSCamera.h"
#include "../main.h"

const float2 FPSCamera::s_fixMousePos = float2(769, 432);

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
	// マウス表示OFF
	APP.g_rawInputDevice->g_spMouse->SetCursorShow(false);
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void FPSCamera::Update()
{
	if (!g_enable) return;

	const auto& mouse = APP.g_rawInputDevice->g_spMouse;

	const auto& nowPos = mouse->GetMousePos();
	const auto& deltaTime = static_cast<float>(APP.g_fpsTimer->GetDeltaTime());

	float2 mouseMove = float2::Zero;
	mouseMove.x = nowPos.x - s_fixMousePos.x;
	mouseMove.y = nowPos.y - s_fixMousePos.y;

	mouse->SetAt(s_fixMousePos, APP.g_window->GetWndHandle());

	// カメラを回転
	m_degAngle.x += mouseMove.y * 0.08f;
	m_degAngle.y += mouseMove.x * 0.1f;

	// 回転制御
	m_degAngle.x = std::clamp(m_degAngle.x, m_minAngleX, m_maxAngleX);
}

//-----------------------------------------------------------------------------
// 回転行列を返す
//-----------------------------------------------------------------------------
const mfloat4x4 FPSCamera::GetRotationMatrix()
{
	// ヨー ピッチ およびロールから作成
	return mfloat4x4::CreateFromYawPitchRoll(
		m_degAngle.y * ToRadians,
		m_degAngle.x * ToRadians,
		m_degAngle.z * ToRadians
	);
}

//-----------------------------------------------------------------------------
// Y軸の回転行列を返す
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

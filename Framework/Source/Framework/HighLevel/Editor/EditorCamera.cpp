#include "EditorCamera.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
EditorCamera::EditorCamera()
	: m_transform()
	, m_position(float3::Zero)
	, m_rotation(float3::Zero)
	, m_mousePosOld(float2::Zero)
	, m_mousePosNow(float2::Zero)
{
	g_priority = FLT_MIN;
	m_position = GetCameraMatrix().Translation();
	m_transform = std::make_shared<Transform>();
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void EditorCamera::Update(float deltaTime)
{
	const auto& rawDeltaTime = static_cast<float>(ApplicationChilled::GetApplication()->g_fpsTimer->GetDeltaTime(true));

	m_mousePosNow = ApplicationChilled::GetApplication()->g_inputDevice->GetMousePos();

	// 回転
	{
		float raito = 12.0f;
		if (ApplicationChilled::GetApplication()->g_inputDevice->IsMouseDown(MouseButton::Right))
		{
			float deltaX = m_mousePosNow.x - m_mousePosOld.x;
			float deltaY = m_mousePosNow.y - m_mousePosOld.y;

			m_rotation.x += deltaY * raito * rawDeltaTime;
			m_rotation.y += deltaX * raito * rawDeltaTime;

			m_transform->SetAngle(m_rotation);
		}
	}

	// 移動
	{
		float raito = 6.0f;
		float3 axisZ = m_transform->GetWorldMatrix().Backward(); axisZ.Normalize();
		float3 axisX = m_transform->GetWorldMatrix().Right(); axisX.Normalize();

		if (ApplicationChilled::GetApplication()->g_inputDevice->IsKeyDown(KeyCode::Shift)) raito = 20.0f;
		if (ApplicationChilled::GetApplication()->g_inputDevice->IsKeyDown(KeyCode::Control)) raito = 2.0f;

		if (ApplicationChilled::GetApplication()->g_inputDevice->IsKeyDown(KeyCode::W)) m_position += axisZ * raito * rawDeltaTime;
		if (ApplicationChilled::GetApplication()->g_inputDevice->IsKeyDown(KeyCode::S)) m_position -= axisZ * raito * rawDeltaTime;
		if (ApplicationChilled::GetApplication()->g_inputDevice->IsKeyDown(KeyCode::A)) m_position -= axisX * raito * rawDeltaTime;
		if (ApplicationChilled::GetApplication()->g_inputDevice->IsKeyDown(KeyCode::D)) m_position += axisX * raito * rawDeltaTime;

		if (ApplicationChilled::GetApplication()->g_inputDevice->IsKeyDown(KeyCode::C)) m_position -= float3(0, 1, 0) * raito * rawDeltaTime;
		if (ApplicationChilled::GetApplication()->g_inputDevice->IsKeyDown(KeyCode::V)) m_position += float3(0, 1, 0) * raito * rawDeltaTime;
		m_transform->SetPosition(m_position);
	}

	SetCameraMatrix(m_transform->GetWorldMatrix());

	// 前フレームのマウス座標更新
	m_mousePosOld = m_mousePosNow;
}

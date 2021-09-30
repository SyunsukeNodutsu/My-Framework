#include "EditorCamera.h"
#include "../Actors/Actor.h"
#include "../main.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
EditorCamera::EditorCamera()
	: m_mousePosOld(float2::Zero)
	, m_mousePosNow(float2::Zero)
{
	g_priority = -999;

	m_position = GetCameraMatrix().Translation();
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void EditorCamera::Update(float deltaTime)
{
	auto& mouse = APP.g_rawInputDevice->g_spMouse;
	auto& keyboard = APP.g_rawInputDevice->g_spKeyboard;
	static Transform trans;

	m_mousePosNow = mouse->GetMousePos(true);

	// 回転
	{
		float raito = 200.0f;
		if (mouse->IsDown(MouseButton::Right))
		{
			float deltaX = m_mousePosNow.x - m_mousePosOld.x;
			float deltaY = m_mousePosNow.y - m_mousePosOld.y;

			m_rotation.x += deltaY * raito * deltaTime;
			m_rotation.y += deltaX * raito * deltaTime;

			trans.SetAngle(m_rotation);
		}
	}

	// 移動
	{
		float raito = 6.0f;
		float3 axisZ = trans.GetWorldMatrix().Backward(); axisZ.Normalize();
		float3 axisX = trans.GetWorldMatrix().Right(); axisX.Normalize();

		if (keyboard->IsDown(KeyCode::Shift)) raito = 20.0f;
		if (keyboard->IsDown(KeyCode::Control)) raito = 2.0f;

		if (keyboard->IsDown(KeyCode::W)) m_position += axisZ * raito * deltaTime;
		if (keyboard->IsDown(KeyCode::S)) m_position -= axisZ * raito * deltaTime;
		if (keyboard->IsDown(KeyCode::A)) m_position -= axisX * raito * deltaTime;
		if (keyboard->IsDown(KeyCode::D)) m_position += axisX * raito * deltaTime;
		trans.SetPosition(m_position);
	}

	SetCameraMatrix(trans.GetWorldMatrix());

	// 前フレームのマウス座標更新
	m_mousePosOld = m_mousePosNow;
}

#include "EditorCamera.h"
#include "../main.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
EditorCamera::EditorCamera()
	: m_mousePosOld(float2::Zero)
	, m_mousePosNow(float2::Zero)
{
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void EditorCamera::Update(float deltaTime)
{
	auto& mouse = APP.g_rawInputDevice->g_spMouse;

	m_mousePosNow = mouse->GetMousePos();

	{



	}

	m_mousePosOld = m_mousePosNow;
}

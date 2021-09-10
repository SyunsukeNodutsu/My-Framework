#include "MainCamera.h"

//-----------------------------------------------------------------------------
// インスタンス生成直後
//-----------------------------------------------------------------------------
void MainCamera::Awake()
{
	m_viewMatrix.CreateTranslation(float3(-124, 41, 123));
	D3D.GetRenderer().SetViewMatrix(m_viewMatrix);
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void MainCamera::Update(float deltaTime)
{
	// 平行移動
	{
		float3 moveVec;
		if (RAW_INPUT.GetKeyboard()->IsDown(KeyCode::W)) moveVec.z += 1;
		if (RAW_INPUT.GetKeyboard()->IsDown(KeyCode::S)) moveVec.z -= 1;
		if (RAW_INPUT.GetKeyboard()->IsDown(KeyCode::A)) moveVec.x -= 1;
		if (RAW_INPUT.GetKeyboard()->IsDown(KeyCode::D)) moveVec.x += 1;

		if (RAW_INPUT.GetKeyboard()->IsDown(KeyCode::Space)) { moveVec.y += 1; }
		if (RAW_INPUT.GetKeyboard()->IsDown(KeyCode::C)) { moveVec.y -= 1; }
		moveVec.Normalize();

		float movePow = 10.0f;
		if (RAW_INPUT.GetKeyboard()->IsDown(KeyCode::Shift)) { movePow *= 2; }
		if (RAW_INPUT.GetKeyboard()->IsDown(KeyCode::Control)) { movePow *= 0.5f; }
		m_viewMatrix *= mfloat4x4::CreateTranslation(moveVec * movePow * deltaTime * -1);
	}

	// 回転

	static bool bStop = false;
	float rotPow = 0.06f;

	if (!bStop)
	{
		// 現在のマウス座標所得
		POINT mousePos = {};
		GetCursorPos(&mousePos);

		if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
		{
			// マウスの前回からの移動量
			float deltaX = static_cast<float>(mousePos.x - m_prevMousePos.x);
			float deltaY = static_cast<float>(mousePos.y - m_prevMousePos.y);

			// その場でXY回転 TODO: 調査.なんかこれで動いたぁ^^
			m_viewMatrix *= mfloat4x4::CreateFromAxisAngle(m_viewMatrix.Up(), deltaX * rotPow * ToRadians * -1);
			m_viewMatrix *= mfloat4x4::CreateFromAxisAngle(float3(1, 0, 0), deltaY * rotPow * ToRadians * -1);
		}

		// 前フレームのマウス座標
		m_prevMousePos = mousePos;
	}

	if (bStop)
	{
		// 停止させる座標
		RECT rect = {};
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
		const int stopX = (rect.right - rect.left) / 2;
		const int stopY = (rect.bottom - rect.top) / 2;

		// 現在の座標所得
		POINT mousePos = {};
		GetCursorPos(&mousePos);

		// 画面外に出ないように固定
		SetCursorPos(stopX, stopY);

		// スティック操作
		float deltaX = static_cast<float>(mousePos.x - m_prevMousePos.x);
		float deltaY = static_cast<float>(mousePos.y - m_prevMousePos.y);

		// 今フレームの座標記憶
		m_prevMousePos = POINT{ stopX, stopY };

		m_viewMatrix *= mfloat4x4::CreateFromAxisAngle(m_viewMatrix.Up(), deltaX * rotPow * ToRadians * -1);
		m_viewMatrix *= mfloat4x4::CreateFromAxisAngle(float3(1, 0, 0), deltaY * rotPow * ToRadians * -1);
	}

	D3D.GetRenderer().SetViewMatrix(m_viewMatrix);

	// Escキーでカーソル操作切り替え
	if (RAW_INPUT.GetKeyboard()->IsPressed(KeyCode::Escape))
	{
		RAW_INPUT.GetMouse()->SetCursorShow(bStop);
		bStop = !bStop;
	}
}

#include "EditorCamera.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
EditorCamera::EditorCamera()
	: m_spCamera(nullptr)
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void EditorCamera::Initialize()
{
	m_transform.SetPosition(float3(0, 10, 0));

	m_spCamera = std::make_shared<FPSCamera>();

	m_spCamera->g_priority = 0.0f;
	m_spCamera->g_name = "EditorFPS";
	APP.g_gameSystem->g_cameraSystem.SetCameraList(m_spCamera);
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void EditorCamera::Update(float deltaTime)
{
	if (m_spCamera == nullptr) return;
	if (!m_spCamera->g_enable) return;

	// 入力ベクトル取得
	float3 moveVec;
	if (APP.g_rawInputDevice->g_spKeyboard->IsDown(KeyCode::W)) moveVec.z += 1.0f;
	if (APP.g_rawInputDevice->g_spKeyboard->IsDown(KeyCode::S)) moveVec.z -= 1.0f;
	if (APP.g_rawInputDevice->g_spKeyboard->IsDown(KeyCode::A)) moveVec.x -= 1.0f;
	if (APP.g_rawInputDevice->g_spKeyboard->IsDown(KeyCode::D)) moveVec.x += 1.0f;
	moveVec.Normalize();

	// カメラを加味
	moveVec = moveVec.TransformNormal(moveVec, m_spCamera->GetRotationYMatrix());

	constexpr float speed = 10.0f;
	moveVec *= speed * deltaTime;

	auto pos = m_transform.GetPosition() + moveVec;
	m_transform.SetPosition(pos);

	//
	mfloat4x4 trans = mfloat4x4::CreateTranslation(m_transform.GetPosition());
	m_spCamera->Update();
	m_spCamera->SetCameraMatrix(trans);
}

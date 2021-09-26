#include "Tank.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Tank::Tank()
	: m_spCamera(nullptr)
{
}

//-----------------------------------------------------------------------------
// インスタンス生成直後
//-----------------------------------------------------------------------------
void Tank::Awake()
{
	m_name = "T43";

	LoadModel("Resource/Model/T43/T43_Full.gltf");
	m_transform.SetPosition(float3(8, 0, 0));

	m_tire.LoadModel("Resource/Model/T43/T43_Tire.gltf");
	m_tireOffset = mfloat4x4::CreateTranslation(float3(1, 0, 0));

	// カメラ作成
	m_spCamera = std::make_shared<TPSCamera>();
	if (m_spCamera)
	{
		m_spCamera->SetClampAngleX(-75.0f, 90.0f);
		m_spCamera->SetAngle(0, 0);

		m_spCamera->SetLocalPos(float3(0.0f, 0.0f, -10));
		m_spCamera->SetLocalGazePosition(float3(0.0f, 2.4f, 0.0f));

		m_spCamera->g_name = "TankTPS";
		m_spCamera->g_priority = 10.0f;
		APP.g_gameSystem->g_cameraSystem.SetCameraList(m_spCamera);
	}
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void Tank::Update(float deltaTime)
{
	// Escでカメラ操作切り替え
	static bool enable = true;
	if (APP.g_rawInputDevice->g_spKeyboard->IsPressed(KeyCode::Escape))
	{
		enable = !enable;
		m_spCamera->SetEnable(enable);
	}

	if (!enable) return;

	UpdateMove(deltaTime);
	UpdateRotate(deltaTime);

	if (APP.g_rawInputDevice->g_spMouse->IsPressed(MouseButton::Left))
	{
		float3 axisZ = m_transform.GetWorldMatrix().Backward();
		float3 pos = m_transform.GetPosition() + float3(0, 2, 0) + axisZ * 6;
		APP.g_effectDevice->Play(u"Resource/Effect/TankFire.efk", pos);
	}

	m_tire.GetTransform().SetWorldMatrix(m_tireOffset * m_transform.GetWorldMatrix());

	if (m_spCamera)
	{
		mfloat4x4 trans = mfloat4x4::CreateTranslation(m_transform.GetPosition());

		m_spCamera->SetLocalPos(float3(0.0f, 0.0f, -10));
		m_spCamera->Update();
		m_spCamera->SetCameraMatrix(trans);
	}
}

//-----------------------------------------------------------------------------
// 描画
//-----------------------------------------------------------------------------
void Tank::Draw(float deltaTime)
{
	Actor::Draw(deltaTime);

	m_tire.Draw(deltaTime);
}

//-----------------------------------------------------------------------------
// 移動更新
//-----------------------------------------------------------------------------
void Tank::UpdateMove(float deltaTime)
{
	float speed = 4.0f;

	float3 axisZ = float3::Zero;
	if (APP.g_rawInputDevice->g_spKeyboard->IsDown(KeyCode::W)) axisZ = m_transform.GetWorldMatrix().Backward();
	if (APP.g_rawInputDevice->g_spKeyboard->IsDown(KeyCode::S)) axisZ = m_transform.GetWorldMatrix().Forward();

	axisZ.Normalize();
	axisZ *= speed * deltaTime;

	// 座標の更新 TransformのZ軸に前進
	auto pos = m_transform.GetPosition() + axisZ;
	m_transform.SetPosition(pos);
}

//-----------------------------------------------------------------------------
// 回転更新
//-----------------------------------------------------------------------------
void Tank::UpdateRotate(float deltaTime)
{
	float rotY = 0;
	if (APP.g_rawInputDevice->g_spKeyboard->IsDown(KeyCode::A)) rotY -= 1.0f;
	if (APP.g_rawInputDevice->g_spKeyboard->IsDown(KeyCode::D)) rotY += 1.0f;
	
	constexpr float rotPow = 20.0f;

	auto angle = m_transform.GetAngle();
	angle.y += rotY * rotPow * deltaTime;
	m_transform.SetAngle(angle);
}

// State待機 更新
void Tank::StateWait::Update(float deltaTime, Tank& owner)
{

}

// State移動 更新
void Tank::StateMove::Update(float deltaTime, Tank& owner)
{

}

﻿#include "Tank.h"
#include "TankParts.h"

#include "../../Framework/Audio/SoundDirector.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Tank::Tank()
	: m_spCamera(nullptr)
	, m_spTankParts(nullptr)
	, m_moveSpeed(0.0f)
	, m_rotateSpeed(0.0f)
{
}

//-----------------------------------------------------------------------------
// インスタンス生成直後
//-----------------------------------------------------------------------------
void Tank::Awake()
{
	m_name = "T43";

	LoadModel("Resource/Model/T43/T43_Body.gltf");
	
	m_spTankParts = std::make_shared<TankParts>(*this);
	m_spTankParts->Initialize();

	// 初期化後に浮かせる ※TODO: モデルのY座標修正
	m_transform.SetPosition(float3(0, 1.2f, 0));

	// カメラ作成
	m_spCamera = std::make_shared<TPSCamera>();
	if (m_spCamera)
	{
		m_spCamera->Initialize();

		m_spCamera->SetClampAngleX(-75.0f, 90.0f);
		m_spCamera->SetAngle(0, 0);

		m_spCamera->SetLocalPos(float3(0.0f, 0.0f, -10));
		m_spCamera->SetLocalGazePosition(float3(0.0f, 2.4f, 0.0f));

		m_spCamera->g_name = "TankTPS";
		m_spCamera->g_priority = 10.0f;
		m_spCamera->SetEnable(true);
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
		float3 pos = m_transform.GetPosition() + float3(0.2f, 1.0f, 0.0f) + axisZ * 6.6f;
		APP.g_effectDevice->Play(u"Resource/Effect/TankFire.efk", pos);
		SOUND_DIRECTOR.Play3D("Resource/Audio/SE/Cannon01.wav", pos);
	}

	m_spTankParts->Update(deltaTime, m_moveSpeed, m_rotateSpeed);

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

	m_spTankParts->Draw(deltaTime);
}

//-----------------------------------------------------------------------------
// 移動更新
//-----------------------------------------------------------------------------
void Tank::UpdateMove(float deltaTime)
{
	// 外部ファイル定義のほうがええ
	constexpr float advanceSpeed = 2.0f;// 前進速度
	constexpr float backwardSpeed = 1.2f;// 後進速度
	constexpr float decelerateSpeed = 4.0f;// 減速速度(操作無しの時)

	constexpr float ADVANCE_SPEED_MAX = 6.0f;// 最大前進速度
	constexpr float BACKWARD_SPEED_MAX = 4.0f;// 最大後進速度
	constexpr float DECELERATE_SPEED_MAX = 6.0f;// 最大減速速度

	// 前進
	if (APP.g_rawInputDevice->g_spKeyboard->IsDown(KeyCode::W)) {
		m_moveSpeed += advanceSpeed * deltaTime;
		m_moveSpeed = std::clamp(m_moveSpeed, -BACKWARD_SPEED_MAX, ADVANCE_SPEED_MAX);
	}
	else if (m_moveSpeed > 0) {
		m_moveSpeed -= decelerateSpeed * deltaTime;
		m_moveSpeed = std::clamp(m_moveSpeed, 0.0f, DECELERATE_SPEED_MAX);
	}

	// 後進
	if (APP.g_rawInputDevice->g_spKeyboard->IsDown(KeyCode::S)) {
		m_moveSpeed -= backwardSpeed * deltaTime;
		m_moveSpeed = std::clamp(m_moveSpeed, -BACKWARD_SPEED_MAX, ADVANCE_SPEED_MAX);
	}
	else if (m_moveSpeed < 0) {
		m_moveSpeed += decelerateSpeed * deltaTime;
		m_moveSpeed = std::clamp(m_moveSpeed, -DECELERATE_SPEED_MAX, 0.0f);
	}

	// Z軸に移動
	float3 axisZ = m_transform.GetWorldMatrix().Backward();
	axisZ.Normalize();
	axisZ *= m_moveSpeed * deltaTime;

	float3 pos = m_transform.GetPosition() + axisZ;
	m_transform.SetPosition(pos);
}

//-----------------------------------------------------------------------------
// 回転更新
//-----------------------------------------------------------------------------
void Tank::UpdateRotate(float deltaTime)
{
	constexpr float rotSpeed = 20.0f;// 回転速度
	constexpr float decelerateSpeed = 28.0f;// 減速速度(操作無しの時)

	constexpr float ROTATE_SPEED_MAX = 20.0f;// 最大回転速度

	// 左回転
	if (APP.g_rawInputDevice->g_spKeyboard->IsDown(KeyCode::A)) {
		m_rotateSpeed -= rotSpeed * deltaTime;
		m_rotateSpeed = std::clamp(m_rotateSpeed, -ROTATE_SPEED_MAX, ROTATE_SPEED_MAX);
	}
	else if (m_rotateSpeed < 0) {
		m_rotateSpeed += decelerateSpeed * deltaTime;
		m_rotateSpeed = std::clamp(m_rotateSpeed, -ROTATE_SPEED_MAX, 0.0f);
	}

	// 右回転
	if (APP.g_rawInputDevice->g_spKeyboard->IsDown(KeyCode::D)) {
		m_rotateSpeed += rotSpeed * deltaTime;
		m_rotateSpeed = std::clamp(m_rotateSpeed, -ROTATE_SPEED_MAX, ROTATE_SPEED_MAX);
	}
	else if (m_rotateSpeed > 0) {
		m_rotateSpeed -= decelerateSpeed * deltaTime;
		m_rotateSpeed = std::clamp(m_rotateSpeed, 0.0f, ROTATE_SPEED_MAX);
	}

	auto angle = m_transform.GetAngle();
	angle.y += m_rotateSpeed * deltaTime;
	m_transform.SetAngle(angle);
}

// State待機 更新
void Tank::State3rd::Update(float deltaTime, Tank& owner)
{

}

// State移動 更新
void Tank::StateAim::Update(float deltaTime, Tank& owner)
{

}

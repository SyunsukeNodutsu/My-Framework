﻿#include "Tank.h"
#include "TankParts.h"

#include "TankBullet.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Tank::Tank()
	: m_spCamera3rd(nullptr)
	, m_spCamera1st(nullptr)
	, m_spTankParts(nullptr)
	, m_runSound3D(nullptr)
	, m_shotSound3D(nullptr)
	, m_moveSpeed(0.0f)
	, m_rotateSpeed(0.0f)
	, m_cameraAngle(float2::Zero)
{
}

//-----------------------------------------------------------------------------
// インスタンス生成直後
//-----------------------------------------------------------------------------
void Tank::Awake()
{	
	m_spState = std::make_shared<State3rd>();

	m_spTankParts = std::make_shared<TankParts>(*this);
	m_spTankParts->Initialize();

	// BGM再生
	// 音量操作や停止、フィルターなどをかける場合は インスタンスを取得
	m_runSound3D = SOUND_DIRECTOR.CreateSoundWork3D("Resource/Audio/SE/TankRun.wav", true, true);
	if (m_runSound3D) {
		m_runSound3D->Play3D(m_transform.GetPosition());
		m_runSound3D->SetVolume(0.0f);
	}

	//--------------------------------------------------
	// カメラ作成
	//--------------------------------------------------
	m_spCamera3rd = std::make_shared<TPSCamera>();
	if (m_spCamera3rd)
	{
		m_spCamera3rd->Initialize();

		m_spCamera3rd->SetClampAngleX(-15.0f, 15.0f);

		m_spCamera3rd->SetLocalPos(float3(0.0f, 0.0f, -10));
		m_spCamera3rd->SetLocalGazePosition(float3(0.0f, 2.4f, 0.0f));

		m_spCamera3rd->g_name = "TankTPS";
		m_spCamera3rd->g_priority = 1.0f;

		APP.g_gameSystem->g_cameraSystem.AddCameraList(m_spCamera3rd);
	}

	m_spCamera1st = std::make_shared<FPSCamera>();
	if (m_spCamera1st)
	{
		m_spCamera1st->SetClampAngleX(-15.0f, 15.0f);

		m_spCamera1st->g_name = "TankFPS";
		m_spCamera1st->g_priority = 0.0f;

		APP.g_gameSystem->g_cameraSystem.AddCameraList(m_spCamera1st);
	}
}

//-----------------------------------------------------------------------------
// jsonファイルの逆シリアル
//-----------------------------------------------------------------------------
void Tank::Deserialize(const json11::Json& jsonObject)
{
	Actor::Deserialize(jsonObject);

	//m_spTankParts->Deserialize(jsonObject);
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void Tank::Update(float deltaTime)
{
	if (!APP.g_gameSystem->g_cameraSystem.IsEditorMode())
	{
		// 現在のステート更新
		m_spState->Update(*this, deltaTime);

		// 部品更新
		m_spTankParts->Update(deltaTime, m_moveSpeed, m_rotateSpeed);
	}
	else
	{
		// 部品更新
		// 編集カメラの際は UVスクロールをOFF
		m_spTankParts->Update(deltaTime, 0, 0);
	}

	if (APP.g_rawInputDevice->g_spKeyboard->IsPressed(KeyCode::Back))
		APP.g_gameSystem->RequestChangeScene("Resource/Jsons/test.json");
}

//-----------------------------------------------------------------------------
// 描画
//-----------------------------------------------------------------------------
void Tank::Draw(float deltaTime)
{
	Actor::Draw(deltaTime);

	// 現在のステート描画
	m_spState->Draw(*this, deltaTime);
}

//-----------------------------------------------------------------------------
// スプライト描画
//-----------------------------------------------------------------------------
void Tank::DrawSprite(float deltaTime)
{
	if (APP.g_gameSystem->g_cameraSystem.IsEditorMode()) return;

	m_spState->DrawSprite(*this, deltaTime);
}

//-----------------------------------------------------------------------------
// 移動更新
//-----------------------------------------------------------------------------
void Tank::UpdateMove(float deltaTime)
{
	// TODO: 外部ファイル定義のほうがええ
	constexpr float forwardSpeed = 2.0f;// 前進速度
	constexpr float backwardSpeed = 1.2f;// 後進速度
	constexpr float decelerateSpeed = 4.0f;// 減速速度(操作無しの時)

	constexpr float FORWARD_SPEED_MAX = 6.0f;// 最大前進速度
	constexpr float BACKWARD_SPEED_MAX = 4.0f;// 最大後進速度
	constexpr float DECELERATE_SPEED_MAX = 6.0f;// 最大減速速度

	// 前進
	if (APP.g_rawInputDevice->g_spKeyboard->IsDown(KeyCode::W)) {
		m_moveSpeed += forwardSpeed * deltaTime;
		m_moveSpeed = std::clamp(m_moveSpeed, -BACKWARD_SPEED_MAX, FORWARD_SPEED_MAX);
	}
	else if (m_moveSpeed > 0) {
		m_moveSpeed -= decelerateSpeed * deltaTime;
		m_moveSpeed = std::clamp(m_moveSpeed, 0.0f, DECELERATE_SPEED_MAX);
	}

	// 後進
	if (APP.g_rawInputDevice->g_spKeyboard->IsDown(KeyCode::S)) {
		m_moveSpeed -= backwardSpeed * deltaTime;
		m_moveSpeed = std::clamp(m_moveSpeed, -BACKWARD_SPEED_MAX, FORWARD_SPEED_MAX);
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

//-----------------------------------------------------------------------------
// 走行の音更新
//-----------------------------------------------------------------------------
void Tank::UpdateRunSound(bool state1st)
{
	if (!m_runSound3D) return;

	auto moveSpeed = std::abs(m_moveSpeed);
	auto rotSpeed = std::abs(m_rotateSpeed) * 0.36f;
	auto volume = std::max(moveSpeed, rotSpeed);
	volume *= 0.08f;
	volume = std::clamp(volume, 0.02f, 0.6f);
	m_runSound3D->SetVolume(volume * (state1st ? 1.0f : 0.5f));
	m_runSound3D->SetPosition(m_transform.GetPosition());
}

//-----------------------------------------------------------------------------
// 射撃更新
//-----------------------------------------------------------------------------
void Tank::UpdateShot(bool state1st)
{
	if (APP.g_rawInputDevice->g_spMouse->IsPressed(MouseButton::Left))
	{
		const auto& mMain = m_spTankParts->GetMainGunMatrix();

		float3 axisZ = mMain.Backward();
		float3 axisX = mMain.Right();
		float3 pos = mMain.Translation() + (axisX * 0.1f) + (axisZ * 6.6f);

		const auto& cannon = std::make_shared<TankBullet>(*this, pos + axisZ, axisZ);
		APP.g_gameSystem->AddActor(cannon);

		// Effect
		APP.g_effectDevice->Play(u"Resource/Effect/TankFire.efk", pos);
		// Sound
		m_shotSound3D = SOUND_DIRECTOR.CreateSoundWork3D("Resource/Audio/SE/TankShot01.wav", false, true);
		m_shotSound3D->SetVolume(state1st ? 4.0f : 2.0f);
		m_shotSound3D->Play3D(pos);
		// 1人称の際は室内を表現
		if (state1st) m_shotSound3D->SetFilter(XAUDIO2_FILTER_TYPE::LowPassFilter, 0.08f);
	}
}



//=============================================================================
// 
// State machine
// 
//=============================================================================

//-----------------------------------------------------------------------------
// 三人称State: 更新
//-----------------------------------------------------------------------------
void Tank::State3rd::Update(Tank& owner, float deltaTime)
{
	owner.UpdateMove(deltaTime);
	owner.UpdateRotate(deltaTime);

	owner.UpdateRunSound(false);

	owner.UpdateShot(false);

	if (owner.m_spCamera3rd) {
		mfloat4x4 trans = mfloat4x4::CreateTranslation(owner.m_spTankParts->GetTurretMatrix().Translation());
		owner.m_spCamera3rd->SetLocalPos(float3(0.0f, 0.0f, -10));
		owner.m_spCamera3rd->Update();
		owner.m_spCamera3rd->SetCameraMatrix(trans);

		// カメラ角度
		auto& camera = owner.m_spCamera3rd->GetCameraMatrix();
		owner.m_cameraAngle.x = atan2(camera.Backward().y, camera.Backward().z) * ToDegrees;
		owner.m_cameraAngle.y = atan2(camera.Backward().x, camera.Backward().z) * ToDegrees;

		//APP.g_imGuiSystem->ClearLog();
		//APP.g_imGuiSystem->AddLog(std::string(u8"角度X: " + std::to_string(owner.m_cameraAngle.x)).c_str());
		//APP.g_imGuiSystem->AddLog(std::string(u8"角度Y: " + std::to_string(owner.m_cameraAngle.y)).c_str());
	}

	// 遷移
	if (APP.g_rawInputDevice->g_spMouse->IsPressed(MouseButton::Right)) {
		owner.m_spState = std::make_shared<State1st>();
		// 室内を表現
		if (owner.m_runSound3D)
			owner.m_runSound3D->SetFilter(XAUDIO2_FILTER_TYPE::LowPassFilter, 0.2f);
		if (owner.m_shotSound3D)
			owner.m_shotSound3D->SetFilter(XAUDIO2_FILTER_TYPE::LowPassFilter, 0.6f);

		owner.m_spCamera3rd->g_priority = 0;
		owner.m_spCamera1st->g_priority = 1;

		auto& angle = owner.m_spCamera3rd->GetRotationAngles();
		owner.m_spCamera1st->SetAngle(float2(angle.x, angle.y));

		owner.m_modelWork.SetEnable(false);
	}
}

//-----------------------------------------------------------------------------
// 三人称State: 描画
//-----------------------------------------------------------------------------
void Tank::State3rd::Draw(Tank& owner, float deltaTime)
{
	owner.m_spTankParts->Draw(deltaTime);
}

//-----------------------------------------------------------------------------
// 三人称State: スプライト描画
//-----------------------------------------------------------------------------
void Tank::State3rd::DrawSprite(Tank& owner, float deltaTime)
{
	static std::shared_ptr<Texture> texture = nullptr;
	if (!texture) {
		texture = std::make_shared<Texture>();
		texture->Create("Resource/Texture/Aim_3rd_01.png");
	}

	SHADER.GetSpriteShader().DrawTexture(texture.get(), float2::Zero);
}

//-----------------------------------------------------------------------------
// 一人称State: 更新
//-----------------------------------------------------------------------------
void Tank::State1st::Update(Tank& owner, float deltaTime)
{
	owner.UpdateMove(deltaTime);
	owner.UpdateRotate(deltaTime);

	owner.UpdateRunSound(true);

	owner.UpdateShot(true);

	if (owner.m_spCamera1st) {
		mfloat4x4 trans = mfloat4x4::CreateTranslation(owner.m_spTankParts->GetTurretMatrix().Translation());
		float3 axisZ = owner.m_transform.GetWorldMatrix().Backward();
		float3 pos = float3(0.0f, 0.4f, 0.0f);// +axisZ;
		owner.m_spCamera1st->SetLocalPos(pos);
		owner.m_spCamera1st->Update();
		owner.m_spCamera1st->SetCameraMatrix(trans);

		// カメラ角度
		auto& camera = owner.m_spCamera1st->GetCameraMatrix();
		owner.m_cameraAngle.x = atan2(camera.Backward().y, camera.Backward().z) * ToDegrees;
		owner.m_cameraAngle.y = atan2(camera.Backward().x, camera.Backward().z) * ToDegrees;
	}

	// 遷移
	if (APP.g_rawInputDevice->g_spMouse->IsReleased(MouseButton::Right)) {
		owner.m_spState = std::make_shared<State3rd>();
		// フィルターをもとに戻す
		if (owner.m_runSound3D)
			owner.m_runSound3D->SetFilter(XAUDIO2_FILTER_TYPE::LowPassFilter, 1, 1);
		if (owner.m_shotSound3D)
			owner.m_shotSound3D->SetFilter(XAUDIO2_FILTER_TYPE::LowPassFilter, 1, 1);

		owner.m_spCamera3rd->g_priority = 1;
		owner.m_spCamera1st->g_priority = 0;

		auto& angle = owner.m_spCamera1st->GetRotationAngles();
		owner.m_spCamera3rd->SetAngle(float2(angle.x, angle.y));

		owner.m_modelWork.SetEnable(true);
	}
}

//-----------------------------------------------------------------------------
// 一人称State: スプライト描画
//-----------------------------------------------------------------------------
void Tank::State1st::DrawSprite(Tank& owner, float deltaTime)
{
	static std::shared_ptr<Texture> texture = nullptr;
	static std::shared_ptr<Texture> texture2 = nullptr;
	static std::shared_ptr<Texture> texture3 = nullptr;
	if (!texture) {
		texture = std::make_shared<Texture>();
		texture->Create("Resource/Texture/Aim_1st_01.png");
	}
	if (!texture2) {
		texture2 = std::make_shared<Texture>();
		texture2->Create("Resource/Texture/Aim_1st_02.png");
	}
	if (!texture3) {
		texture3 = std::make_shared<Texture>();
		texture3->Create("Resource/Texture/Aim_1st_03.png");
	}

	SHADER.GetSpriteShader().DrawTexture(texture.get(), float2::Zero);
	SHADER.GetSpriteShader().DrawTexture(texture2.get(), float2::Zero);
	SHADER.GetSpriteShader().DrawTexture(texture3.get(), float2(0, -100));
}
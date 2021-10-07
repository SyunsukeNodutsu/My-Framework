#include "Tank.h"
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
	, m_cameraAngleY(0.0f)
{
}

//-----------------------------------------------------------------------------
// インスタンス生成直後
//-----------------------------------------------------------------------------
void Tank::Awake()
{
	m_name = "T43";

	LoadModel("Resource/Model/T43/T43_Body.gltf");
	
	m_spState = std::make_shared<State3rd>();

	m_spTankParts = std::make_shared<TankParts>(*this);
	m_spTankParts->Initialize();

	// 初期化後に浮かせる ※TODO: モデルのY座標修正
	m_transform.SetPosition(float3(0, 1.2f, 0));

	// BGM再生
	// 音量操作や停止、フィルターなどをかける場合は インスタンスを取得
	m_runSound3D = SOUND_DIRECTOR.CreateSoundWork3D("Resource/Audio/SE/TankRun.wav", true, true);
	if (m_runSound3D)
	{
		m_runSound3D->Play3D(m_transform.GetPosition());
		m_runSound3D->SetVolume(0.0f);
	}

	// カメラ作成
	m_spCamera3rd = std::make_shared<TPSCamera>();
	if (m_spCamera3rd)
	{
		m_spCamera3rd->Initialize();

		m_spCamera3rd->SetClampAngleX(-75.0f, 75.0f);

		m_spCamera3rd->SetLocalPos(float3(0.0f, 0.0f, -10));
		m_spCamera3rd->SetLocalGazePosition(float3(0.0f, 2.4f, 0.0f));

		m_spCamera3rd->g_name = "TankTPS";
		m_spCamera3rd->g_priority = 1.0f;

		APP.g_gameSystem->g_cameraSystem.AddCameraList(m_spCamera3rd);
	}

	m_spCamera1st = std::make_shared<FPSCamera>();
	if (m_spCamera1st)
	{
		m_spCamera1st->SetClampAngleX(-75.0f, 75.0f);

		m_spCamera1st->g_name = "TankFPS";
		m_spCamera1st->g_priority = 0.0f;

		APP.g_gameSystem->g_cameraSystem.AddCameraList(m_spCamera1st);
	}
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void Tank::Update(float deltaTime)
{
	if (APP.g_gameSystem->g_cameraSystem.IsEditorMode()) return;

	// 現在のステート更新
	m_spState->Update(*this, deltaTime);

	// 部品更新
	m_spTankParts->Update(deltaTime, m_moveSpeed, m_rotateSpeed);

	// カメラ角度
	auto& camera = RENDERER.Getcb9().Get().m_camera_matrix;
	m_cameraAngleY = atan2(camera.Backward().x, camera.Backward().z) * ToDegrees;
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
		float3 axisZ = m_spTankParts->GetMainGunMatrix().Backward();
		float3 pos = m_spTankParts->GetMainGunMatrix().Translation()
			+ float3(0.2f, 0.0f, 0.0f) + axisZ * 6.6f;

		APP.g_gameSystem->AddDebugSphereLine(pos, 2.0f);

		std::shared_ptr<Actor> cannon = std::make_shared<TankBullet>(*this, pos + axisZ, axisZ);
		APP.g_gameSystem->AddActorList(cannon);

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
		mfloat4x4 trans = mfloat4x4::CreateTranslation(owner.m_transform.GetPosition());
		owner.m_spCamera3rd->SetLocalPos(float3(0.0f, 0.0f, -10));
		owner.m_spCamera3rd->Update();
		owner.m_spCamera3rd->SetCameraMatrix(trans);
	}

	// 遷移
	if (APP.g_rawInputDevice->g_spMouse->IsPressed(MouseButton::Right)) {
		owner.m_spState = std::make_shared<State1st>();
		// 室内を表現
		if (owner.m_runSound3D) owner.m_runSound3D->SetFilter(XAUDIO2_FILTER_TYPE::LowPassFilter, 0.2f);
		if (owner.m_shotSound3D) owner.m_shotSound3D->SetFilter(XAUDIO2_FILTER_TYPE::LowPassFilter, 0.6f);

		owner.m_spCamera3rd->g_priority = 0;
		owner.m_spCamera1st->g_priority = 1;

		auto& angle = owner.m_spCamera3rd->GetRotationAngles();
		owner.m_spCamera1st->SetAngle(float2(angle.x, angle.y));
	}
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
		mfloat4x4 trans = mfloat4x4::CreateTranslation(owner.m_transform.GetPosition());
		float3 axisZ = owner.m_transform.GetWorldMatrix().Backward();
		float3 pos = float3(0.0f, 0.6f, 0.0f) + axisZ;
		owner.m_spCamera1st->SetLocalPos(pos);
		owner.m_spCamera1st->Update();
		owner.m_spCamera1st->SetCameraMatrix(trans);
	}

	// 遷移
	if (APP.g_rawInputDevice->g_spMouse->IsReleased(MouseButton::Right)) {
		owner.m_spState = std::make_shared<State3rd>();
		// フィルターをもとに戻す
		if (owner.m_runSound3D) owner.m_runSound3D->SetFilter(XAUDIO2_FILTER_TYPE::LowPassFilter, 1, 1);
		if (owner.m_shotSound3D) owner.m_shotSound3D->SetFilter(XAUDIO2_FILTER_TYPE::LowPassFilter, 1, 1);

		owner.m_spCamera3rd->g_priority = 1;
		owner.m_spCamera1st->g_priority = 0;

		auto& angle = owner.m_spCamera1st->GetRotationAngles();
		owner.m_spCamera3rd->SetAngle(float2(angle.x, angle.y));
	}
}

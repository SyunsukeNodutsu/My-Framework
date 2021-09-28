#include "Tank.h"
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
		m_runSound3D->Play();
		m_runSound3D->SetVolume(0.4f);
	}

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

	// 現在のステート更新
	m_spState->Update(*this, deltaTime);

	// 部品更新
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

//-----------------------------------------------------------------------------
// 走行の音更新
//-----------------------------------------------------------------------------
void Tank::UpdateRunSound()
{
	if (!m_runSound3D) return;

	auto moveSpeed = std::abs(m_moveSpeed);
	auto rotSpeed = std::abs(m_rotateSpeed) * 0.36f;
	auto volume = std::max(moveSpeed, rotSpeed);
	volume *= 0.08f;
	volume = std::clamp(volume, 0.2f, 0.6f);
	m_runSound3D->SetVolume(volume * 0.5f);
}

//-----------------------------------------------------------------------------
// 射撃更新
//-----------------------------------------------------------------------------
void Tank::UpdateShot(bool state1st)
{
	if (APP.g_rawInputDevice->g_spMouse->IsPressed(MouseButton::Left))
	{
		float3 axisZ = m_transform.GetWorldMatrix().Backward();
		float3 pos = m_transform.GetPosition() + float3(0.2f, 1.0f, 0.0f) + axisZ * 6.6f;

		// Effect
		APP.g_effectDevice->Play(u"Resource/Effect/TankFire.efk", pos);
		// Sound
		m_shotSound3D = SOUND_DIRECTOR.CreateSoundWork3D("Resource/Audio/SE/TankShot01.wav", false, true);
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

// 三人称State: 更新
void Tank::State3rd::Update(Tank& owner, float deltaTime)
{
	owner.UpdateMove(deltaTime);
	owner.UpdateRotate(deltaTime);

	owner.UpdateRunSound();

	owner.UpdateShot(false);

	// 遷移
	if (APP.g_rawInputDevice->g_spMouse->IsPressed(MouseButton::Right)) {
		owner.m_spState = std::make_shared<State1st>();
		// 室内を表現
		if (owner.m_runSound3D) owner.m_runSound3D->SetFilter(XAUDIO2_FILTER_TYPE::LowPassFilter, 0.08f);
		if (owner.m_shotSound3D) owner.m_shotSound3D->SetFilter(XAUDIO2_FILTER_TYPE::LowPassFilter, 0.08f);
	}
}

// 一人称State: 更新
void Tank::State1st::Update(Tank& owner, float deltaTime)
{
	owner.UpdateMove(deltaTime);
	owner.UpdateRotate(deltaTime);

	owner.UpdateRunSound();

	owner.UpdateShot(true);

	// 遷移
	if (APP.g_rawInputDevice->g_spMouse->IsReleased(MouseButton::Right)) {
		owner.m_spState = std::make_shared<State3rd>();
		// フィルターをもとに戻す
		if (owner.m_runSound3D) owner.m_runSound3D->SetFilter(XAUDIO2_FILTER_TYPE::LowPassFilter, 1, 1);
		if (owner.m_shotSound3D) owner.m_shotSound3D->SetFilter(XAUDIO2_FILTER_TYPE::LowPassFilter, 1, 1);
	}
}

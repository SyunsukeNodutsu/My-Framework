#include "Human.h"
#include "../main.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Human::Human()
	: m_spCamera(nullptr)
	, m_zoom(2)
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void Human::Initialize()
{
	m_name = "Human";

	LoadModel("Resource/Model/Robot/Robot.gltf");

	m_spCamera = std::make_shared<TPSCamera>();
	if(m_spCamera)
	{
		m_spCamera->Initialize();
		m_spCamera->SetClampAngleX(-75.0f, 90.0f);
		//m_spCamera->SetProjectionMatrix(60.0f);
		m_spCamera->SetAngle(0, 0);

		m_spCamera->SetLocalPos(float3(0.0f, 0.0f, -m_zoom));
		m_spCamera->SetLocalGazePosition(float3(0.0f, 1.4f, 0.0f));

		m_spCamera->SetEnable(false);

		// GameSystemにもカメラをシェアさせる
		APP.g_gameSystem.SetCamera(m_spCamera);
	}

	m_spState = std::make_shared<StateWait>();

	auto data = m_modelWork.GetAnimation("Stand");
	m_animator.SetAnimation(data);
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void Human::Finalize()
{
	m_spCamera.reset();
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void Human::Update(float deltaTime)
{
	// Escでカメラ操作切り替え
	static bool enable = false;
	if (APP.g_rawInputDevice.g_spKeyboard->IsPressed(KeyCode::Escape))
	{
		enable = !enable;
		m_spCamera->SetEnable(enable);
	}

	m_spState->Update(deltaTime, *this);

	// アニメーション
	float animationSpeed = 60.0f;
	if (APP.g_rawInputDevice.g_spKeyboard->IsDown(KeyCode::Shift)) animationSpeed *= 1.6f;
	if (APP.g_rawInputDevice.g_spKeyboard->IsDown(KeyCode::Alt)) animationSpeed *= 0.6f;
	m_animator.AdvanceTime(m_modelWork.WorkNodes(), animationSpeed * deltaTime);
	m_modelWork.CalcNodeMatrices();

	// TPS視点カメラ
	if (m_spCamera)
	{
		mfloat4x4 trans = mfloat4x4::CreateTranslation(m_transform.GetPosition());

		m_spCamera->SetLocalPos(float3(1.0f, 0.0f, -m_zoom));
		m_spCamera->Update();
		m_spCamera->SetCameraMatrix(trans);
	}
}

//-----------------------------------------------------------------------------
// 描画
//-----------------------------------------------------------------------------
void Human::Draw(float deltaTime)
{
	//D3D.GetRenderer().SetDitherEnable(true);
	GameObject::Draw(deltaTime);
	//D3D.GetRenderer().SetDitherEnable(false);
}

//-----------------------------------------------------------------------------
// 移動更新
//-----------------------------------------------------------------------------
void Human::UpdateMove(float deltaTime)
{
	float speed = 10.0f;

	// ダッシュ or スニーク？
	float moveSpd = 10;
	if (APP.g_rawInputDevice.g_spKeyboard->IsDown(KeyCode::Shift))
		speed *= 2;
	if (APP.g_rawInputDevice.g_spKeyboard->IsDown(KeyCode::Alt))
		speed *= 0.4f;

	auto axisZ = m_transform.GetWorldMatrix().Backward();
	axisZ.Normalize();
	axisZ *= speed * deltaTime;

	// 座標の更新 TransformのZ軸に前進
	auto pos = m_transform.GetPosition() + axisZ;
	m_transform.SetPosition(pos);
}

//-----------------------------------------------------------------------------
// 体の回転更新
//-----------------------------------------------------------------------------
void Human::UpdateRotate(float deltaTime)
{
	// 入力ベクトル取得
	float3 moveVec;
	if (APP.g_rawInputDevice.g_spKeyboard->IsDown(KeyCode::W)) moveVec.z += 1.0f;
	if (APP.g_rawInputDevice.g_spKeyboard->IsDown(KeyCode::S)) moveVec.z -= 1.0f;
	if (APP.g_rawInputDevice.g_spKeyboard->IsDown(KeyCode::A)) moveVec.x -= 1.0f;
	if (APP.g_rawInputDevice.g_spKeyboard->IsDown(KeyCode::D)) moveVec.x += 1.0f;
	moveVec.Normalize();

	// カメラを加味
	if (m_spCamera)
		moveVec = moveVec.TransformNormal(moveVec, m_spCamera->GetRotationYMatrix());

	//--------------------------------------------------

	if (moveVec.LengthSquared() == 0.0f)
		return;

	// 現在の方向
	float3 nowDir = m_transform.GetWorldMatrix().Backward();// なんかForwardだと逆になる
	nowDir.Normalize();

	float nowAngleY		= atan2(nowDir.x, nowDir.z);
	float targetAngleY  = atan2(moveVec.x, moveVec.z);	// 移動方向への角度
	float rotateRadian  = targetAngleY - nowAngleY;		// 差分を計算

	if (rotateRadian > PI)
		rotateRadian -= 2 * float(PI);
	else if (rotateRadian < -PI)
		rotateRadian += 2 * float(PI);

	// 回転制限
	constexpr float rotlimit = 30.0f;
	//rotateRadian = std::clamp(rotateRadian, -rotlimit * ToRadians, rotlimit * ToRadians);

	constexpr float rotPow = 400.0f;
	m_rotation.y += rotateRadian * rotPow * deltaTime;
	m_transform.SetAngle(m_rotation);
}

void Human::StateWait::Update(float deltaTime, Human& owner)
{
	float3 moveVec = float3::Zero;
	if (APP.g_rawInputDevice.g_spKeyboard->IsDown(KeyCode::W)) moveVec.z += 1.0f * deltaTime;
	if (APP.g_rawInputDevice.g_spKeyboard->IsDown(KeyCode::S)) moveVec.z -= 1.0f * deltaTime;
	if (APP.g_rawInputDevice.g_spKeyboard->IsDown(KeyCode::A)) moveVec.x -= 1.0f * deltaTime;
	if (APP.g_rawInputDevice.g_spKeyboard->IsDown(KeyCode::D)) moveVec.x += 1.0f * deltaTime;
	moveVec.Normalize();

	if (moveVec.LengthSquared() >= 0.01f)
	{
		owner.m_spState = nullptr;
		owner.m_spState = std::make_shared<StateMove>();

		auto data = owner.m_modelWork.GetAnimation("Walk");
		owner.m_animator.SetAnimation(data);
	}
}

void Human::StateMove::Update(float deltaTime, Human& owner)
{
	owner.UpdateMove(deltaTime);
	owner.UpdateRotate(deltaTime);

	// TODO: fix
	float3 moveVec = float3::Zero;
	if (APP.g_rawInputDevice.g_spKeyboard->IsDown(KeyCode::W)) moveVec.z += 1.0f * deltaTime;
	if (APP.g_rawInputDevice.g_spKeyboard->IsDown(KeyCode::S)) moveVec.z -= 1.0f * deltaTime;
	if (APP.g_rawInputDevice.g_spKeyboard->IsDown(KeyCode::A)) moveVec.x -= 1.0f * deltaTime;
	if (APP.g_rawInputDevice.g_spKeyboard->IsDown(KeyCode::D)) moveVec.x += 1.0f * deltaTime;
	moveVec.Normalize();

	if (moveVec.LengthSquared() == 0)
	{
		owner.m_spState = nullptr;
		owner.m_spState = std::make_shared<StateWait>();

		auto data = owner.m_modelWork.GetAnimation("Stand");
		owner.m_animator.SetAnimation(data);
	}
}

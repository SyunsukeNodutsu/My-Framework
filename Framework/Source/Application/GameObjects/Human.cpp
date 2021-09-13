#include "Human.h"
#include "../GameSystem.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Human::Human()
	: m_spCamera(nullptr)
	, m_zoom(6)
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void Human::Initialize()
{
	LoadModel("Resource/Model/Robot/Robot.gltf");

	m_spCamera = std::make_shared<TPSCamera>();
	if(m_spCamera)
	{
		m_spCamera->Initialize();
		m_spCamera->SetClampAngleX(-75.0f, 90.0f);
		//m_spCamera->SetProjectionMatrix(60.0f);
		m_spCamera->SetAngle(45, -0);

		m_spCamera->SetLocalPos(float3(0.0f, 0.0f, -m_zoom));
		m_spCamera->SetLocalGazePosition(float3(0.0f, 1.0f, 0.0f));

		// GameSystemにもカメラをシェアさせる
		GAMESYSTEM.SetCamera(m_spCamera);
	}

	auto data = m_modelWork.GetAnimation("Walk");
	m_animator.SetAnimation(data, true);
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
	static bool enable = true;
	if (RAW_INPUT.GetKeyboard()->IsPressed(KeyCode::Escape))
	{
		enable = !enable;
		m_spCamera->SetEnable(enable);
	}

	// 移動
	UpdateMove(deltaTime);

	// アニメーション
	float animationSpeed = 60.0f;
	if (RAW_INPUT.GetKeyboard()->IsDown(KeyCode::Shift))
		animationSpeed *= 1.6f;
	if (RAW_INPUT.GetKeyboard()->IsDown(KeyCode::Control))
		animationSpeed *= 0.6f;
	m_animator.AdvanceTime(m_modelWork.WorkNodes(), animationSpeed * deltaTime);
	m_modelWork.CalcNodeMatrices();

	// TPS視点カメラ
	if (m_spCamera)
	{
		mfloat4x4 trans = mfloat4x4::CreateTranslation(m_transform.GetPosition());

		m_spCamera->SetLocalPos(float3(0.0f, 0.0f, -m_zoom));
		m_spCamera->Update();
		m_spCamera->SetCameraMatrix(trans);
	}
}

//-----------------------------------------------------------------------------
// 描画
//-----------------------------------------------------------------------------
void Human::Draw(float deltaTime)
{
	D3D.GetRenderer().SetDitherEnable(true);
	GameObject::Draw(deltaTime);
	D3D.GetRenderer().SetDitherEnable(false);
}

//-----------------------------------------------------------------------------
// 移動更新
//-----------------------------------------------------------------------------
void Human::UpdateMove(float deltaTime)
{
	float moveSpd = 0.006f;
	if (RAW_INPUT.GetKeyboard()->IsDown(KeyCode::Shift))
		moveSpd *= 2;
	if (RAW_INPUT.GetKeyboard()->IsDown(KeyCode::Control))
		moveSpd *= 0.4f;

	float3 moveVec;
	if (RAW_INPUT.GetKeyboard()->IsDown(KeyCode::W)) moveVec.z += 1.0f * deltaTime;
	if (RAW_INPUT.GetKeyboard()->IsDown(KeyCode::S)) moveVec.z -= 1.0f * deltaTime;
	if (RAW_INPUT.GetKeyboard()->IsDown(KeyCode::A)) moveVec.x -= 1.0f * deltaTime;
	if (RAW_INPUT.GetKeyboard()->IsDown(KeyCode::D)) moveVec.x += 1.0f * deltaTime;
	moveVec.Normalize();
	moveVec *= moveSpd;

	if (m_spCamera)
		moveVec = moveVec.TransformNormal(moveVec, m_spCamera->GetRotationYMatrix());

	// 座標の更新
	auto pos = m_transform.GetPosition() + moveVec;
	m_transform.SetPosition(pos);

	RotateBody(moveVec);
}

//-----------------------------------------------------------------------------
// 体の回転更新
//-----------------------------------------------------------------------------
void Human::RotateBody(const float3& moveVec)
{
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
	constexpr float rotPow = 30.0f;
	rotateRadian = std::clamp(rotateRadian, -rotPow * ToRadians, rotPow * ToRadians);

	m_rotation.y += rotateRadian;
	m_transform.SetAngle(m_rotation);
}

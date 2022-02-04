#include "Human.h"
#include "../main.h"
#include "../../Framework/Audio/SoundDirector.h"

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
	g_name = "Human";

	LoadModel("Resource/Model/Robot/Robot.gltf");

	m_spCamera = std::make_shared<TPSCamera>();
	if(m_spCamera)
	{
		m_spCamera->Initialize();
		m_spCamera->SetClampAngleX(-75.0f, 90.0f);

		m_spCamera->SetLocalPos(float3(0.0f, 0.0f, -m_zoom));
		m_spCamera->SetLocalGazePosition(float3(0.0f, 1.4f, 0.0f));

		m_spCamera->g_priority = 1.0f;
		m_spCamera->g_name = "HumanTPS";
		g_application->g_gameSystem->g_cameraSystem.AddCameraList(m_spCamera);
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
	if (!g_application->g_gameSystem->g_cameraSystem.IsEditorMode())
	{
		if(m_spState != nullptr)
			m_spState->Update(deltaTime, *this);
	}

	// アニメーション
	float animationSpeed = 60.0f;
	if (g_application->g_inputDevice->IsKeyDown(KeyCode::Shift)) animationSpeed *= 1.6f;
	if (g_application->g_inputDevice->IsKeyDown(KeyCode::Alt)) animationSpeed *= 0.6f;
	m_animator.AdvanceTime(m_modelWork.WorkNodes(), animationSpeed * deltaTime);
	m_modelWork.CalcNodeMatrices();

	//エフェクトテスト
	static float timecount = 0.0f;
	timecount += deltaTime;
	if (timecount >= 3.0f)
	{
		const auto& pos = float3(0, 4, 0);
		//g_application->g_effectDevice->Play(u"Resource/Effect/003_snowstorm_effect/snowstorm11.efk", pos, 0.2f);
		g_application->g_effectDevice->Play(u"Resource/Effect/AndrewFM01/electric_dust.efk", pos);

		timecount = 0.0f;
	}

	// TPS視点カメラ
	if (m_spCamera)
	{
		mfloat4x4 trans = mfloat4x4::CreateTranslation(m_transform.GetPosition());

		auto delta = g_application->g_inputDevice->GetMouseWheelDelta() * -1;

		m_zoom += delta * deltaTime * 0.8f;

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
	Actor::Draw(deltaTime);
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
	if (g_application->g_inputDevice->IsKeyDown(KeyCode::Shift))
		speed *= 2;
	if (g_application->g_inputDevice->IsKeyDown(KeyCode::Alt))
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
	if (g_application->g_inputDevice->IsKeyDown(KeyCode::W)) moveVec.z += 1.0f;
	if (g_application->g_inputDevice->IsKeyDown(KeyCode::S)) moveVec.z -= 1.0f;
	if (g_application->g_inputDevice->IsKeyDown(KeyCode::A)) moveVec.x -= 1.0f;
	if (g_application->g_inputDevice->IsKeyDown(KeyCode::D)) moveVec.x += 1.0f;
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

	constexpr float rotPow = 400.0f;
	m_rotation.y += rotateRadian * rotPow * deltaTime;

	if (m_rotation.y > 360) m_rotation.y -= 360.0f;
	if (m_rotation.y < 0) m_rotation.y += 360.0f;

	m_transform.SetAngle(m_rotation);
}

//-----------------------------------------------------------------------------
// 衝突判定更新
//-----------------------------------------------------------------------------
void Human::UpdateCollision()
{
	CheckBump();
}

//-----------------------------------------------------------------------------
// ぶつかり判定
//-----------------------------------------------------------------------------
void Human::CheckBump()
{
	//球の設定
	float3 center = m_transform.GetPosition();
	center.y += 0.8f;//少し持ち上げる
	float radius = 0.4f;//半径いいかんじに設定

	g_application->g_gameSystem->AddDebugSphereLine(center, radius);

	float3 push = float3::Zero;

	for (auto& actor : g_application->g_gameSystem->GetActorList())
	{
		if (actor.get() == this) continue;

		//地面と判定
		if (actor->g_tag & 8)
		{
			if (actor->CheckCollision(center, radius, push))
				m_transform.SetPosition(m_transform.GetPosition() + push);
		}
	}
}



//=============================================================================
// 
// State machine
// 
//=============================================================================

//-----------------------------------------------------------------------------
// 待機State: 更新
//-----------------------------------------------------------------------------
void Human::StateWait::Update(float deltaTime, Human& owner)
{
	owner.UpdateCollision();

	//TODO: 修正
	float3 moveVec = float3::Zero;
	if (g_application->g_inputDevice->IsKeyDown(KeyCode::W)) moveVec.z += 1.0f;
	if (g_application->g_inputDevice->IsKeyDown(KeyCode::S)) moveVec.z -= 1.0f;
	if (g_application->g_inputDevice->IsKeyDown(KeyCode::A)) moveVec.x -= 1.0f;
	if (g_application->g_inputDevice->IsKeyDown(KeyCode::D)) moveVec.x += 1.0f;
	moveVec.Normalize();

	//遷移 移動ステート
	if (moveVec.LengthSquared() >= 0.01f)
	{
		owner.m_spState = nullptr;
		owner.m_spState = std::make_shared<StateMove>();

		auto data = owner.m_modelWork.GetAnimation("Walk");
		owner.m_animator.SetAnimation(data);
	}
}

//-----------------------------------------------------------------------------
// 移動State: 更新
//-----------------------------------------------------------------------------
void Human::StateMove::Update(float deltaTime, Human& owner)
{
	owner.UpdateMove(deltaTime);
	owner.UpdateRotate(deltaTime);
	owner.UpdateCollision();

	//TODO: fix
	float3 moveVec = float3::Zero;
	if (g_application->g_inputDevice->IsKeyDown(KeyCode::W)) moveVec.z += 1.0f;
	if (g_application->g_inputDevice->IsKeyDown(KeyCode::S)) moveVec.z -= 1.0f;
	if (g_application->g_inputDevice->IsKeyDown(KeyCode::A)) moveVec.x -= 1.0f;
	if (g_application->g_inputDevice->IsKeyDown(KeyCode::D)) moveVec.x += 1.0f;
	moveVec.Normalize();

	//遷移 待機ステート
	if (moveVec.LengthSquared() == 0)
	{
		owner.m_spState = nullptr;
		owner.m_spState = std::make_shared<StateWait>();

		auto data = owner.m_modelWork.GetAnimation("Stand");
		owner.m_animator.SetAnimation(data);
	}
}

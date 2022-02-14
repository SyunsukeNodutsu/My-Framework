#include "Human.h"
#include "../main.h"
#include "../../Framework/Audio/SoundDirector.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Human::Human()
	: m_spCamera(nullptr)
	, m_animator()
	, m_rotation(float3::Zero)
	, m_zoom(10.0f)
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
	if (g_application->g_gameSystem->g_cameraSystem.IsEditorMode()) {

		//さびしいから動いて
		m_animator.AdvanceTime(m_modelWork.WorkNodes(), 60.0f * deltaTime);
		m_modelWork.CalcNodeMatrices();

		return;
	}

	//現在のステート更新 各種更新を記述
	if (m_spState)
		m_spState->Update(deltaTime, *this);
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
	const auto& input = g_application->g_inputDevice;

	// ダッシュ or スニーク？
	float speed = 10.0f;
	if (input->IsKeyDown(KeyCode::Shift)) speed *= 2;
	else if (input->IsKeyDown(KeyCode::Alt)) speed *= 0.4f;

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
	//入力ベクトル取得
	const float& horizontal = g_application->g_inputDevice->GetHorizontal();
	const float& vertical = g_application->g_inputDevice->GetVertical();
	float3 moveVec = float3(horizontal, 0, vertical);
	if (moveVec.LengthSquared() == 0.0f) return;

	// カメラを加味
	if (m_spCamera)
		moveVec = moveVec.TransformNormal(moveVec, m_spCamera->GetRotationYMatrix());

	// 現在の方向
	float3 nowDir = m_transform.GetWorldMatrix().Backward();
	nowDir.Normalize();

	float nowAngleY = atan2(nowDir.x, nowDir.z);
	float targetAngleY = atan2(moveVec.x, moveVec.z);//移動方向への角度
	float rotateRadian = targetAngleY - nowAngleY;//差分を計算

	//360度の切れ目
	if (rotateRadian > PI) rotateRadian -= 2 * float(PI);
	else if (rotateRadian < -PI) rotateRadian += 2 * float(PI);

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
	float distanceFromGround = FLT_MAX;
	if (CheckGround(distanceFromGround))
	{

	}
	
	CheckBump();
}

//-----------------------------------------------------------------------------
// カメラ更新
//-----------------------------------------------------------------------------
void Human::UpdateCamera(float deltaTime)
{
	if (m_spCamera == nullptr) return;

	mfloat4x4 trans = mfloat4x4::CreateTranslation(m_transform.GetPosition());

	const auto& delta = g_application->g_inputDevice->GetMouseWheelDelta() * -1;
	m_zoom += delta * deltaTime * 0.8f;

	m_spCamera->SetLocalPos(float3(1.0f, 0.0f, -m_zoom));
	m_spCamera->Update();
	m_spCamera->SetCameraMatrix(trans);
}

//-----------------------------------------------------------------------------
// アニメーション更新
//-----------------------------------------------------------------------------
void Human::UpdateAnimation(float deltaTime)
{
	const auto& input = g_application->g_inputDevice;
	
	float animationSpeed = 60.0f;
	if (input->IsKeyDown(KeyCode::Shift)) animationSpeed *= 1.6f;
	else if (input->IsKeyDown(KeyCode::Alt)) animationSpeed *= 0.6f;

	m_animator.AdvanceTime(m_modelWork.WorkNodes(), animationSpeed * deltaTime);
	m_modelWork.CalcNodeMatrices();
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
		//対象は地面
		if (actor.get() == this) continue;
		if (!(actor->g_tag & ACTOR_TAG::GROUND)) continue;

		if (actor->CheckCollision(center, radius, push))
			m_transform.SetPosition(m_transform.GetPosition() + push);
	}
}

//-----------------------------------------------------------------------------
// 地面との判定
//-----------------------------------------------------------------------------
bool Human::CheckGround(float& dstDistance)
{


	return false;
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
	//ステートで行う更新
	owner.UpdateCamera(deltaTime);
	owner.UpdateAnimation(deltaTime);

	//入力ベクトル取得
	const float& horizontal = g_application->g_inputDevice->GetHorizontal();
	const float& vertical = g_application->g_inputDevice->GetVertical();
	float3 moveVec = float3(horizontal, 0, vertical);

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
	//ステートで行う更新
	owner.UpdateMove(deltaTime);
	owner.UpdateRotate(deltaTime);
	//owner.UpdateCollision();
	owner.UpdateCamera(deltaTime);
	owner.UpdateAnimation(deltaTime);

	//入力ベクトル取得
	const float& horizontal = g_application->g_inputDevice->GetHorizontal();
	const float& vertical = g_application->g_inputDevice->GetVertical();
	float3 moveVec = float3(horizontal, 0, vertical);

	//こっちのがいいかも
	//const float& horizontal = Input::GetAxis("Horizontal");

	//遷移 待機ステート
	if (moveVec.LengthSquared() == 0)
	{
		owner.m_spState = nullptr;
		owner.m_spState = std::make_shared<StateWait>();

		auto data = owner.m_modelWork.GetAnimation("Stand");
		owner.m_animator.SetAnimation(data);
	}
}

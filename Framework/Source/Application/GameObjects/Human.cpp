#include "Human.h"
#include "../GameSystem.h"

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void Human::Initialize()
{
	auto spModel = std::make_shared<ModelData>();
	spModel->Load("Resource/Model/Robot/Robot.gltf");

	m_modelWork.SetModel (spModel);
	//m_modelWork.CalcWorldMatrixes();
	//m_modelWork.Transform().Translation(float3::Zero);
	//m_modelWork.Transform() *= mfloat4x4::CreateRotationY(180 * ToRadians);

	spModel.reset();
	spModel = nullptr;

	m_spCamera = std::make_shared<TPSCamera>();

	//? GameSystemにもカメラをシェアさせる
	GAMESYSTEM.SetCamera(m_spCamera);

	m_spCamera->Initialize();
	m_spCamera->SetClampAngleX(-75.0f, 90.0f);
	//m_spCamera->SetProjectionMatrix(60.0f);

	m_spCamera->SetAngle(45, -0);
	m_zoom = 6;

	m_spCamera->SetLocalPos(float3(0.0f, 0.0f, -m_zoom));
	// 注視点
	m_spCamera->SetLocalGazePosition(float3(0.0f, 1.0f, 0.0f));
}

//-----------------------------------------------------------------------------
// 終了
//-----------------------------------------------------------------------------
void Human::Finalize()
{

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

	static float animationSpeed = 60.0f;
	//m_modelWork.PlayAnimation(animationSpeed * deltaTime);

	// 移動
	if (enable)
	{
		float moveSpd = 0.006f;
		if (RAW_INPUT.GetKeyboard()->IsDown(KeyCode::Shift))
			moveSpd *= 2;

		float3 moveVec;
		if (RAW_INPUT.GetKeyboard()->IsDown(KeyCode::W)) { moveVec.z += 1.0f * deltaTime; }// Wキーで前進
		if (RAW_INPUT.GetKeyboard()->IsDown(KeyCode::S)) { moveVec.z -= 1.0f * deltaTime; }// Sキーで後退
		if (RAW_INPUT.GetKeyboard()->IsDown(KeyCode::A)) { moveVec.x -= 1.0f * deltaTime; }// Aキーで左移動
		if (RAW_INPUT.GetKeyboard()->IsDown(KeyCode::D)) { moveVec.x += 1.0f * deltaTime; }// Dキーで右へ移動
		moveVec.Normalize();

		moveVec *= moveSpd;

		if (m_spCamera)
			moveVec = moveVec.TransformNormal(moveVec, m_spCamera->GetRotationYMatrix());

		// 座標の更新
		auto pos = m_transform.GetPosition() + moveVec;
		m_transform.SetPosition(pos);
	}

	// Transform更新
	// TODO: ここ汚い 描画で(model, transform)って引数で指定した方がいい
	//m_modelWork.Transform() = m_transform.GetWorldMatrix();

	if (m_spCamera)
	{
		m_spCamera->SetLocalPos(float3(0.0f, 0.0f, -m_zoom));
		m_spCamera->Update();

		mfloat4x4 trans = mfloat4x4::CreateTranslation(m_transform.GetPosition());

		// プレイヤー追従カメラの位置
		// キャラクターから見たカメラの位置にプレイヤーの位置を合成したもの
		m_spCamera->SetCameraMatrix(trans);

		// カメラの方向に回転
		// TODO: すぐに回転しない
		float3 angle = m_spCamera->GetRotationAngles();
		m_transform.SetAngle(float3(0, angle.y, 0));
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

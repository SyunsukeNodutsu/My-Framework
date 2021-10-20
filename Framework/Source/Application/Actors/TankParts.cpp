#include "TankParts.h"
#include "Tank.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
TankParts::TankParts(Tank& owner)
	: m_owner(owner)
	, m_trackR()
	, m_trackOffsetR()
	, m_trackL()
	, m_trackOffsetL()
	, m_turret()
	, m_turretOffset()
	, m_mainGun()
	, m_mainGunOffset()
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void TankParts::Initialize()
{
	float3 ownerOffset = m_owner.GetTransform().GetPosition();

	m_trackR.LoadModel("Resource/Model/T43/T43_Track.gltf");
	m_trackR.SetUVScroll(true);
	m_trackOffsetR = mfloat4x4::CreateTranslation(ownerOffset + float3(1.1f, -1.1f, 0.32f));

	m_trackL.LoadModel("Resource/Model/T43/T43_Track.gltf");
	m_trackL.SetUVScroll(true);
	m_trackOffsetL = mfloat4x4::CreateTranslation(ownerOffset + float3(-1.1f, -1.1f, 0.32f));

	m_turret.LoadModel("Resource/Model/T43/T43_Turret.gltf");
	m_turretOffset = mfloat4x4::CreateTranslation(ownerOffset + float3(0.0f, 0.42f, 0.94f));

	m_mainGun.LoadModel("Resource/Model/T43/T43_MainGun.gltf");
	m_mainGunOffset = mfloat4x4::CreateTranslation(ownerOffset + float3(0.0f, 0.8f, 1.8f));

	// 左右5個づつのタイヤ
	{
		constexpr float diff = 0.852f;
		for (int i = 0; i < 5; i++) {
			m_tireR[i].LoadModel("Resource/Model/T43/T43_Tire.gltf");
			m_tireL[i].LoadModel("Resource/Model/T43/T43_Tire.gltf");

			m_tireOffsetR[i] = mfloat4x4::CreateTranslation(ownerOffset + float3( 1.2f, -0.64f, -1.48f + (diff * i)));
			m_tireOffsetL[i] = mfloat4x4::CreateTranslation(ownerOffset + float3(-1.2f, -0.64f, -1.48f + (diff * i)));
		}
	}
	// 左右2個ずつのミニタイヤ
	{
		constexpr float diff = 4.76f;
		for (int i = 0; i < 2; i++) {
			m_miniTireR[i].LoadModel("Resource/Model/T43/T43_TireMin.gltf");
			m_miniTireL[i].LoadModel("Resource/Model/T43/T43_TireMin.gltf");

			m_miniTireOffsetR[i] = mfloat4x4::CreateTranslation(ownerOffset + float3( 1.2f, -0.42f, -2.16f + (diff * i)));
			m_miniTireOffsetL[i] = mfloat4x4::CreateTranslation(ownerOffset + float3(-1.2f, -0.42f, -2.16f + (diff * i)));
		}
	}
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void TankParts::Update(float deltaTime, float moveSpeed, float rotSpeed)
{
	// 行列計算
	auto& ownerMatrix = m_owner.GetTransform().GetWorldMatrix();

	// キャタピラ
	{
		// 前後移動から
		float trackMove = moveSpeed * 0.4f;
		trackMove = std::clamp(trackMove, -1.6f, 1.6f);
		// 左右回転から
		float trackRotMove = rotSpeed * 0.06f;
		trackRotMove = std::clamp(trackRotMove, -1.6f, 1.6f);

		static float uvoffsetR = 0;
		uvoffsetR += trackRotMove * deltaTime;
		uvoffsetR -= trackMove * deltaTime;
		m_trackR.SetUVOffset(float2(0, uvoffsetR));

		static float uvoffsetL = 0;
		uvoffsetL -= trackRotMove * deltaTime;
		uvoffsetL -= trackMove * deltaTime;
		m_trackL.SetUVOffset(float2(0, uvoffsetL));

		m_trackR.GetTransform().SetWorldMatrix(m_trackOffsetR * ownerMatrix);
		m_trackL.GetTransform().SetWorldMatrix(m_trackOffsetL * ownerMatrix);
	}

	// 砲塔.主砲
	{
		auto& ownerAngle = m_owner.GetTransform().GetAngle();
		auto ownerCameraAngle = m_owner.GetCameraAngle();

		m_turret.GetTransform().SetWorldMatrix(m_turretOffset * ownerMatrix);
		m_mainGun.GetTransform().SetWorldMatrix(m_turretOffset * ownerMatrix);

		// XZは車体の回転 Yはカメラの回転
		m_turret.GetTransform().SetAngle(float3(ownerAngle.x, ownerCameraAngle.y, ownerAngle.z));
		m_mainGun.GetTransform().SetAngle(float3(ownerAngle.x, ownerCameraAngle.y, ownerAngle.z));

		auto pos = m_mainGun.GetTransform().GetPosition();
		m_mainGun.GetTransform().SetPosition(
			pos + float3(0, 0.4f, 0) +
			(m_mainGun.GetTransform().GetWorldMatrix().Backward() * 0.8f)
		);
	}

	// タイヤ
	{
		// 前後移動から
		float trackMove = moveSpeed * 48.0f;
		trackMove = std::clamp(trackMove, -280.0f, 280.0f);
		// 左右回転から
		float trackRotMove = rotSpeed * 10.0f;
		trackRotMove = std::clamp(trackRotMove, -200.0f, 200.0f);

		static float tireRotR = 0;
		tireRotR -= trackRotMove * deltaTime;
		tireRotR += trackMove * deltaTime;

		static float tireRotL = 0;
		tireRotL -= trackRotMove * deltaTime;
		tireRotL -= trackMove * deltaTime;

		auto& ownerAngle = m_owner.GetTransform().GetAngle();
		for (int i = 0; i < 5; i++)
		{
			m_tireR[i].GetTransform().SetWorldMatrix(m_tireOffsetR[i] * ownerMatrix);
			m_tireL[i].GetTransform().SetWorldMatrix(m_tireOffsetL[i] * ownerMatrix);

			m_tireR[i].GetTransform().SetAngle(float3(tireRotR, ownerAngle.y, ownerAngle.z));
			m_tireL[i].GetTransform().SetAngle(float3(tireRotL, ownerAngle.y + 180, ownerAngle.z));
		}

		// ミニタイヤ
		for (int i = 0; i < 2; i++)
		{
			m_miniTireR[i].GetTransform().SetWorldMatrix(m_miniTireOffsetR[i] * ownerMatrix);
			m_miniTireL[i].GetTransform().SetWorldMatrix(m_miniTireOffsetL[i] * ownerMatrix);

			m_miniTireR[i].GetTransform().SetAngle(float3(tireRotR, ownerAngle.y, ownerAngle.z));
			m_miniTireL[i].GetTransform().SetAngle(float3(tireRotL, ownerAngle.y + 180, ownerAngle.z));
		}
	}
}

//-----------------------------------------------------------------------------
// 描画
//-----------------------------------------------------------------------------
void TankParts::Draw(float deltaTime)
{
	m_trackR.Draw(deltaTime);
	m_trackL.Draw(deltaTime);

	m_turret.Draw(deltaTime);
	m_mainGun.Draw(deltaTime);

	for (int i = 0; i < 5; i++) {
		m_tireR[i].Draw(deltaTime);
		m_tireL[i].Draw(deltaTime);
	}

	for (int i = 0; i < 2; i++) {
		m_miniTireR[i].Draw(deltaTime);
		m_miniTireL[i].Draw(deltaTime);
	}
}

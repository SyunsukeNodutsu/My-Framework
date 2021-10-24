#include "TankParts.h"
#include "Tank.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
TankParts::TankParts(Tank& owner)
	: m_owner(owner)
	, m_uvoffsetR(0.0f)
	, m_uvoffsetL(0.0f)
	, m_trackR()
	, m_trackOffsetR()
	, m_trackL()
	, m_trackOffsetL()
	, m_turret()
	, m_turretOffset()
	, m_mainGun()
	, m_mainGunOffset()
	, m_tireRotR(0.0f)
	, m_tireRotL(0.0f)
	, m_tireR()
	, m_tireOffsetR()
	, m_tireL()
	, m_tireOffsetL()
	, m_miniTireL()
	, m_miniTireOffsetL()
{
}

//-----------------------------------------------------------------------------
// 初期化
//-----------------------------------------------------------------------------
void TankParts::Initialize()
{
	const auto& ownerOffset = m_owner.GetTransform().GetPosition();

	m_trackR.LoadModel("Resource/Model/T43/T43_Track.gltf");
	m_trackR.g_isUVScroll = true;
	m_trackOffsetR = mfloat4x4::CreateTranslation(ownerOffset + float3(1.1f, -1.1f, 0.32f));

	m_trackL.LoadModel("Resource/Model/T43/T43_Track.gltf");
	m_trackL.g_isUVScroll = true;
	m_trackOffsetL = mfloat4x4::CreateTranslation(ownerOffset + float3(-1.1f, -1.1f, 0.32f));

	m_turret.LoadModel("Resource/Model/T43/T43_Turret.gltf");
	m_turretOffset = mfloat4x4::CreateTranslation(ownerOffset + float3(0.0f, 0.42f, 0.94f));

	// 主砲はownerではなく砲塔の子としてOffsetを計算
	m_mainGun.LoadModel("Resource/Model/T43/T43_MainGun.gltf");
	m_mainGunOffset = mfloat4x4::CreateTranslation(ownerOffset + float3(0.0f, 0.4f, 0.82f));

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
// jsonファイルの逆シリアル
//-----------------------------------------------------------------------------
void TankParts::Deserialize(const json11::Json& jsonObject)
{
	// 部品のモデルを読み込み

	if (!jsonObject["track_model_filepath"].is_null()) {
		m_trackR.LoadModel(jsonObject["track_model_filepath"].string_value());
		m_trackL.LoadModel(jsonObject["track_model_filepath"].string_value());
	}
	if (!jsonObject["turret_model_filepath"].is_null()) {
		m_turret.LoadModel(jsonObject["turret_model_filepath"].string_value());
	}
	if (!jsonObject["mainGun_model_filepath"].is_null()) {
		m_mainGun.LoadModel(jsonObject["trackL_model_filepath"].string_value());
	}

	if (!jsonObject["tire_model_filepath"].is_null()) {
		for (int i = 0; i < 5; i++) {
			m_tireR[i].LoadModel(jsonObject["tire_model_filepath"].string_value());
			m_tireL[i].LoadModel(jsonObject["tire_model_filepath"].string_value());
		}
	}
	if (!jsonObject["miniTire_model_filepath"].is_null()) {
		for (int i = 0; i < 2; i++) {
			m_miniTireR[i].LoadModel(jsonObject["miniTire_model_filepath"].string_value());
			m_miniTireL[i].LoadModel(jsonObject["miniTire_model_filepath"].string_value());
		}
	}
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void TankParts::Update(float deltaTime, float moveSpeed, float rotSpeed)
{
	// キャタピラ
	UpdateTrack(deltaTime, moveSpeed, rotSpeed);

	// 砲塔/主砲
	UpdateTurret(deltaTime);
	UpdateMainGun(deltaTime);

	// タイヤ
	UpdateTire(deltaTime, moveSpeed, rotSpeed);
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

//-----------------------------------------------------------------------------
// キャタピラ更新
//-----------------------------------------------------------------------------
void TankParts::UpdateTrack(float deltaTime, float moveSpeed, float rotSpeed)
{
	const auto& ownerMatrix = m_owner.GetTransform().GetWorldMatrix();

	// 前後移動から
	float trackMove = moveSpeed * 0.4f;
	trackMove = std::clamp(trackMove, -1.6f, 1.6f);
	// 左右回転から
	float trackRotMove = rotSpeed * 0.06f;
	trackRotMove = std::clamp(trackRotMove, -1.6f, 1.6f);

	m_uvoffsetR += trackRotMove * deltaTime;
	m_uvoffsetR -= trackMove * deltaTime;
	m_trackR.g_numUVOffset = float2(0, m_uvoffsetR);

	m_uvoffsetL -= trackRotMove * deltaTime;
	m_uvoffsetL -= trackMove * deltaTime;
	m_trackL.g_numUVOffset = float2(0, m_uvoffsetL);

	m_trackR.GetTransform().SetWorldMatrix(m_trackOffsetR * ownerMatrix);
	m_trackL.GetTransform().SetWorldMatrix(m_trackOffsetL * ownerMatrix);
}

//-----------------------------------------------------------------------------
// 砲塔更新
//-----------------------------------------------------------------------------
void TankParts::UpdateTurret(float deltaTime)
{
	const auto& ownerMatrix  = m_owner.GetTransform().GetWorldMatrix();
	const auto& cameraAngleY = m_owner.GetCameraAngle().y * ToRadians;

	float3 nowDir = m_turret.GetTransform().GetWorldMatrix().Backward();
	nowDir.Normalize();

	float rotateRadian = cameraAngleY - atan2f(nowDir.x, nowDir.z);

	if (rotateRadian > PI) rotateRadian -= 2 * PI;
	else if (rotateRadian < -PI) rotateRadian += 2 * PI;

	// 補正
	constexpr float fixRotMax = 30.0f * ToRadians;
	rotateRadian = std::clamp(rotateRadian, -fixRotMax, fixRotMax);

	// 回転
	// TODO: 回転量の最低値を設定
	constexpr float rotSpeed = 10.0f;
	m_turretOffset = mfloat4x4::CreateRotationY(rotateRadian * rotSpeed * deltaTime) * m_turretOffset;

	m_turret.GetTransform().SetWorldMatrix(m_turretOffset * ownerMatrix);
}

//-----------------------------------------------------------------------------
// 主砲更新
//-----------------------------------------------------------------------------
void TankParts::UpdateMainGun(float deltaTime)
{
	const auto& ownerMatrix  = m_owner.GetTransform().GetWorldMatrix();
	const auto& cameraAngleX = m_owner.GetCameraAngle().x;
	
	const float mainGunAngleX = DirectX::XMVector3Dot(
		m_mainGun.GetTransform().GetWorldMatrix().Backward(),
		ownerMatrix.Up()).m128_f32[0] * -1 * ToDegrees;

	float rotateRadian = (cameraAngleX - mainGunAngleX) * ToRadians;

	// 回転
	constexpr float rotSpeed = 10.0f;
	m_mainGunOffset = mfloat4x4::CreateRotationX(rotateRadian * rotSpeed * deltaTime) * m_mainGunOffset;

	m_mainGun.GetTransform().SetWorldMatrix(m_mainGunOffset * m_turret.GetTransform().GetWorldMatrix());
}

//-----------------------------------------------------------------------------
// タイヤ更新
//-----------------------------------------------------------------------------
void TankParts::UpdateTire(float deltaTime, float moveSpeed, float rotSpeed)
{
	const auto& ownerMatrix = m_owner.GetTransform().GetWorldMatrix();
	const auto& ownerAngle  = m_owner.GetTransform().GetAngle();

	// 前後移動から
	float trackMove = moveSpeed * 48.0f;
	trackMove = std::clamp(trackMove, -280.0f, 280.0f);
	// 左右回転から
	float trackRotMove = rotSpeed * 10.0f;
	trackRotMove = std::clamp(trackRotMove, -200.0f, 200.0f);

	m_tireRotR -= trackRotMove * deltaTime;
	m_tireRotR += trackMove * deltaTime;

	m_tireRotL -= trackRotMove * deltaTime;
	m_tireRotL -= trackMove * deltaTime;

	// 通常タイヤ
	for (int i = 0; i < 5; i++)
	{
		m_tireR[i].GetTransform().SetWorldMatrix(m_tireOffsetR[i] * ownerMatrix);
		m_tireL[i].GetTransform().SetWorldMatrix(m_tireOffsetL[i] * ownerMatrix);

		m_tireR[i].GetTransform().SetAngle(float3(m_tireRotR, ownerAngle.y, ownerAngle.z));
		m_tireL[i].GetTransform().SetAngle(float3(m_tireRotL, ownerAngle.y + 180, ownerAngle.z));
	}

	// ミニタイヤ
	for (int i = 0; i < 2; i++)
	{
		m_miniTireR[i].GetTransform().SetWorldMatrix(m_miniTireOffsetR[i] * ownerMatrix);
		m_miniTireL[i].GetTransform().SetWorldMatrix(m_miniTireOffsetL[i] * ownerMatrix);

		m_miniTireR[i].GetTransform().SetAngle(float3(m_tireRotR, ownerAngle.y, ownerAngle.z));
		m_miniTireL[i].GetTransform().SetAngle(float3(m_tireRotL, ownerAngle.y + 180, ownerAngle.z));
	}
}

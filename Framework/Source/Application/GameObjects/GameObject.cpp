#include "GameObject.h"

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Transform::Transform()
	: m_worldPosition(float3::Zero)
	, m_worldAngle(float3::Zero)
	, m_worldScale(float3::One)
	, m_mWorkWorld(mfloat4x4::Identity)
	, m_needUpdateMatrix(true)
{
}

//-----------------------------------------------------------------------------
// 行列を返す
//-----------------------------------------------------------------------------
const mfloat4x4& Transform::GetWorldMatrix() const
{
	if (m_needUpdateMatrix)
	{
		m_needUpdateMatrix = false;

		// ワールド行列の作成

		// 拡大
		mfloat4x4 scale;
		scale = mfloat4x4::CreateScale(m_worldScale);

		// 移動
		mfloat4x4 trans;
		trans = mfloat4x4::CreateTranslation(m_worldPosition);

		// 回転
		mfloat4x4 rotation;
		rotation = mfloat4x4::CreateFromYawPitchRoll(
			DirectX::XMConvertToRadians(m_worldAngle.y),
			DirectX::XMConvertToRadians(m_worldAngle.x),
			DirectX::XMConvertToRadians(m_worldAngle.z));

		m_mWorkWorld = scale * rotation * trans;
	}

	return m_mWorkWorld;
}

//-----------------------------------------------------------------------------
// 行列の設定
//-----------------------------------------------------------------------------
void Transform::SetWorldMatrix(const mfloat4x4& matrix)
{
	m_mWorkWorld = matrix;

	m_needUpdateMatrix = false;

	// 行列からSRTを取得

	// 拡大
	m_worldScale.x = m_mWorkWorld.Right().Length();
	m_worldScale.y = m_mWorkWorld.Up().Length();
	m_worldScale.z = m_mWorkWorld.Backward().Length();

	// 座標
	m_worldPosition = m_mWorkWorld.Translation();

	// 角度
	mfloat4x4 mRota = mfloat4x4::CreateFromYawPitchRoll(
		DirectX::XMConvertToRadians(m_worldAngle.y),
		DirectX::XMConvertToRadians(m_worldAngle.x),
		DirectX::XMConvertToRadians(m_worldAngle.z));

	m_worldAngle.x = atan2f(mRota.m[1][2], mRota.m[2][2]);
	m_worldAngle.y = atan2f(-mRota.m[0][2], sqrtf(mRota.m[1][2] * mRota.m[1][2] + mRota.m[2][2] * mRota.m[2][2]));
	m_worldAngle.z = atan2f(mRota.m[0][1], mRota.m[0][0]);
}



//=============================================================================
//
// GameObject
//
//=============================================================================

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
GameObject::GameObject()
	: m_modelWork()
	, m_transform()
	, m_isActiv(true)
	, m_isEnable(true)
	, m_name("empty")
{
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void GameObject::Update(float deltaTime)
{

}

//-----------------------------------------------------------------------------
// 描画
//-----------------------------------------------------------------------------
void GameObject::Draw(float deltaTime)
{
	//// カメラ座標
	//float3 cameraPos = D3D.GetRenderer().GetCameraPos();
	//// カメラとの距離
	//float dist = float3::Distance(cameraPos, m_transform.GetPosition());
	//D3D.GetRenderer().SetDistToEye(dist);

	//m_modelWork.Transform() = m_transform.GetWorldMatrix();
	SHADER.GetModelShader().DrawModel(m_modelWork);
}

//-----------------------------------------------------------------------------
// 破棄
//-----------------------------------------------------------------------------
void GameObject::Destroy()
{
}

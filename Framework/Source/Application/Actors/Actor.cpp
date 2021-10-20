﻿#include "Actor.h"

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
// Actor
//
//=============================================================================

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Actor::Actor()
	: m_modelWork()
	, m_transform()
	, m_tag(ACTOR_TAG::eUntagged)
	, m_name("empty")
	, m_isActiv(true)
	, m_isEnable(true)
	, m_shadowCaster(true)
	, m_numUVOffset(float2::Zero)
	, m_isUVScroll(false)
{
}

//-----------------------------------------------------------------------------
// 更新
//-----------------------------------------------------------------------------
void Actor::Update(float deltaTime)
{

}

//-----------------------------------------------------------------------------
// 描画
//-----------------------------------------------------------------------------
void Actor::Draw(float deltaTime)
{
	// 定数バッファ
	{
		// カメラ座標
		float3 cameraPos = RENDERER.Getcb9().Get().m_camera_matrix.Translation();
		// カメラとの距離
		float dist = float3::Distance(cameraPos, m_transform.GetPosition());
		RENDERER.Getcb8().Work().m_dist_to_eye = dist;

		// UVスクロール
		RENDERER.Getcb8().Work().m_uv_offset = float2::Zero;
		if (m_isUVScroll)
			RENDERER.Getcb8().Work().m_uv_offset += m_numUVOffset;
		else
			RENDERER.Getcb8().Work().m_uv_offset = float2::Zero;

		RENDERER.Getcb8().Write();
	}

	// TODO: Shader分け
	SHADER.GetModelShader().DrawModel(m_modelWork, m_transform.GetWorldMatrix());
}

//-----------------------------------------------------------------------------
// シャドウマップ描画
//-----------------------------------------------------------------------------
void Actor::DrawShadowMap(float deltaTime)
{
	if (!m_shadowCaster) return;

	SHADER.GetShadowMapShader().DrawModel(m_modelWork, m_transform.GetWorldMatrix());
}

//-----------------------------------------------------------------------------
// 破棄
//-----------------------------------------------------------------------------
void Actor::Destroy()
{
}

//-----------------------------------------------------------------------------
// レイとメッシュの当たり判定
//-----------------------------------------------------------------------------
bool Actor::CheckCollision(const float3& rayPos, const float3& rayDir, float hitRange)
{
	RayResult result = {};

	auto& data = m_modelWork.GetData();
	if (data == nullptr) return false;

	for (auto& index : data->GetMeshNodeIndices())
	{
		auto& mesh = m_modelWork.GetMesh(index);
		auto& node = m_modelWork.GetNodes()[index];

		RayResult tmpResult = {};
		Collision::RayToMesh(rayPos, rayDir, hitRange, *(mesh.get()), node.m_localTransform * m_transform.GetWorldMatrix(), &tmpResult);

		// 最も近いnodeを優先
		if (tmpResult.m_distance < result.m_distance)
			result = tmpResult;
	}

	return result.m_hit;
}

//-----------------------------------------------------------------------------
// モデル読み込み
//-----------------------------------------------------------------------------
void Actor::LoadModel(const std::string& filepath)
{
	auto model = RES_FAC.GetModelData(filepath);
	if (model) m_modelWork.SetModel(model);
}

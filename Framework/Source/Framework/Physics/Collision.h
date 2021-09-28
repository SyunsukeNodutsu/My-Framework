//-----------------------------------------------------------------------------
// File: Collision.h
//
// 衝突判定
// 参考：ゲームプログラミングのためのリアルタイム衝突判定
//-----------------------------------------------------------------------------
#pragma once

// 球判定の結果
struct SphereResult
{
	float	m_distance = FLT_MAX;	// お互いの球の間の距離が入る
	bool	m_hit = false;			// ヒットしたかどうか
};

// レイ判定の結果
struct RayResult
{
	float	m_distance = FLT_MAX;	// レイの座標からヒットした位置までの距離が入る
	float3	m_HitPos = float3::Zero;// 衝突座標
	bool	m_hit = false;			// ヒットしたかどうか
};

// 衝突判定
namespace Collision
{
	// @brief 球 vs 球判定
	// @note 判定結果だけで良い場合はリザルトにnullptrを設定
	// @return 衝突...true
	bool SphereToSphere(const DirectX::XMVECTOR& rPos1, const DirectX::XMVECTOR& rPos2, float radius1, float radius2, SphereResult* pResult = nullptr);

	// @brief レイ vs メッシュ判定 : 判定結果だけで良い場合はリザルトにnullptrを渡しておく
	// @return 衝突...true
	bool RayToMesh(const DirectX::XMVECTOR& rRayPos, const DirectX::XMVECTOR& rRayDir, float maxDistance, const Mesh& rMesh, const DirectX::XMMATRIX& rMatrix, RayResult* pResult = nullptr);

	// @brief 球 vs メッシュ判定
	// @return 衝突...true
	bool SphereToMesh(const float3& rSpherePos, float radius, const Mesh& mesh, const DirectX::XMMATRIX& matrix, float3& rPushedPos);

	// @brief 点 vs 三角形との最近接点を求める
	// @return 衝突...true
	void PointToTriangle(const DirectX::XMVECTOR& p, const DirectX::XMVECTOR& a, const DirectX::XMVECTOR& b, const DirectX::XMVECTOR& c, DirectX::XMVECTOR& outPt);
}

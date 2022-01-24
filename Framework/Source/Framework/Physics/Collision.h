//-----------------------------------------------------------------------------
// File: Collision.h
//
// 衝突判定
// 参考：ゲームプログラミングのためのリアルタイム衝突判定
//-----------------------------------------------------------------------------
#pragma once

//球判定の結果
struct SphereResult
{
	float m_distance = FLT_MAX;//お互いの球の間の距離が入る
	bool m_hit = false;//ヒットしたかどうか
};

//レイ判定の結果
struct RayResult
{
	float m_distance = FLT_MAX;//レイの座標からヒットした位置までの距離
	float3 m_hitPos	= float3::Zero;//衝突座標
	bool m_hit = false;//ヒットしたかどうか
};

//衝突判定
namespace Collision
{
	//@brief 球 vs 球判定
	//@note 判定結果だけで良い場合はリザルトにnullptrを設定
	//@param pos1 球1の座標
	//@param pos2 球2の座標
	//@param radius1 球1の半径
	//@param radius2 球2の半径
	//@param pResult 結果格納用
	//@return 衝突...true
	bool SphereToSphere(const float3& pos1, const float3& pos2, float radius1, float radius2, SphereResult* pResult = nullptr);

	//@brief レイ vs メッシュ判定
	//@note 判定結果だけで良い場合はリザルトにnullptrを渡しておく
	//@param rayPos レイの座標
	//@param rayDir レイの方向
	//@param maxDistance レイの最大距離
	//@param mesh メッシュ
	//@param matrix ノードを考慮した行列
	//@param pResult 結果格納用
	//@return 衝突...true
	bool RayToMesh(const float3& rayPos, const float3& rayDir, float maxDistance, const Mesh& mesh, const DirectX::XMMATRIX& matrix, RayResult* pResult = nullptr);

	//@brief 球 vs メッシュ判定
	//@param spherePos 球の中心座標
	//@param radius 球の半径
	//@param mesh メッシュ
	//@param matrix ノードを考慮した行列
	//@param pushedPos 押し出されるベクトル
	//@return 衝突...true
	bool SphereToMesh(const float3& spherePos, float radius, const Mesh& mesh, const DirectX::XMMATRIX& matrix, float3& pushedPos);

	//@brief 点 vs 三角形との最近接点を求める
	//@param p 点の座標
	//@param a 頂点1
	//@param b 頂点2
	//@param c 頂点3
	//@param outPt 最近接座標
	void PointToTriangle(const float3& p, const float3& a, const float3& b, const float3& c, float3& outPt);

}

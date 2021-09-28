#include "Collision.h"

using namespace DirectX;

//-----------------------------------------------------------------------------
// 球 vs 球判定
//-----------------------------------------------------------------------------
bool Collision::SphereToSphere(const DirectX::XMVECTOR& rPos1, const DirectX::XMVECTOR& rPos2, float radius1, float radius2, SphereResult* pResult)
{
	// 球同士の半径を加算して、衝突判定距離を求める
	float collisionRange = radius1 + radius2;

	// 座標間の距離を求める
	float distance = float3(rPos1 - rPos2).Length();

	// 衝突判定距離よりも座標間の距離が短ければ衝突判定
	bool isCollision = (distance <= collisionRange);

	// リザルトに結果を格納
	if (isCollision && pResult)
	{
		pResult->m_distance = distance;
		pResult->m_hit = true;
	}

	return isCollision;
}

//-----------------------------------------------------------------------------
// レイ vs メッシュ判定
//-----------------------------------------------------------------------------
bool Collision::RayToMesh(const XMVECTOR& rRayPos, const XMVECTOR& rRayDir, float maxDistance, const Mesh& rMesh, const XMMATRIX& rMatrix, RayResult* pResult)
{
	//--------------------------------------------------------
	// レイの逆行列化
	//--------------------------------------------------------

	// モデルの逆行列でレイを変換
	XMMATRIX invMat = XMMatrixInverse(0, rMatrix);

	// レイの判定開始位置を逆変換
	XMVECTOR rayPos = XMVector3TransformCoord(rRayPos, invMat);

	// レイの方向を逆変換
	XMVECTOR rayDir = XMVector3TransformNormal(rRayDir, invMat);

	// レイの方向ベクトルの長さで判定用の最大距離を補正
	// ※逆行列に拡縮が入っていると、レイの長さが変わるため
	// レイが当たった座標からの距離に拡縮が反映されてしまうので
	// 判定用の最大距離にも拡縮を反映させておく
	float dirLength = XMVector3Length(rayDir).m128_f32[0];
	float maxCheckRange = maxDistance * dirLength;

	// 方角レイとして正しく扱うためには長さが１でなければならないので正規化
	rayDir = XMVector3Normalize(rayDir);

	//--------------------------------------------------------
	// ブロードフェイズ
	// 　比較的軽量なAABB vs レイな判定で、
	// 　あきらかにヒットしない場合は、これ以降の判定を中止
	//--------------------------------------------------------
	{
		// AABB vs レイ
		float AABBdist = FLT_MAX;
		if (rMesh.GetBoundingBox().Intersects(rayPos, rayDir, AABBdist) == false) { return false; }

		// 最大距離外なら範囲外なので中止
		if (AABBdist > maxCheckRange) { return false; }
	}

	//--------------------------------------------------------
	// ナローフェイズ
	// 　レイ vs 全ての面
	//--------------------------------------------------------

	// ヒット判定
	bool ret = false;
	float closestDist = FLT_MAX;

	// DEBUGビルドでも速度を維持するため、別変数に拾っておく
	const MeshFace* pFaces = &rMesh.GetFaces()[0];
	UINT faceNum = rMesh.GetFaces().size();

	// 全ての面(三角形)
	for (UINT faceIdx = 0; faceIdx < faceNum; ++faceIdx)
	{
		// 三角形を構成する３つの頂点のIndex
		const UINT* idx = pFaces[faceIdx].m_index;

		// レイと三角形の判定
		float triDist = FLT_MAX;
		bool bHit = DirectX::TriangleTests::Intersects(rayPos, rayDir,
			rMesh.GetVertexPositions()[idx[0]],
			rMesh.GetVertexPositions()[idx[1]],
			rMesh.GetVertexPositions()[idx[2]],
			triDist
		);

		// ヒットした
		if (bHit == false) { continue; }

		// 最大距離以内の場合
		if (triDist <= maxCheckRange)
		{
			ret = true;

			// 当たり判定を行うモデルの行列の拡大率が影響するので、その影響を打ち消す
			triDist /= dirLength;

			// 近いほうを残す
			if ((triDist) < closestDist)
			{
				closestDist = triDist;		// 距離を更新
			}
		}
	}

	// リザルトに結果を格納
	if (ret && pResult)
	{
		pResult->m_hit = ret;
		pResult->m_distance = closestDist;

		pResult->m_HitPos = rRayPos + rRayDir * closestDist;
	}

	return ret;
}

//-----------------------------------------------------------------------------
// 球 vs メッシュ判定
//-----------------------------------------------------------------------------
bool Collision::SphereToMesh(const float3& rSpherePos, float radius, const Mesh& mesh, const DirectX::XMMATRIX& matrix, float3& rPushedPos)
{
	//------------------------------------------
	// ブロードフェイズ
	// 　高速化のため、まずは境界ボックス(AABB)で判定
	// 　この段階でヒットしていないなら、詳細な判定をする必要なし
	//------------------------------------------
	{
		// メッシュのAABBを元に、行列で変換したAABBを作成
		BoundingBox aabb;
		mesh.GetBoundingBox().Transform(aabb, matrix);

		BoundingSphere bs(rSpherePos, radius);
		if (aabb.Intersects(bs) == false)return false;
	}

	//------------------------------------------
	// ナローフェイズ
	// 　球とメッシュとの詳細判定
	//------------------------------------------

	// １つでもヒットしたらtrue
	bool bHit = false;

	// DEBUGビルドでも速度を維持するため、別変数に拾っておく
	const auto* pFaces = &mesh.GetFaces()[0];
	UINT faceNum = mesh.GetFaces().size();
	const float3* vertices = &mesh.GetVertexPositions()[0];

	// メッシュの逆行列で、球の中心座標を変換(メッシュの座標系へ変換される)
	XMMATRIX invM = XMMatrixInverse(0, matrix);
	XMVECTOR QPos = rSpherePos;
	QPos = XMVector3TransformCoord(QPos, invM);

	// 半径の二乗(高速判定用)
	float radiusSq = radius * radius;	// 半径の２乗

	// 行列の各軸の拡大率を計算しておく
	XMVECTOR scale;
	scale.m128_f32[0] = XMVector3Length(matrix.r[0]).m128_f32[0];
	scale.m128_f32[1] = XMVector3Length(matrix.r[1]).m128_f32[0];
	scale.m128_f32[2] = XMVector3Length(matrix.r[2]).m128_f32[0];
	scale.m128_f32[3] = 0;

	// その他、作業用変数
	XMVECTOR nearPt;
	XMVECTOR vToCenter;

	// 全ての面と判定
	// ※判定はメッシュのローカル空間で行われる
	for (UINT fi = 0; fi < faceNum; fi++)
	{
		// 点 と 三角形 の最近接点を求める
		PointToTriangle(QPos, vertices[pFaces[fi].m_index[0]], vertices[pFaces[fi].m_index[1]], vertices[pFaces[fi].m_index[2]], nearPt);

		// 最近接点から、球の中心へのベクトルを求める
		vToCenter = QPos - nearPt;

		// XYZ軸が別々の大きさで拡縮されてる状態の場合、球が変形してる状態なため正確な半径がわからない。
		// そこでscaleをかけてXYZ軸のスケールが均等な座標系へ変換する
		vToCenter *= scale;

		// 最近接点が半径以内の場合は、衝突している
		// ※LengthSq関数はベクトルの長さの二乗を返す。Length関数に比べて平方根の計算が入らない分高速
		if (XMVector3LengthSq(vToCenter).m128_f32[0] <= radiusSq)
		{
			// 最近接点を出力データに記憶する
			nearPt = XMVector3TransformCoord(nearPt, matrix);

			// 押し戻し計算
			// めり込んでいるぶんのベクトルを計算
			XMVECTOR v = XMVector3Normalize(vToCenter);
			v *= radius - XMVector3Length(vToCenter).m128_f32[0];

			// 拡縮を考慮した座標系へ戻す
			v /= scale;

			// 球の座標を移動させる
			QPos += v;

			// ヒットしたよフラグ
			bHit = true;
		}
	}

	// 押し戻された球の座標を格納
	if (bHit)
	{
		rPushedPos = XMVector3TransformCoord(QPos, matrix);
	}

	return bHit;
}

//-----------------------------------------------------------------------------
// 点 vs 三角形との最近接点を求める
//-----------------------------------------------------------------------------
void Collision::PointToTriangle(const XMVECTOR& p, const XMVECTOR& a, const XMVECTOR& b, const XMVECTOR& c, DirectX::XMVECTOR& outPt)
{
	// ※参考:[書籍]「ゲームプログラミングのためのリアルタイム衝突判定」

	// pがaの外側の頂点領域の中にあるかどうかチェック
	XMVECTOR ab = b - a;
	XMVECTOR ac = c - a;
	XMVECTOR ap = p - a;

	float d1 = XMVector3Dot(ab, ap).m128_f32[0];//ab.Dot(ap);
	float d2 = XMVector3Dot(ac, ap).m128_f32[0];//ac.Dot(ap);
	if (d1 <= 0.0f && d2 <= 0.0f) {
		outPt = a;	// 重心座標(1,0,0)
		return;
	}

	// pがbの外側の頂点領域の中にあるかどうかチェック
	XMVECTOR bp = p - b;
	float d3 = XMVector3Dot(ab, bp).m128_f32[0];//ab.Dot(bp);
	float d4 = XMVector3Dot(ac, bp).m128_f32[0];//ac.Dot(bp);
	if (d3 >= 0.0f && d4 <= d3) {
		outPt = b;	// 重心座標(0,1,0)
		return;
	}


	// pがabの辺領域の中にあるかどうかチェックし、あればpのab上に対する射影を返す
	float vc = d1 * d4 - d3 * d2;
	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
		float v = d1 / (d1 - d3);
		outPt = a + ab * v;	// 重心座標(1-v,v,0)
		return;
	}

	// pがcの外側の頂点領域の中にあるかどうかチェック
	XMVECTOR cp = p - c;
	float d5 = XMVector3Dot(ab, cp).m128_f32[0];//ab.Dot(cp);
	float d6 = XMVector3Dot(ac, cp).m128_f32[0];;//ac.Dot(cp);
	if (d6 >= 0.0f && d5 <= d6) {
		outPt = c;	// 重心座標(0,0,1)
		return;
	}


	// pがacの辺領域の中にあるかどうかチェックし、あればpのac上に対する射影を返す
	float vb = d5 * d2 - d1 * d6;
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
		float w = d2 / (d2 - d6);
		outPt = a + ac * w;	// 重心座標(1-w,0,w)
		return;
	}

	// pがbcの辺領域の中にあるかどうかチェックし、あればpのbc上に対する射影を返す
	float va = d3 * d6 - d5 * d4;
	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		outPt = b + (c - b) * w;	// 重心座標(0,1-w,w)
		return;
	}

	// pは面領域の中にある。Qをその重心座標(u,v,w)を用いて計算
	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;
	outPt = a + ab * v + ac * w;	// = u*a + v*b + w*c, u = va*demon = 1.0f - v - w
}

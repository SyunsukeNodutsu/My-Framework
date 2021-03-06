#include "Animation.h"

//@brief 二分探索で 指定時間から次の配列要素のKeyIndexを求める
//@param list キー配列
//@param time 時間
//@return 次の配列要素のIndex
template<class T>
int BinarySearchNextAnimKey(const std::vector<T>& list, float time)
{
	int low = 0;
	int high = (int)list.size();
	while (low < high)
	{
		int mid = (low + high) / 2;
		float midTime = list[mid].m_time;

		if (midTime <= time) low = mid + 1;
		else high = mid;
	}
	return low;
}

//-----------------------------------------------------------------------------
//座標補完
//-----------------------------------------------------------------------------
bool AnimationData::Node::InterpolateTranslations(float3& result, float time)
{
	if (m_translations.size() == 0) return false;

	//キー位置検索
	UINT keyIdx = BinarySearchNextAnimKey(m_translations, time);

	//先頭のキーなら 先頭のデータを返す
	if (keyIdx == 0)
	{
		result = m_translations.front().m_numVec;
		return true;
	}
	//配列外のキーなら 最後のデータを返す
	else if (keyIdx >= m_translations.size())
	{
		result = m_translations.back().m_numVec;
		return true;
	}
	//それ以外(中間の時間)なら その時間の値を補間計算で求める
	else
	{
		auto& prev = m_translations[keyIdx - 1];	//前のキー
		auto& next = m_translations[keyIdx];		//次のキー
		//前のキーと次のキーの時間から、0～1間の時間を求める
		float f = (time - prev.m_time) / (next.m_time - prev.m_time);
		//補間
		result = DirectX::XMVectorLerp(
			prev.m_numVec,
			next.m_numVec,
			f
		);
	}

	return true;
}

//-----------------------------------------------------------------------------
//回転補完
//-----------------------------------------------------------------------------
bool AnimationData::Node::InterpolateRotations(qfloat4x4& result, float time)
{
	if (m_rotations.size() == 0)return false;

	// キー位置検索
	UINT keyIdx = BinarySearchNextAnimKey(m_rotations, time);
	// 先頭のキーなら、先頭のデータを返す
	if (keyIdx == 0) {
		result = m_rotations.front().m_numRot;
	}
	// 配列外のキーなら、最後のデータを返す
	else if (keyIdx >= m_rotations.size()) {
		result = m_rotations.back().m_numRot;
	}
	// それ以外(中間の時間)なら、その時間の値を補間計算で求める
	else {
		auto& prev = m_rotations[keyIdx - 1];	// 前のキー
		auto& next = m_rotations[keyIdx];		// 次のキー
		// 前のキーと次のキーの時間から、0～1間の時間を求める
		float f = (time - prev.m_time) / (next.m_time - prev.m_time);
		// 補間
		result = DirectX::XMQuaternionSlerp(
			prev.m_numRot,
			next.m_numRot,
			f
		);
	}

	return true;
}

//-----------------------------------------------------------------------------
//拡縮補完
//-----------------------------------------------------------------------------
bool AnimationData::Node::InterpolateScales(float3& result, float time)
{
	if (m_scales.size() == 0)return false;

	// キー位置検索
	UINT keyIdx = BinarySearchNextAnimKey(m_scales, time);

	// 先頭のキーなら、先頭のデータを返す
	if (keyIdx == 0) {
		result = m_scales.front().m_numVec;
		return true;
	}
	// 配列外のキーなら、最後のデータを返す
	else if (keyIdx >= m_scales.size()) {
		result = m_scales.back().m_numVec;
		return true;
	}
	// それ以外(中間の時間)なら、その時間の値を補間計算で求める
	else {
		auto& prev = m_scales[keyIdx - 1];	// 前のキー
		auto& next = m_scales[keyIdx];		// 次のキー
		// 前のキーと次のキーの時間から、0～1間の時間を求める
		float f = (time - prev.m_time) / (next.m_time - prev.m_time);
		// 補間
		result = DirectX::XMVectorLerp(
			prev.m_numVec,
			next.m_numVec,
			f
		);
	}

	return true;
}

//-----------------------------------------------------------------------------
//補完
//-----------------------------------------------------------------------------
void AnimationData::Node::Interpolate(mfloat4x4& rDst, float time)
{
	//ベクターによる拡縮補間
	bool isChange = false;
	mfloat4x4 scale;
	float3 resultVec;
	if (InterpolateScales(resultVec, time))
	{
		scale = scale.CreateScale(resultVec);
		isChange = true;
	}

	//クォタニオンによる回転補間
	mfloat4x4 rotate;
	qfloat4x4 resultQuat;
	if (InterpolateRotations(resultQuat, time))
	{
		rotate = rotate.CreateFromQuaternion(resultQuat);
		isChange = true;
	}

	//ベクターによる座標補間
	mfloat4x4 trans;
	if (InterpolateTranslations(resultVec, time))
	{
		trans = trans.CreateTranslation(resultVec);
		isChange = true;
	}

	if (isChange)
		rDst = scale * rotate * trans;
}



//=============================================================================
// 
// Animator
// 
//=============================================================================

//-----------------------------------------------------------------------------
//アニメーションの更新
//-----------------------------------------------------------------------------
void Animator::AdvanceTime(std::vector<ModelWork::Node>& nodes, float speed)
{
	if (!m_spAnimation) return;

	//全てのアニメーションノード（モデルの行列を補間する情報）の行列補間を実行
	for (auto& rAnimNode : m_spAnimation->m_nodes)
	{
		//対応するモデルノードのインデックス
		UINT idx = rAnimNode.m_nodeOffset;

		auto prev = nodes[idx].m_localTransform;

		//アニメーションデータによる行列補間
		rAnimNode.Interpolate(nodes[idx].m_localTransform, m_time);

		prev = nodes[idx].m_localTransform;
	}

	//アニメーションのフレームを進める
	m_time += speed;

	//アニメーションデータの最後のフレームを超えた
	if (m_time >= m_spAnimation->m_maxLength)
	{
		m_time = m_isLoop ? 0.0f : m_spAnimation->m_maxLength;
	}
}

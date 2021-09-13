﻿#include "Animation.h"

// @brief 二分探索で指定時間から次の配列要素のKeyIndexを求める
// @param list キー配列
// @param time 時間
// @return 次の配列要素のIndex
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

//=============================================================================
//
// AnimationData
//
//=============================================================================

//-----------------------------------------------------------------------------
// 座標補完
//-----------------------------------------------------------------------------
bool AnimationData::Node::InterpolateTranslations(float3& result, float time)
{
	if (m_translations.size() == 0)
		return false;

	// キー位置検索
	UINT keyIdx = BinarySearchNextAnimKey(m_translations, time);

	// 先頭のキーなら、先頭のデータを返す
	if (keyIdx == 0)
	{
		result = m_translations.front().m_vec;
		return true;
	}
	// 配列外のキーなら、最後のデータを返す
	else if (keyIdx >= m_translations.size())
	{
		result = m_translations.back().m_vec;
		return true;
	}
	// それ以外(中間の時間)なら、その時間の値を補間計算で求める
	else
	{
		auto& prev = m_translations[keyIdx - 1];	// 前のキー
		auto& next = m_translations[keyIdx];		// 次のキー
		// 前のキーと次のキーの時間から、0～1間の時間を求める
		float f = (time - prev.m_time) / (next.m_time - prev.m_time);
		// 補間
		result = DirectX::XMVectorLerp(prev.m_vec, next.m_vec, f);
	}
	return true;
}

//-----------------------------------------------------------------------------
// 回転補完
//-----------------------------------------------------------------------------
bool AnimationData::Node::InterpolateRotations(qfloat4x4& result, float time)
{
	if (m_rotations.size() == 0)
		return false;

	// キー位置検索
	UINT keyIdx = BinarySearchNextAnimKey(m_rotations, time);

	// 先頭のキーなら、先頭のデータを返す
	if (keyIdx == 0)
	{
		result = m_rotations.front().m_quat;
	}
	// 配列外のキーなら、最後のデータを返す
	else if (keyIdx >= m_rotations.size())
	{
		result = m_rotations.back().m_quat;
	}
	// それ以外(中間の時間)なら、その時間の値を補間計算で求める
	else
	{
		auto& prev = m_rotations[keyIdx - 1];	// 前のキー
		auto& next = m_rotations[keyIdx];		// 次のキー
		// 前のキーと次のキーの時間から、0～1間の時間を求める
		float f = (time - prev.m_time) / (next.m_time - prev.m_time);
		// 補間
		result = DirectX::XMQuaternionSlerp(prev.m_quat, next.m_quat, f);
	}
	return true;
}

//-----------------------------------------------------------------------------
// 拡縮補完
//-----------------------------------------------------------------------------
bool AnimationData::Node::InterpolateScales(float3& result, float time)
{
	if (m_scales.size() == 0)
		return false;

	// キー位置検索
	UINT keyIdx = BinarySearchNextAnimKey(m_scales, time);

	// 先頭のキーなら、先頭のデータを返す
	if (keyIdx == 0)
	{
		result = m_scales.front().m_vec;
		return true;
	}
	// 配列外のキーなら、最後のデータを返す
	else if (keyIdx >= m_scales.size())
	{
		result = m_scales.back().m_vec;
		return true;
	}
	// それ以外(中間の時間)なら、その時間の値を補間計算で求める
	else
	{
		auto& prev = m_scales[keyIdx - 1];	// 前のキー
		auto& next = m_scales[keyIdx];		// 次のキー
		// 前のキーと次のキーの時間から、0～1間の時間を求める
		float f = (time - prev.m_time) / (next.m_time - prev.m_time);
		// 補間
		result = DirectX::XMVectorLerp(prev.m_vec, next.m_vec, f);
	}
	return true;
}

//-----------------------------------------------------------------------------
// 補完
//-----------------------------------------------------------------------------
void AnimationData::Node::Interpolate(mfloat4x4& dstMatrix, float time)
{
	bool isChange = false;

	float3 resultVec = float3::Zero;
	qfloat4x4 resultQuat = qfloat4x4::Identity;

	// 座標補間
	mfloat4x4 trans;
	if (InterpolateTranslations(resultVec, time))
	{
		trans = mfloat4x4::CreateTranslation(resultVec);
		isChange = true;
	}

	// 拡縮補間
	mfloat4x4 scale;
	if (InterpolateScales(resultVec, time))
	{
		scale = mfloat4x4::CreateScale(resultVec);
		isChange = true;
	}

	// 回転補間
	mfloat4x4 rotate = mfloat4x4::Identity;
	if (InterpolateRotations(resultQuat, time))
	{
		rotate = mfloat4x4::CreateFromQuaternion(resultQuat);
		isChange = true;
	}

	if (!isChange)
		return;

	dstMatrix = scale * rotate * trans;
}



//=============================================================================
//
// Animator
//
//=============================================================================

//-----------------------------------------------------------------------------
// コンストラクタ
//-----------------------------------------------------------------------------
Animator::Animator()
	: m_spAnimation(nullptr)
	, m_time(0.0f)
	, m_isLoop(false)
{
}

//-----------------------------------------------------------------------------
// アニメーション更新
//-----------------------------------------------------------------------------
void Animator::AdvanceTime(std::vector<ModelWork::Node>& nodes, float speed)
{
	if (m_spAnimation == nullptr)
		return;

	for (auto& node : m_spAnimation->m_nodes)
	{
		// 対応するモデルノードのインデックス
		UINT idx = node.m_nodeOffset;

		auto prev = nodes[idx].m_localTransform;

		// アニメーションデータによる行列補間
		node.Interpolate(nodes[idx].m_localTransform, m_time);

		prev = nodes[idx].m_localTransform;
	}

	m_time += speed;

	// 終了 or ループ
	if (m_time >= m_spAnimation->m_maxLength)
		m_time = (m_isLoop) ? 0.0f : m_spAnimation->m_maxLength;
}
